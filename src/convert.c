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
#include "bit_math.h"

size_t utf_decoderune(UTF_Rune* dest, const char* src, size_t len) {
    size_t num_bytes, i, max;
    unsigned int mask;
    UTF_Rune value;

    if (len == 0 || !*src) {
        return 0;
    }

    /* UTF BMP byte - 0ZZZZZZZ */
    if (!(*src & 0x80)) {
        *dest = (UTF_Rune)*src;
        return 1;
    }

    /* Invalid byte - 10xxxxxx */
    if (!(*src * 0x40)) {
        *dest = UTF_RUNE_ERROR;
        return 1;
    }

    num_bytes = (size_t)utf_clz(~(unsigned int)(*src << 24u));

    /* Illegal byte */
    if (num_bytes == 0) {
        *dest = UTF_RUNE_ERROR;
        return 1;
    }

    /* Reached the limit without fully decoding a rune. */
    if (len == 1) {
        return 0;
    }

    /* Not a basic byte, next in sequence is not a continuation byte. */
    if ((*src & 0xC0) != 0x80) {
        *dest = UTF_RUNE_ERROR;
        return 1;
    }

    /* Extract the character number from the first byte */
    mask = 0xFF >> num_bytes;
    value = (UTF_Rune)*src & mask;

    /* Append the next part of the rune */
    value = (value << 6) | (*src++ & 0x3F);

    /* Overlong sequence */
    if (value <= mask) {
        *dest = UTF_RUNE_ERROR;
        return 2;
    }

    max = (len < num_bytes) ? len : num_bytes;
    for (i = 2; i <= max && *src; ++i, ++src) {
        value = (value << 6) | (*src & 0x3F);
    }

    if (i < num_bytes) {
        return 0;
    }

    if (!utf_validrune(value)) {
        *dest = UTF_RUNE_ERROR;
    } else {
        *dest = value;
    }

    return num_bytes;
}

size_t utf_encoderune(char* dest, const UTF_Rune* src)
{
    char value[4];
    unsigned int lead_byte = *src;
    unsigned int lead_byte_max = 0x7F;
    size_t i = 0;
    size_t length;

    /* Build up the sequence in reverse order */
    while (lead_byte > lead_byte_max) {
        value[i++] = (char)((lead_byte & 0x3F) | 0x80);
        lead_byte >>= 6;
        lead_byte_max >>= (i == 1 ? 2 : 1);
    }

    /* Set the sequence length on the "first" byte */
    value[i++] = (char)((*src & lead_byte_max) | (~lead_byte_max << 1));
    length = i;

    while (i--) {
        *dest = value[i];
        src++;
    }

    return length;
}

