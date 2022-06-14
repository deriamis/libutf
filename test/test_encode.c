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

#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

/* TODO: Parametrized encoder tests that mirror the decoder tests. Encoding and
 * decoding should be idempotent given valid inputs. */

void test_encode_kosme(void)
{
    char dest[6] = {0};
    utf_rune src[6] = {0x03BA, 0x1F79, 0x03C3, 0x03BC, 0x03B5, 0x0000};
    char *kosme = "\xCE\xBA\xE1\xBD\xB9\xCF\x83\xCE\xBC\xCE\xB5";
    int n = 0;
    int i, j;

    for (i = 0, j = 0; j < 6; i+=n, j++) {
        n = utf_encoderune(&dest[i], &src[j]);
        if (n == 0) {
            break;
        }
    }

    TEST_ASSERT_EQUAL_MESSAGE(12, i, "Incorrect number of bytes encoded");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(kosme, dest, "String incorrectly encoded");
}

int main(void)
{
    UNITY_BEGIN();
    if (TEST_PROTECT()) {
        RUN_TEST(test_encode_kosme);
    }
    return UNITY_END();
}
