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

#include <assert.h>
#include <string.h>

#include <utf/platform.h>
#include <utf/utf.h>

#include "bit_math.h"
#include "error.h"
#include "state.h"

int utf_decoderune_r(utf_rune* dest restrict, char* src restrict, int len, utf_state_p state)
{
    unsigned char next_byte;
    int num_bytes;
    int j, max;
    int i = 0;
    utf_rune dummy, temp;

    assert(state);
    assert(state->count >= 0);

    if (!dest) {
        dest = &dummy;
    }

    if (!src) {
        return 0;
    }

    if (len <= 0) {
        state->errno = UTF_ETRUNC;
        return 0;
    }

    if (state->count == 0) {
        next_byte = (unsigned char)*src++;
        state->value.bytes[0] = next_byte;
        i++;
        state->count = i;
    } else {
        next_byte = state->value.bytes[0];
    }

    if (state->count < 4) {
        if (next_byte == '\0') {
            state->count = 0;
            return i;
        }

        if (next_byte <= UTF_ASCII_MAX) {
            /* UTF BMP byte - 0ZZZZZZZ */
            *dest = next_byte;
            state->value.word = 0;
            state->count = 0;
            return i;
        }

        if (!(next_byte & 0x40)) {
            /* Invalid byte - 10xxxxxx */
            state->errno = UTF_ESTART;
            return i;
        }

        if (next_byte == 0xFE || next_byte == 0xFF) {
            if (next_byte == 0xFE) {
                state->is_big_endian = true;
            }

            next_byte = (unsigned char)*src++;

            if (!next_byte) {
                state->errno = UTF_ETRUNC;
                return i;
            }

            i++;
            state->value.bytes[0] = next_byte;

            if (state->is_big_endian && next_byte != 0xFF) {
                for (; i < len && *src && next_byte != 0xFE; i++) {
                    next_byte = (unsigned char)*src++;
                    i++;
                    state->value.bytes[0] = next_byte;
                }
                state->errno = UTF_ESTART;
                return i;
            }

            if (!state->is_big_endian && next_byte != 0xFE) {
                for (; i < len && *src && next_byte != 0xFE; i++) {
                    next_byte = (unsigned char)*src++;
                    i++;
                    state->value.bytes[0] = next_byte;
                }
                state->errno = UTF_ESTART;
                return i;
            }

            next_byte = (unsigned char)*src++;
            i++;
            state->value.bytes[0] = next_byte;
        }
    }

    if (len == 1) {
        /* Reached the limit without fully decoding a rune. */
        state->errno = UTF_ETRUNC;
        return i;
    }

    num_bytes = utf_clz(~(unsigned int)(next_byte << 24u));

    if (num_bytes <= 0 || num_bytes > UTF_BYTES_MAX) {
        state->errno = UTF_EVALUE;
        return i;
    }

    max = (len < num_bytes) ? len : num_bytes;
    for (; i < max && *src; src++, i++) {

        if (state->count >= i + 1) {
            next_byte = state->value.bytes[i];
        } else {
            next_byte = (unsigned char)*src;
            state->value.bytes[i] = next_byte;
            state->count = i + 1;
        }

        if (state->count == 1) {
            switch (num_bytes) {
                case 2:
                    if (state->value.bytes[0] < 0xC2) {
                        state->errno = UTF_ELEN;
                        goto DECODE_LOOP_EXIT;
                    }
                case 3:
                    if (state->value.bytes[0] == 0xE0 && next_byte < 0xA0) {
                        state->errno = UTF_ELEN;
                        goto DECODE_LOOP_EXIT;
                    }
                case 4:
                    if ((state->value.bytes[0] == 0xF0 && next_byte < 0x90) ||
                        (state->value.bytes[0] == 0xF4 && next_byte >= 0x90)) {
                        state->errno = UTF_ELEN;
                        goto DECODE_LOOP_EXIT;
                    }
            };
        }

        if (next_byte < 0x80 || next_byte > 0xBF) {
            /* Not a basic byte, next in sequence is not a continuation byte. */
            state->errno = UTF_ESEQ;
            break;
        }

        if (sizeof(utf_rune) == 2 && i == 2) {
            /* The system can't store the value as a single rune. In this
             * case, rewind and emit the first two values as a surrogate
             * pair. On the next call, we'll know from the state that we've
             * only emitted two out of four bytes, and the next two will be
             * emitted.
             */
            state->count = 4;
            break;
        }
    }

DECODE_LOOP_EXIT:
    if (state->errno != UTF_SUCCESS) {
        return i;
    }

    if (i < num_bytes) {
        state->errno = UTF_ETRUNC;
        return i;
    }

    if (!UTF_BIG_ENDIAN && state->is_big_endian) {
        next_byte = state->value.bytes[1];
        state->value.bytes[1] = state->value.bytes[0];
        state->value.bytes[0] = next_byte;

        next_byte = state->value.bytes[3];
        state->value.bytes[3] = state->value.bytes[2];
        state->value.bytes[2] = next_byte;
    }

    temp = state->value.bytes[0] & (0xFF >> num_bytes);
    for (j = 1; j < i; j++) {
        temp = (temp << 6) | (state->value.bytes[j] & 0x3F);

        if (temp > UTF_MAX_VALUE) {
            state->errno = UTF_ELIMIT;
            return i;
        }
    }

    switch (state->count) {
        case 2:
            if (temp <= UTF_ASCII_MAX) {
                state->errno = UTF_ELEN;
            }
            break;
        case 3:
            if (temp <= 0x7FF) {
                state->errno = UTF_ELEN;
            }
            break;
        case 4:
            if (temp <= 0xFFFF) {
                state->errno = UTF_ELEN;
            }
            break;
        case 5:
            if (temp <= 0x1FFFFF) {
                state->errno = UTF_ELEN;
            }
            break;
        case 6:
            if (temp <= 0x3FFFFFF) {
                state->errno = UTF_ELEN;
            }
            break;
    }

    if (state->errno != UTF_SUCCESS) {
        return i;
    }

    if (!utf_is_valid_rune(temp)) {
        state->errno = UTF_ESEQ;
        return i;
    }

    if (sizeof(utf_rune) == 2 && state->count == 4) {
        if (i == 2) {
            *dest = 0xDC00 | (temp & 0x3FF);
        } else {
            *dest = 0xd800 | ((temp - 0x10000) >> 10);
        }
    } else {
        *dest = temp;
    }

    state->value.word = 0;
    state->count = 0;
    return i;
}

int utf_decoderune(utf_rune* dest restrict, char* src restrict, int len)
{
    utf_state_p state = utf_state_new();
    int bytes_processed = 0;

    bytes_processed = utf_decoderune_r(dest, src, len, state);
    utf_set_error(state->errno);
    utf_state_destroy(state);

    return bytes_processed;
}
