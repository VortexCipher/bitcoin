/**********************************************************************
 * Copyright (c) 2021 Cory Fields                                     *
 * Distributed under the MIT software license, see the accompanying   *
 * file LICENSE or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#ifndef _MINISKETCH_FIELDDEFINES_H_
#define _MINISKETCH_FIELDDEFINES_H_

/*

This file translates external defines ENABLE_FIELD_FOO, DISABLE_FIELD_FOO, and
DISABLE_DEFAULT_FIELDS to internal ones: ENABLE_FIELD_INT_FOO. Only the
resulting internal includes should be used.

Default: All fields enabled
-DDISABLE_FIELD_3: All fields except 3 are enabled
-DENABLE_FIELD_3: All fields enabled
-DDISABLE_DEFAULT_FIELDS: Error, no fields enabled
-DDISABLE_DEFAULT_FIELDS -DENABLE_FIELD_3: Only field 3 enabled
-DDISABLE_DEFAULT_FIELDS -DENABLE_FIELD_2 -DENABLE_FIELD_3: Only fields 2 and 3 are enabled
-DDISABLE_DEFAULT_FIELDS -DENABLE_FIELD_2 -DENABLE_FIELD_3 -DDISABLE_FIELD_3: Only field 2 enabled

*/

#ifdef DISABLE_DEFAULT_FIELDS
#if defined(ENABLE_FIELD_2) && !defined(DISABLE_FIELD_2)
#define ENABLE_FIELD_INT_2
#endif
#if defined(ENABLE_FIELD_3) && !defined(DISABLE_FIELD_3)
#define ENABLE_FIELD_INT_3
#endif
#if defined(ENABLE_FIELD_4) && !defined(DISABLE_FIELD_4)
#define ENABLE_FIELD_INT_4
#endif
#if defined(ENABLE_FIELD_5) && !defined(DISABLE_FIELD_5)
#define ENABLE_FIELD_INT_5
#endif
#if defined(ENABLE_FIELD_6) && !defined(DISABLE_FIELD_6)
#define ENABLE_FIELD_INT_6
#endif
#if defined(ENABLE_FIELD_7) && !defined(DISABLE_FIELD_7)
#define ENABLE_FIELD_INT_7
#endif
#if defined(ENABLE_FIELD_8) && !defined(DISABLE_FIELD_8)
#define ENABLE_FIELD_INT_8
#endif
#if defined(ENABLE_FIELD_9) && !defined(DISABLE_FIELD_9)
#define ENABLE_FIELD_INT_9
#endif
#if defined(ENABLE_FIELD_10) && !defined(DISABLE_FIELD_10)
#define ENABLE_FIELD_INT_10
#endif
#if defined(ENABLE_FIELD_11) && !defined(DISABLE_FIELD_11)
#define ENABLE_FIELD_INT_11
#endif
