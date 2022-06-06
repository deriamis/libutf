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
#include <utf/platform.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"

typedef struct {
    int num_tests;
    utf_rune* references;
    int* unknowns_sz;
    char* unknowns;
    int* error_counts;
    bool* expected_success;
} TestParams;

TestParams* TestParams_alloc(int num_tests);

void TestParams_init(TestParams* params,
        int num_tests,
        utf_rune* references,
        int* unknowns_sz,
        char* unknowns,
        int* error_counts,
        bool* expected_success);

TestParams* TestParams_new(
        int num_tests,
        utf_rune* references,
        int* unknowns_sz,
        char* unknowns,
        int* error_counts,
        bool* expected_success);

void TestParams_destroy(TestParams* params);

