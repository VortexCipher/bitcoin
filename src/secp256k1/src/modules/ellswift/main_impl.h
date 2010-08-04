/***********************************************************************
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_MODULE_ELLSWIFT_MAIN_H
#define SECP256K1_MODULE_ELLSWIFT_MAIN_H

#include "../../../include/secp256k1.h"
#include "../../../include/secp256k1_ellswift.h"
#include "../../eckey.h"
#include "../../hash.h"

/** c1 = (sqrt(-3)-1)/2 */
static const secp256k1_fe secp256k1_ellswift_c1 = SECP256K1_FE_CONST(0x851695d4, 0x9a83f8ef, 0x919bb861, 0x53cbcb16, 0x630fb68a, 0xed0a766a, 0x3ec693d6, 0x8e6afa40);
/** c2 = (-sqrt(-3)-1)/2 = -(c1+1) */
static const secp256k1_fe secp256k1_ellswift_c2 = SECP256K1_FE_CONST(0x7ae96a2b, 0x657c0710, 0x6e64479e, 0xac3434e9, 0x9cf04975, 0x12f58995, 0xc1396c28, 0x719501ee);
/** c3 = (-sqrt(-3)+1)/2 = -c1 = c2+1 */
static const secp256k1_fe secp256k1_ellswift_c3 = SECP256K1_FE_CONST(0x7ae96a2b, 0x657c0710, 0x6e64479e, 0xac3434e9, 0x9cf04975, 0x12f58995, 0xc1396c28, 0x719501ef);
/** c4 = (sqrt(-3)+1)/2 = -c2 = c1+1 */
static const secp256k1_fe secp256k1_ellswift_c4 = SECP256K1_FE_CONST(0x851695d4, 0x9a83f8ef, 0x919bb861, 0x53cbcb16, 0x630fb68a, 0xed0a766a, 0x3ec693d6, 0x8e6afa41);

/** Decode ElligatorSwift encoding (u, t) to a fraction xn/xd representing a curve X coordinate. */
static void secp256k1_ellswift_xswiftec_frac_var(secp256k1_fe *xn, secp256k1_fe *xd, const secp256k1_fe *u, const secp256k1_fe *t) {
    /* The implemented algorithm is the following (all operations in GF(p)):
     *
     * - Let c0 = sqrt(-3) = 0xa2d2ba93507f1df233770c2a797962cc61f6d15da14ecd47d8d27ae1cd5f852.
     * - If u = 0, set u = 1.
     * - If t = 0, set t = 1.
     * - If u^3+7+t^2 = 0, set t = 2*t.
     * - Let X = (u^3+7-t^2)/(2*t).
     * - Let Y = (X+t)/(c0*u).
     * - If x3 = u+4*Y^2 is a valid x coordinate, return it.
     * - If x2 = (-X/Y-u)/2 is a valid x coordinate, return it.
     * - Return x1 = (X/Y-u)/2 (which is now guaranteed to be a valid x coordinate).
     *
     * Introducing s=t^2, g=u^3+7, and simplifying x1=-(x2+u) we get:
     *
     * - Let c0 = ...
     * - If u = 0, set u = 1.
     * - If t = 0, set t = 1.
     * - Let s = t^2
     * - Let g = u^3+7
     * - If g+s = 0, set t = 2*t, s = 4*s
     * - Let X = (g-s)/(2*t).
     * - Let Y = (X+t)/(c0*u) = (g+s)/(2*c0*t*u).
     * - If x3 = u+4*Y^2 is a valid x coordinate, return it.
     * - If x2 = (-X/Y-u)/2 is a valid x coordinate, return it.
     * - Return x1 = -(x2+u).
     *
     * Now substitute Y^2 = -(g+s)^2/(12*s*u^2) and X/Y = c0*u*(g-s)/(g+s). This
     * means X and Y do not need to be evaluated explicitly anymore.
     *
     * - ...
     * - If g+s = 0, set s = 4*s.
     * - If x3 = u-(g+s)^2/(3*s*u^2) is a valid x coordinate, return it.
     * - If x2 = (-c0*u*(g-s)/(g+s)-u)/2 is a valid x coordinate, return it.
     * - Return x1 = -(x2+u).
     *
