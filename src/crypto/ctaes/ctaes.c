 /*********************************************************************
 * Copyright (c) 2016 Pieter Wuille                                   *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

/* Constant time, unoptimized, concise, plain C, AES implementation
 * Based On:
 *   Emilia Kasper and Peter Schwabe, Faster and Timing-Attack Resistant AES-GCM
 *   http://www.iacr.org/archive/ches2009/57470001/57470001.pdf
 * But using 8 16-bit integers representing a single AES state rather than 8 128-bit
 * integers representing 8 AES states.
 */

#include "ctaes.h"

/* Slice variable slice_i contains the i'th bit of the 16 state variables in this order:
 *  0  1  2  3
 *  4  5  6  7
 *  8  9 10 11
 * 12 13 14 15
 */

/** Convert a byte to sliced form, storing it corresponding to given row and column in s */
static void LoadByte(AES_state* s, unsigned char byte, int r, int c) {
    int i;
    for (i = 0; i < 8; i++) {
        s->slice[i] |= (byte & 1) << (r * 4 + c);
        byte >>= 1;
    }
}

/** Load 16 bytes of data into 8 sliced integers */
static void LoadBytes(AES_state *s, const unsigned char* data16) {
    int c;
    for (c = 0; c < 4; c++) {
        int r;
        for (r = 0; r < 4; r++) {
            LoadByte(s, *(data16++), r, c);
        }
    }
}

/** Convert 8 sliced integers into 16 bytes of data */
static void SaveBytes(unsigned char* data16, const AES_state *s) {
    int c;
    for (c = 0; c < 4; c++) {
        int r;
        for (r = 0; r < 4; r++) {
            int b;
            uint8_t v = 0;
            for (b = 0; b < 8; b++) {
                v |= ((s->slice[b] >> (r * 4 + c)) & 1) << b;
            }
            *(data16++) = v;
