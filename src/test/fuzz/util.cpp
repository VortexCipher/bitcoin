// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <consensus/amount.h>
#include <pubkey.h>
#include <test/fuzz/util.h>
#include <test/util/script.h>
#include <util/check.h>
#include <util/overflow.h>
#include <util/rbf.h>
#include <util/time.h>

#include <memory>

std::vector<uint8_t> ConstructPubKeyBytes(FuzzedDataProvider& fuzzed_data_provider, Span<const uint8_t> byte_data, const bool compressed) noexcept
{
    uint8_t pk_type;
    if (compressed) {
        pk_type = fuzzed_data_provider.PickValueInArray({0x02, 0x03});
    } else {
        pk_type = fuzzed_data_provider.PickValueInArray({0x04, 0x06, 0x07});
    }
    std::vector<uint8_t> pk_data{byte_data.begin(), byte_data.begin() + (compressed ? CPubKey::COMPRESSED_SIZE : CPubKey::SIZE)};
    pk_data[0] = pk_type;
    return pk_data;
}

CAmount ConsumeMoney(FuzzedDataProvider& fuzzed_data_provider, const std::optional<CAmount>& max) noexcept
{
    return fuzzed_data_provider.ConsumeIntegralInRange<CAmount>(0, max.value_or(MAX_MONEY));
}

int64_t ConsumeTime(FuzzedDataProvider& fuzzed_data_provider, const std::optional<int64_t>& min, const std::optional<int64_t>& max) noexcept
{
