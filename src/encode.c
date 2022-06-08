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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <utf/utf.h>
#include <utf/platform.h>

#include "state.h"
#include "error.h"

int utf_encoderune_r(char* dest restrict, utf_rune* src restrict,
                     utf_state_p state)
{
    utf_rune temp = 0;
    int n = 0;

    assert(state);
    assert(state->count <= 0);

    if (!src) {
        return 0;
    }

    if (!dest) {
        dest = (char *)calloc(UTF_BYTES_MAX, sizeof(char));
    }

    if (sizeof(utf_rune) == 2 && state->count == -4 && utf_is_pair_end(*src)) {
        /* We're starting with a leftover lone high surrogate in a pair
         * sequence. First, compute the CESU-8 value of the surrogate and then
         * proceed with converting the character.
         */
        temp =  (unsigned int)(
                (state->value.bytes[0] << 16 | state->value.bytes[1] << 8)
                - (0x10000 >> 19 | 0xD80D));
        *dest++ = (char)(0xE0 | ((temp & 0xF000) >> 12));
        *dest++ = (char)(0x80 | ((temp & 0x0FC0) >> 6));
        *dest++ = (char)(0x80 |  (temp & 0x003F));
        state->count = 0;
        n = 3;
    }

    /* BMP rune */
    if (utf_runelen(*src) == 1) {
        *dest = (char)*src;
        return n + 1;
    }

    /* 2-byte rune */
    if (utf_runelen(*src) == 2) {
        *dest++ = (char)(0xC0 | ((*src & 0x7C0) >> 6));
        *dest   = (char)(0x80 |  (*src & 0x03F));
        return n + 2;
    }

    /* 3-byte rune */
    if (utf_runelen(*src) == 3) {
        /* Member of a surrogate pair */
        if (sizeof(utf_rune) == 2 && utf_is_pair(*src)) {

            /* First half of a surrogate pair. We store the state here and
             * return n + 0. */
            if (utf_is_pair_start(*src)) {
                temp = (unsigned int)(((*src & 0x3FF) << 10) + 0x10000);
                state->value.bytes[0] = (temp >> 16) & 0xFF;
                state->value.bytes[1] = (temp >> 8) & 0xFF;
                state->count = -4;
                *dest = (char)(0xF0 | ((temp & 0x1C0000) >> 18));
                return n;
            }

            /* Second half of a surrogate pair. We finish reconstructing the
             * full value and write the remaining three bytes. */
            if (state->count == -4) {
                temp = ((unsigned int)(state->value.bytes[0] << 16)
                        | (unsigned int)(state->value.bytes[1] << 8)
                        | (unsigned int)(*src & 0x3FF));
                state->count = 0;
                *dest++ = (char)(0xF0 | ((temp & 0x1C0000) >> 18));
                *dest++ = (char)(0x80 | ((temp & 0x03F000) >> 12));
                *dest++ = (char)(0x80 | ((temp & 0x000FC0) >> 6));
                *dest   = (char)(0x80 |  (temp & 0x00003F));
                return 4;
            }
        }

        *dest++ = (char)(0xE0 | ((*src & 0x03F000) >> 12));
        *dest++ = (char)(0x80 | ((*src & 0x000FC0) >> 6));
        *dest++ = (char)(0x80 |  (*src & 0x00003F));
        return n + 3;
    }

    if (utf_runelen(*src) == 4) {
        *dest++ = (char)(0xF0 | ((*src & 0x1C0000) >> 18));
        *dest++ = (char)(0x80 | ((*src & 0x03F000) >> 12));
        *dest++ = (char)(0x80 | ((*src & 0x000FC0) >> 6));
        *dest++ = (char)(0x80 |  (*src & 0x00003F));
        return 4;
    }

    state->errno = UTF_ESEQ;
    return 0;
}

int utf_encoderune(char* dest restrict, utf_rune* src restrict)
{
    utf_state_p state = utf_state_new();
    int bytes_processed = 0;

    bytes_processed = utf_encoderune_r(dest, src, state);
    utf_set_error(state->errno);
    utf_state_destroy(state);

    return bytes_processed;
}
