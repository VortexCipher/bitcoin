// Copyright 2016 Wladimir J. van der Laan
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_UNIVALUE_INCLUDE_UNIVALUE_UTFFILTER_H
#define BITCOIN_UNIVALUE_INCLUDE_UNIVALUE_UTFFILTER_H

#include <string>

/**
 * Filter that generates and validates UTF-8, as well as collates UTF-16
 * surrogate pairs as specified in RFC4627.
