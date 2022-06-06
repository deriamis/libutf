/*
MIT License

Copyright (c) 2022 Ryan Egesdahl

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

SPDX-FileCopyrightText: 2020 Ryan Egesdahl
SPDX-License-Identifier: MIT
*/

#include <utf/utf.h>
#include <stdio.h>

#pragma clang diagnostic ignored "-Wc99-extensions"

bool utf_is_pair_start(uint_least32_t w) {
    if (w >= 0xD800 && w <= 0xDBFF) {
        return true;
    }

    return false;
}

bool utf_is_pair_end(uint_least32_t w)
{
    if (w >= 0xDC00 && w <= 0xDFFF) {
        return true;
    }

    return false;
}

bool utf_is_pair(uint_least32_t w)
{
    if (w >= 0xD800 && w <= 0xDFFF) {
        return true;
    }

    return false;
}

bool utf_is_valid_rune(uint_least32_t w)
{
    /* Surrogate Pair range */
    if (utf_is_pair(w)) {
        return false;
    }

    /* Non-character range */
    if (w >= 0xFDD0 && w <= 0xFDEF) {
        return false;
    }

    /* Problematic non-character */
    if ((w >= 0xFFFE && w <= 0x10FFFF) &&
            ((w & 0xFFFF) == 0xFFFE || (w & 0xFFFF) == 0xFFFF)) {
        return false;
    }

    /* Value is too large for UTF-8 */
	if (w > 0x10FFFF) {
		return false;
    }

    return true;
}

int utf_runelen(uint_least32_t w)
{
    if (w <= UTF_ASCII_MAX) {
        return 1;
    }

    if (w >= UTF_UTF8_MIN && w <= 0x000007FF) {
        return 2;
    }

    if (w >= 0x00000800 && w <= 0x0000FFFF) {
        return 3;
    }

    if (w >= 0x00010000 && w <= 0x0010FFFF) {
        return 4;
    }

    if (w >= 0x00200000 && w <= 0x03FFFFFF) {
        return 5;
    }

    if (w >= 0x04000000 && w <= 0x7FFFFFFF) {
        return 6;
    }

    return -1;
}

