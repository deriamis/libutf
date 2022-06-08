/*
The MIT License (MIT)

Copyright © 2022 Ryan Egesdahl

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

SPDX-FileCopyrightText: 2020 Ryan Egesdahl
SPDX-License-Identifier: MIT
*/

#ifndef PLATFORM_H
#define PLATFORM_H

#include <limits.h>

#if defined(__STDC__)
#  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#    include <stdint.h>
#    include <stdbool.h>
#  else
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wmissing-declarations"
#    include "stdbool.h"
#    pragma clang diagnostic pop
#    define restrict
#    define inline
#    define UINT32_C(c) c ## U

typedef unsigned long size_t;
#    if defined(_MSC_VER) || defined(__x86_64__) || defined(__aarch64__) || defined(__linux__)
/* On these architectures, an int is guaranteed to be 32 bits. */
typedef unsigned int uint_least32_t;
#    else
/* Everywhere else, we fall back to the standard. */
typedef unsigned long int uint_least32_t;
#    endif
# endif /* defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L */
#endif /* defined(__STDC__) */

#define TYPE_WIDTH(t) (sizeof (t) * CHAR_BIT)
#define INT_BITS_STRLEN_BOUND(b) (((b) * 146 + 484) / 485)
#define INT_STRLEN_BOUND(t) (INT_BITS_STRLEN_BOUND (TYPE_WIDTH (t)))

/* Determine the byte order of the system. */
#ifdef UTF_BIG_ENDIAN
#elif defined(BYTE_ORDER) && defined(BIG_ENDIAN)
#   define UTF_BIG_ENDIAN (BYTE_ORDER == BIG_ENDIAN)
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__)
#   define UTF_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
#   define UTF_BIG_ENDIAN 1
#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
#   define UTF_BIG_ENDIAN 0
#elif defined(__s390__) || defined(__s390x__)
#   define UTF_BIG_ENDIAN 1
#elif defined(_PA_RISC1_0) || defined(_PA_RISC1_1) || defined(_PA_RISC2_0)
#   define UTF_BIG_ENDIAN 1
#elif defined(sparc) || defined(__sparc) || defined(__sparc__)
#   define UTF_BIG_ENDIAN 1
#else
#   define UTF_BIG_ENDIAN 0
#endif

#endif /* PLATFORM_H */

