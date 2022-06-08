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

#include "bit_math.h"

static int utf_count_leading_zeros(unsigned int x)
{
    static unsigned int const clz_lookup[] = {
        32U, 31U, 30U, 30U, 29U, 29U, 29U, 29U,
        28U, 28U, 28U, 28U, 28U, 28U, 28U, 28U
    };
    unsigned int n;

    if (x >= (1U << 16)) {
        if (x >= (1U << 24)) {
            if (x >= (1 << 28)) {
                n = 28U;
            } else {
                n = 24U;
            }
        } else {
            if (x >= (1U << 20)) {
                n = 20U;
            } else {
                n = 16U;
            }
        }
    } else {
        if (x >= (1U << 8)) {
            if (x >= (1U << 12)) {
                n = 12U;
            } else {
                n = 8U;
            }
        } else {
            if (x >= (1U << 4)) {
                n = 4U;
            } else {
                n = 0U;
            }
        }
    }

    return (int)(clz_lookup[x >> n] - n);
}

