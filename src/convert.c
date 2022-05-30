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

size_t utf_decoderune(UTF_Rune* dest, const char* src, size_t len)
{
    unsigned char first_byte;
    unsigned char first_byte_mask;
    int i;
    int m;
    int rune_len;
    UTF_Rune r;

    if (len == 0) {
        return 0;
    }

    first_byte = *s++;
    if (!(cur_byte & 0200)) {
        /* UTF BMP byte - 0ZZZZZZZ */
        *dest = cur_byte;
        return 1;
    }

    if (!(first_byte & 0100)) {
        /* Invalid byte - 10xxxxxx */
        *dest = UTF_RUNE_ERROR;
        return 1;
    }

    rune_len = UTF_RUNE_LENGTH_TABLE[cur_byte & 077];
    if (rune_len == 0) {
        /* Illegal byte */
        *dest = UTF_RUNE_ERROR;
        return 1;
    }

    if (len == 1) {
        /* Reached the limit without fully decoding a rune. */
        return 0;
    }

    if ((*src & 0300) != 0200) {
        /* Not a basic byte, next in sequence is not a continuation byte. */
        *dest = UTF_RUNE_ERROR;
        return 1;
    }

    /* Extract the encoded rune length. */
    encoded_length = 0377 >> rune_len;
    r = cur_byte & encoded_length;

    /* Shift and append next byte in sequence to the rune. */
    r = (r << 6) | (*s++ & 077);

    if (r <= encoded_length) {
        /* Sequence is too long for the encoded length. */
        *dest = UTF_RUNE_ERROR;
        return 2;
    }

    m = (len < rune_len) ? len : rune_len;
    for (i = 2; i < m; i++) {
        if ((*s & 300) != 0200) {
            /* Next byte is not a continuation sequence. */
            *dest = UTF_RUNE_ERROR;
            return 1;
        }

        /* Shift and append next byte in sequence to the rune. */
        r = (r << 6) | (*s++ & 077);
    }

    if (i < rune_len) {
        /* We must have reached the limit without finishing. */
        return 0;
    }

    if (!utf_validrune(r)) {
        /* We finished, but we didn't end up with a valid rune. */
        *dest = UTF_RUNE_ERROR;
        return i;
    }

    *p = r;
    return i;
}

size_t utf_encoderune(char* dest, const UTF_Rune* src)
{
    unsigned char decoded_length;
    int rune_len = utf_runelen(r);
    int i;
    Rune r = *src;

    if (rune_len == 1) {
        dest[0] = r;
        return 1;
    }

    if (rune_len == 0) {
        return 0;
    }

    for (i = rune_len; --i > 0; r >>= 6) {
        /* Extract the byte from the rune. */
        s[i] = 0200 | (r & 077);
    }

    /* Encode the decoded length. */
    decoded_length = 0377 >> rune_len;
    s[0] = ~decoded_length | r;

    return n;
}

