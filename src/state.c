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

#include <string.h>
#include <stdlib.h>

#include <utf/utf.h>
#include <utf/platform.h>

#include "state.h"

utf_state_p utf_state_alloc(void)
{
    utf_state_p state = 0;

    state = (utf_state_p)calloc(1, sizeof(utf_state_s));
    if (!state) {
        abort();
    }

    return state;
}

utf_state_p utf_state_new(void)
{
    utf_state_p state = utf_state_alloc();
    return state;
}

void utf_state_destroy(utf_state_p state)
{
    free(state);
}

void utf_state_clear(utf_state_p state)
{
    memset(state, 0, sizeof(utf_state_s));
}

int utf_state_get_error(utf_state_p state)
{
    return state->errno;
}

bool utf_state_has_error(utf_state_p state)
{
    return (utf_state_get_error(state) > 0) ? true : false;
}

int utf_state_bytes_processed(utf_state_p state)
{
    return state->count;
}
