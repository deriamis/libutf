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

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <utf/utf.h>
#include <utf/platform.h>

#include "error.h"

int utf_errno = 0;

const char* utf_error_messages[] = {
    "Success",
    "Invalid start byte",
    "Truncated sequence",
    "Invalid encoded UTF value",
    "Overlong byte sequence",
    "Invalid byte sequence",
    "Decoded rune exceeds UTF value limit",
};

void utf_set_error(int code)
{
    utf_errno = code;
}

int utf_strerror_r(int errnum, char* buf, size_t buflen)
{
    size_t error_message_len = 0;

    if (buflen <= 1) {
        if (buflen) {
            *buf = '\0';
        }
        return ERANGE;
    }

    memset(buf, '\0', buflen);

    if (errnum < 0 || errnum > UTF_MAX_ERRNO) {
        return EINVAL;
    }

    error_message_len = strlen(utf_error_messages[errnum - 1]);
    if (error_message_len + 1 > buflen) {
        return ERANGE;
    }

    if (!strncpy(buf, utf_error_messages[errnum - 1], error_message_len + 1)){
        return ENOMEM;
    }

    return 0;
}

char* utf_strerror(int errnum)
{
    static char buf[UTF_STRERROR_BUFSIZE] = { 0 };
    static char const unknown_error_fmt[] = "Unknown error %d";
    static char const internal_error[] = "Internal error";

    utf_strerror_r(errnum, buf, UTF_STRERROR_BUFSIZE);

    if (!*buf) {
        assert(sizeof(buf) >= sizeof(unknown_error_fmt) + INT_STRLEN_BOUND(errnum));
        if (!snprintf(buf, UTF_STRERROR_BUFSIZE, unknown_error_fmt, errnum)) {
            return memcpy(buf, internal_error, strlen(internal_error));
        }
    }

    return buf;
}

