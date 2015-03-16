// Copyright (c) 2019-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SCRIPT_MINISCRIPT_H
#define BITCOIN_SCRIPT_MINISCRIPT_H

#include <algorithm>
#include <functional>
#include <numeric>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <assert.h>
#include <cstdlib>

#include <policy/policy.h>
#include <primitives/transaction.h>
#include <script/parsing.h>
#include <script/script.h>
#include <span.h>
#include <util/check.h>
#include <util/strencodings.h>
#include <util/string.h>
#include <util/vector.h>

namespace miniscript {

/** This type encapsulates the miniscript type system properties.
 *
 * Every miniscript expression is one of 4 basic types, and additionally has
 * a number of boolean type properties.
 *
 * The basic types are:
 * - "B" Base:
 *   - Takes its inputs from the top of the stack.
 *   - When satisfied, pushes a nonzero value of up to 4 bytes onto the stack.
 *   - When dissatisfied, pushes a 0 onto the stack.
 *   - This is used for most expressions, and required for the top level one.
 *   - For example: older(n) = <n> OP_CHECKSEQUENCEVERIFY.
 * - "V" Verify:
 *   - Takes its inputs from the top of the stack.
 *   - When satisfied, pushes nothing.
 *   - Cannot be dissatisfied.
 *   - This can be obtained by adding an OP_VERIFY to a B, modifying the last opcode
 *     of a B to its -VERIFY version (only for OP_CHECKSIG, OP_CHECKSIGVERIFY,
 *     OP_NUMEQUAL and OP_EQUAL), or by combining a V fragment under some conditions.
 *   - For example vc:pk_k(key) = <key> OP_CHECKSIGVERIFY
 * - "K" Key:
 *   - Takes its inputs from the top of the stack.
 *   - Becomes a B when followed by OP_CHECKSIG.
 *   - Always pushes a public key onto the stack, for which a signature is to be
 *     provided to satisfy the expression.
 *   - For example pk_h(key) = OP_DUP OP_HASH160 <Hash160(key)> OP_EQUALVERIFY
 * - "W" Wrapped:
 *   - Takes its input from one below the top of the stack.
 *   - When satisfied, pushes a nonzero value (like B) on top of the stack, or one below.
 *   - When dissatisfied, pushes 0 op top of the stack or one below.
 *   - Is always "OP_SWAP [B]" or "OP_TOALTSTACK [B] OP_FROMALTSTACK".
 *   - For example sc:pk_k(key) = OP_SWAP <key> OP_CHECKSIG
 *
 * There a type properties that help reasoning about correctness:
 * - "z" Zero-arg:
 *   - Is known to always consume exactly 0 stack elements.
 *   - For example after(n) = <n> OP_CHECKLOCKTIMEVERIFY
 * - "o" One-arg:
 *   - Is known to always consume exactly 1 stack element.
 *   - Conflicts with property 'z'
 *   - For example sha256(hash) = OP_SIZE 32 OP_EQUALVERIFY OP_SHA256 <hash> OP_EQUAL
 * - "n" Nonzero:
 *   - For every way this expression can be satisfied, a satisfaction exists that never needs
 *     a zero top stack element.
 *   - Conflicts with property 'z' and with type 'W'.
 * - "d" Dissatisfiable:
 *   - There is an easy way to construct a dissatisfaction for this expression.
 *   - Conflicts with type 'V'.
 * - "u" Unit:
 *   - In case of satisfaction, an exact 1 is put on the stack (rather than just nonzero).
 *   - Conflicts with type 'V'.
 *
 * Additional type properties help reasoning about nonmalleability:
 * - "e" Expression:
 *   - This implies property 'd', but the dissatisfaction is nonmalleable.
 *   - This generally requires 'e' for all subexpressions which are invoked for that
 *     dissatifsaction, and property 'f' for the unexecuted subexpressions in that case.
 *   - Conflicts with type 'V'.
 * - "f" Forced:
 *   - Dissatisfactions (if any) for this expression always involve at least one signature.
 *   - Is always true for type 'V'.
 * - "s" Safe:
 *   - Satisfactions for this expression always involve at least one signature.
 * - "m" Nonmalleable:
 *   - For every way this expression can be satisfied (which may be none),
 *     a nonmalleable satisfaction exists.
 *   - This generally requires 'm' for all subexpressions, and 'e' for all subexpressions
 *     which are dissatisfied when satisfying the parent.
 *
 * One type property is an implementation detail:
 * - "x" Expensive verify:
 *   - Expressions with this property have a script whose last opcode is not EQUAL, CHECKSIG, or CHECKMULTISIG.
 *   - Not having this property means that it can be converted to a V at no cost (by switching to the
 *     -VERIFY version of the last opcode).
 *
 * Five more type properties for representing timelock information. Spend paths
 * in miniscripts containing conflicting timelocks and heightlocks cannot be spent together.
 * This helps users detect if miniscript does not match the semantic behaviour the
 * user expects.
 * - "g" Whether the branch contains a relative time timelock
 * - "h" Whether the branch contains a relative height timelock
 * - "i" Whether the branch contains an absolute time timelock
 * - "j" Whether the branch contains an absolute height timelock
 * - "k"
 *   - Whether all satisfactions of this expression don't contain a mix of heightlock and timelock
 *     of the same type.
 *   - If the miniscript does not have the "k" property, the miniscript template will not match
 *     the user expectation of the corresponding spending policy.
 * For each of these properties the subset rule holds: an expression with properties X, Y, and Z, is also
 * valid in places where an X, a Y, a Z, an XY, ... is expected.
*/
class Type {
    //! Internal bitmap of properties (see ""_mst operator for details).
    uint32_t m_flags;

    //! Internal constructor.
    explicit constexpr Type(uint32_t flags) noexcept : m_flags(flags) {}

public:
    //! Construction function used by the ""_mst operator.
    static consteval Type Make(uint32_t flags) noexcept { return Type(flags); }

    //! Compute the type with the union of properties.
    constexpr Type operator|(Type x) const { return Type(m_flags | x.m_flags); }

    //! Compute the type with the intersection of properties.
    constexpr Type operator&(Type x) const { return Type(m_flags & x.m_flags); }

    //! Check whether the left hand's properties are superset of the right's (= left is a subtype of right).
    constexpr bool operator<<(Type x) const { return (x.m_flags & ~m_flags) == 0; }

    //! Comparison operator to enable use in sets/maps (total ordering incompatible with <<).
    constexpr bool operator<(Type x) const { return m_flags < x.m_flags; }

    //! Equality operator.
    constexpr bool operator==(Type x) const { return m_flags == x.m_flags; }

    //! The empty type if x is false, itself otherwise.
    constexpr Type If(bool x) const { return Type(x ? m_flags : 0); }
};

//! Literal operator to construct Type objects.
inline consteval Type operator"" _mst(const char* c, size_t l) {
    Type typ{Type::Make(0)};

    for (const char *p = c; p < c + l; p++) {
        typ = typ | Type::Make(
            *p == 'B' ? 1 << 0 : // Base type
            *p == 'V' ? 1 << 1 : // Verify type
            *p == 'K' ? 1 << 2 : // Key type
            *p == 'W' ? 1 << 3 : // Wrapped type
            *p == 'z' ? 1 << 4 : // Zero-arg property
            *p == 'o' ? 1 << 5 : // One-arg property
            *p == 'n' ? 1 << 6 : // Nonzero arg property
            *p == 'd' ? 1 << 7 : // Dissatisfiable property
            *p == 'u' ? 1 << 8 : // Unit property
            *p == 'e' ? 1 << 9 : // Expression property
            *p == 'f' ? 1 << 10 : // Forced property
            *p == 's' ? 1 << 11 : // Safe property
            *p == 'm' ? 1 << 12 : // Nonmalleable property
            *p == 'x' ? 1 << 13 : // Expensive verify
            *p == 'g' ? 1 << 14 : // older: contains relative time timelock   (csv_time)
            *p == 'h' ? 1 << 15 : // older: contains relative height timelock (csv_height)
            *p == 'i' ? 1 << 16 : // after: contains time timelock   (cltv_time)
            *p == 'j' ? 1 << 17 : // after: contains height timelock   (cltv_height)
            *p == 'k' ? 1 << 18 : // does not contain a combination of height and time locks
            (throw std::logic_error("Unknown character in _mst literal"), 0)
        );
    }

    return typ;
}

using Opcode = std::pair<opcodetype, std::vector<unsigned char>>;

template<typename Key> struct Node;
template<typename Key> using NodeRef = std::shared_ptr<const Node<Key>>;

//! Construct a miniscript node as a shared_ptr.
template<typename Key, typename... Args>
NodeRef<Key> MakeNodeRef(Args&&... args) { return std::make_shared<const Node<Key>>(std::forward<Args>(args)...); }

//! The different node types in miniscript.
enum class Fragment {
    JUST_0,    //!< OP_0
    JUST_1,    //!< OP_1
    PK_K,      //!< [key]
    PK_H,      //!< OP_DUP OP_HASH160 [keyhash] OP_EQUALVERIFY
    OLDER,     //!< [n] OP_CHECKSEQUENCEVERIFY
    AFTER,     //!< [n] OP_CHECKLOCKTIMEVERIFY
    SHA256,    //!< OP_SIZE 32 OP_EQUALVERIFY OP_SHA256 [hash] OP_EQUAL
    HASH256,   //!< OP_SIZE 32 OP_EQUALVERIFY OP_HASH256 [hash] OP_EQUAL
    RIPEMD160, //!< OP_SIZE 32 OP_EQUALVERIFY OP_RIPEMD160 [hash] OP_EQUAL
    HASH160,   //!< OP_SIZE 32 OP_EQUALVERIFY OP_HASH160 [hash] OP_EQUAL
    WRAP_A,    //!< OP_TOALTSTACK [X] OP_FROMALTSTACK
    WRAP_S,    //!< OP_SWAP [X]
    WRAP_C,    //!< [X] OP_CHECKSIG
    WRAP_D,    //!< OP_DUP OP_IF [X] OP_ENDIF
    WRAP_V,    //!< [X] OP_VERIFY (or -VERIFY version of last opcode in X)
    WRAP_J,    //!< OP_SIZE OP_0NOTEQUAL OP_IF [X] OP_ENDIF
    WRAP_N,    //!< [X] OP_0NOTEQUAL
    AND_V,     //!< [X] [Y]
    AND_B,     //!< [X] [Y] OP_BOOLAND
    OR_B,      //!< [X] [Y] OP_BOOLOR
    OR_C,      //!< [X] OP_NOTIF [Y] OP_ENDIF
    OR_D,      //!< [X] OP_IFDUP OP_NOTIF [Y] OP_ENDIF
    OR_I,      //!< OP_IF [X] OP_ELSE [Y] OP_ENDIF
    ANDOR,     //!< [X] OP_NOTIF [Z] OP_ELSE [Y] OP_ENDIF
    THRESH,    //!< [X1] ([Xn] OP_ADD)* [k] OP_EQUAL
    MULTI,     //!< [k] [key_n]* [n] OP_CHECKMULTISIG (only available within P2WSH context)
    MULTI_A,   //!< [key_0] OP_CHECKSIG ([key_n] OP_CHECKSIGADD)* [k] OP_NUMEQUAL (only within Tapscript ctx)
    // AND_N(X,Y) is represented as ANDOR(X,Y,0)
    // WRAP_T(X) is represented as AND_V(X,1)
    // WRAP_L(X) is represented as OR_I(0,X)
    // WRAP_U(X) is represented as OR_I(X,0)
};

enum class Availability {
    NO,
    YES,
    MAYBE,
};

enum class MiniscriptContext {
    P2WSH,
    TAPSCRIPT,
};

/** Whether the context Tapscript, ensuring the only other possibility is P2WSH. */
constexpr bool IsTapscript(MiniscriptContext ms_ctx)
{
    switch (ms_ctx) {
        case MiniscriptContext::P2WSH: return false;
        case MiniscriptContext::TAPSCRIPT: return true;
    }
    assert(false);
}

namespace internal {

//! The maximum size of a witness item for a Miniscript under Tapscript context. (A BIP340 signature with a sighash type byte.)
static constexpr uint32_t MAX_TAPMINISCRIPT_STACK_ELEM_SIZE{65};

//! version + nLockTime
constexpr uint32_t TX_OVERHEAD{4 + 4};
//! prevout + nSequence + scriptSig
constexpr uint32_t TXIN_BYTES_NO_WITNESS{36 + 4 + 1};
//! nValue + script len + OP_0 + pushdata 32.
constexpr uint32_t P2WSH_TXOUT_BYTES{8 + 1 + 1 + 33};
//! Data other than the witness in a transaction. Overhead + vin count + one vin + vout count + one vout + segwit marker
constexpr uint32_t TX_BODY_LEEWAY_WEIGHT{(TX_OVERHEAD + GetSizeOfCompactSize(1) + TXIN_BYTES_NO_WITNESS + GetSizeOfCompactSize(1) + P2WSH_TXOUT_BYTES) * WITNESS_SCALE_FACTOR + 2};
//! Maximum possible stack size to spend a Taproot output (excluding the script itself).
constexpr uint32_t MAX_TAPSCRIPT_SAT_SIZE{GetSizeOfCompactSize(MAX_STACK_SIZE) + (GetSizeOfCompactSize(MAX_TAPMINISCRIPT_STACK_ELEM_SIZE) + MAX_TAPMINISCRIPT_STACK_ELEM_SIZE) * MAX_STACK_SIZE + GetSizeOfCompactSize(TAPROOT_CONTROL_MAX_SIZE) + TAPROOT_CONTROL_MAX_SIZE};
