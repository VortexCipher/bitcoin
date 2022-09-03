/***********************************************************************
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef SECP256K1_MODULE_ELLSWIFT_TESTS_H
#define SECP256K1_MODULE_ELLSWIFT_TESTS_H

#include "../../../include/secp256k1_ellswift.h"

struct ellswift_xswiftec_inv_test {
    int enc_bitmap;
    secp256k1_fe u;
    secp256k1_fe x;
    secp256k1_fe encs[8];
};

struct ellswift_decode_test {
    unsigned char enc[64];
    secp256k1_fe x;
    int odd_y;
};

struct ellswift_xdh_test {
    unsigned char priv_ours[32];
    unsigned char ellswift_ours[64];
    unsigned char ellswift_theirs[64];
    int initiating;
    unsigned char shared_secret[32];
};

/* Set of (point, encodings) test vectors, selected to maximize branch coverage, part of the BIP324
 * test vectors. Created using an independent implementation, and tested decoding against paper
 * authors' code. */
static const struct ellswift_xswiftec_inv_test ellswift_xswiftec_inv_tests[] = {
    {0xcc, SECP256K1_FE_CONST(0x05ff6bda, 0xd900fc32, 0x61bc7fe3, 0x4e2fb0f5, 0x69f06e09, 0x1ae437d3, 0xa52e9da0, 0xcbfb9590), SECP256K1_FE_CONST(0x80cdf637, 0x74ec7022, 0xc89a5a85, 0x58e373a2, 0x79170285, 0xe0ab2741, 0x2dbce510, 0xbdfe23fc), {SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0x45654798, 0xece071ba, 0x79286d04, 0xf7f3eb1c, 0x3f1d17dd, 0x883610f2, 0xad2efd82, 0xa287466b), SECP256K1_FE_CONST(0x0aeaa886, 0xf6b76c71, 0x58452418, 0xcbf5033a, 0xdc5747e9, 0xe9b5d3b2, 0x303db969, 0x36528557), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0xba9ab867, 0x131f8e45, 0x86d792fb, 0x080c14e3, 0xc0e2e822, 0x77c9ef0d, 0x52d1027c, 0x5d78b5c4), SECP256K1_FE_CONST(0xf5155779, 0x0948938e, 0xa7badbe7, 0x340afcc5, 0x23a8b816, 0x164a2c4d, 0xcfc24695, 0xc9ad76d8)}},
    {0x33, SECP256K1_FE_CONST(0x1737a85f, 0x4c8d146c, 0xec96e3ff, 0xdca76d99, 0x03dcf3bd, 0x53061868, 0xd478c78c, 0x63c2aa9e), SECP256K1_FE_CONST(0x39e48dd1, 0x50d2f429, 0xbe088dfd, 0x5b61882e, 0x7e840748, 0x3702ae9a, 0x5ab35927, 0xb15f85ea), {SECP256K1_FE_CONST(0x1be8cc0b, 0x04be0c68, 0x1d0c6a68, 0xf733f82c, 0x6c896e0c, 0x8a262fcd, 0x392918e3, 0x03a7abf4), SECP256K1_FE_CONST(0x605b5814, 0xbf9b8cb0, 0x66667c9e, 0x5480d22d, 0xc5b6c92f, 0x14b4af3e, 0xe0a9eb83, 0xb03685e3), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0xe41733f4, 0xfb41f397, 0xe2f39597, 0x08cc07d3, 0x937691f3, 0x75d9d032, 0xc6d6e71b, 0xfc58503b), SECP256K1_FE_CONST(0x9fa4a7eb, 0x4064734f, 0x99998361, 0xab7f2dd2, 0x3a4936d0, 0xeb4b50c1, 0x1f56147b, 0x4fc9764c), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0)}},
    {0x00, SECP256K1_FE_CONST(0x1aaa1cce, 0xbf9c7241, 0x91033df3, 0x66b36f69, 0x1c4d902c, 0x228033ff, 0x4516d122, 0xb2564f68), SECP256K1_FE_CONST(0xc7554125, 0x9d3ba98f, 0x207eaa30, 0xc69634d1, 0x87d0b6da, 0x594e719e, 0x420f4898, 0x638fc5b0), {SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0)}},
    {0x33, SECP256K1_FE_CONST(0x2323a1d0, 0x79b0fd72, 0xfc8bb62e, 0xc34230a8, 0x15cb0596, 0xc2bfac99, 0x8bd6b842, 0x60f5dc26), SECP256K1_FE_CONST(0x239342df, 0xb675500a, 0x34a19631, 0x0b8d87d5, 0x4f49dcac, 0x9da50c17, 0x43ceab41, 0xa7b249ff), {SECP256K1_FE_CONST(0xf63580b8, 0xaa49c484, 0x6de56e39, 0xe1b3e73f, 0x171e881e, 0xba8c66f6, 0x14e67e5c, 0x975dfc07), SECP256K1_FE_CONST(0xb6307b33, 0x2e699f1c, 0xf77841d9, 0x0af25365, 0x404deb7f, 0xed5edb30, 0x90db49e6, 0x42a156b6), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0x09ca7f47, 0x55b63b7b, 0x921a91c6, 0x1e4c18c0, 0xe8e177e1, 0x45739909, 0xeb1981a2, 0x68a20028), SECP256K1_FE_CONST(0x49cf84cc, 0xd19660e3, 0x0887be26, 0xf50dac9a, 0xbfb21480, 0x12a124cf, 0x6f24b618, 0xbd5ea579), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0)}},
    {0x33, SECP256K1_FE_CONST(0x2dc90e64, 0x0cb646ae, 0x9164c0b5, 0xa9ef0169, 0xfebe34dc, 0x4437d6e4, 0x6acb0e27, 0xe219d1e8), SECP256K1_FE_CONST(0xd236f19b, 0xf349b951, 0x6e9b3f4a, 0x5610fe96, 0x0141cb23, 0xbbc8291b, 0x9534f1d7, 0x1de62a47), {SECP256K1_FE_CONST(0xe69df7d9, 0xc026c366, 0x00ebdf58, 0x80726758, 0x47c0c431, 0xc8eb7306, 0x82533e96, 0x4b6252c9), SECP256K1_FE_CONST(0x4f18bbdf, 0x7c2d6c5f, 0x818c1880, 0x2fa35cd0, 0x69eaa79f, 0xff74e4fc, 0x837c80d9, 0x3fece2f8), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0x19620826, 0x3fd93c99, 0xff1420a7, 0x7f8d98a7, 0xb83f3bce, 0x37148cf9, 0x7dacc168, 0xb49da966), SECP256K1_FE_CONST(0xb0e74420, 0x83d293a0, 0x7e73e77f, 0xd05ca32f, 0x96155860, 0x008b1b03, 0x7c837f25, 0xc0131937), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0)}},
    {0xcc, SECP256K1_FE_CONST(0x3edd7b39, 0x80e2f2f3, 0x4d1409a2, 0x07069f88, 0x1fda5f96, 0xf08027ac, 0x4465b63d, 0xc278d672), SECP256K1_FE_CONST(0x053a98de, 0x4a27b196, 0x1155822b, 0x3a3121f0, 0x3b2a1445, 0x8bd80eb4, 0xa560c4c7, 0xa85c149c), {SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0xb3dae4b7, 0xdcf858e4, 0xc6968057, 0xcef2b156, 0x46543152, 0x6538199c, 0xf52dc1b2, 0xd62fda30), SECP256K1_FE_CONST(0x4aa77dd5, 0x5d6b6d3c, 0xfa10cc9d, 0x0fe42f79, 0x232e4575, 0x661049ae, 0x36779c1d, 0x0c666d88), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0x4c251b48, 0x2307a71b, 0x39697fa8, 0x310d4ea9, 0xb9abcead, 0x9ac7e663, 0x0ad23e4c, 0x29d021ff), SECP256K1_FE_CONST(0xb558822a, 0xa29492c3, 0x05ef3362, 0xf01bd086, 0xdcd1ba8a, 0x99efb651, 0xc98863e1, 0xf3998ea7)}},
    {0x00, SECP256K1_FE_CONST(0x4295737e, 0xfcb1da6f, 0xb1d96b9c, 0xa7dcd1e3, 0x20024b37, 0xa736c494, 0x8b625981, 0x73069f70), SECP256K1_FE_CONST(0xfa7ffe4f, 0x25f88362, 0x831c087a, 0xfe2e8a9b, 0x0713e2ca, 0xc1ddca6a, 0x383205a2, 0x66f14307), {SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0)}},
    {0xff, SECP256K1_FE_CONST(0x587c1a0c, 0xee91939e, 0x7f784d23, 0xb963004a, 0x3bf44f5d, 0x4e32a008, 0x1995ba20, 0xb0fca59e), SECP256K1_FE_CONST(0x2ea98853, 0x0715e8d1, 0x0363907f, 0xf2512452, 0x4d471ba2, 0x454d5ce3, 0xbe3f0419, 0x4dfd3a3c), {SECP256K1_FE_CONST(0xcfd5a094, 0xaa0b9b88, 0x91b76c6a, 0xb9438f66, 0xaa1c095a, 0x65f9f701, 0x35e81712, 0x92245e74), SECP256K1_FE_CONST(0xa89057d7, 0xc6563f0d, 0x6efa19ae, 0x84412b8a, 0x7b47e791, 0xa191ecdf, 0xdf2af84f, 0xd97bc339), SECP256K1_FE_CONST(0x475d0ae9, 0xef46920d, 0xf07b3411, 0x7be5a081, 0x7de1023e, 0x3cc32689, 0xe9be145b, 0x406b0aef), SECP256K1_FE_CONST(0xa0759178, 0xad802324, 0x54f827ef, 0x05ea3e72, 0xad8d7541, 0x8e6d4cc1, 0xcd4f5306, 0xc5e7c453), SECP256K1_FE_CONST(0x302a5f6b, 0x55f46477, 0x6e489395, 0x46bc7099, 0x55e3f6a5, 0x9a0608fe, 0xca17e8ec, 0x6ddb9dbb), SECP256K1_FE_CONST(0x576fa828, 0x39a9c0f2, 0x9105e651, 0x7bbed475, 0x84b8186e, 0x5e6e1320, 0x20d507af, 0x268438f6), SECP256K1_FE_CONST(0xb8a2f516, 0x10b96df2, 0x0f84cbee, 0x841a5f7e, 0x821efdc1, 0xc33cd976, 0x1641eba3, 0xbf94f140), SECP256K1_FE_CONST(0x5f8a6e87, 0x527fdcdb, 0xab07d810, 0xfa15c18d, 0x52728abe, 0x7192b33e, 0x32b0acf8, 0x3a1837dc)}},
