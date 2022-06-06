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

#include <utf/utf.h>

#include "util.h"

TestParams* TestParams_alloc(int num_tests)
{
    TestParams* params;

    params =
        (TestParams*)malloc(sizeof(TestParams));
    if (!params) {
        return 0;
    }

    params->references =
        (utf_rune*)calloc((size_t)num_tests, sizeof(utf_rune));
    if (!params->references) {
        free(params);
        return 0;
    }

    params->unknowns_sz =
        (int*)calloc((size_t)num_tests, sizeof(int));
    if (!params->unknowns_sz) {
        free(params->references);
        free(params);
        return 0;
    }

    params->unknowns =
        (char*)calloc((size_t)(num_tests * num_tests), sizeof(char));
    if (!params->unknowns) {
        free(params->unknowns_sz);
        free(params->references);
        free(params);
        return 0;
    }

    params->error_counts =
        (int*)calloc((size_t)num_tests, sizeof(int));
    if (!params->error_counts) {
        free(params->unknowns);
        free(params->unknowns_sz);
        free(params->references);
        free(params);
        return 0;
    }

    params->expected_success =
        (bool*)calloc((size_t)num_tests, sizeof(bool));
    if (!params->expected_success) {
        free(params->error_counts);
        free(params->unknowns);
        free(params->unknowns_sz);
        free(params->references);
        free(params);
        return 0;
    }

    return params;
}

void TestParams_init(TestParams* params,
        int num_tests,
        utf_rune* references,
        int* unknowns_sz,
        char* unknowns,
        int* error_counts,
        bool* expected_success)
{
    params->num_tests = num_tests;
    memcpy(params->references, references,
            sizeof(utf_rune) * (size_t)num_tests);
    memcpy(params->unknowns_sz, unknowns_sz,
            sizeof(int) * (size_t)num_tests);
    memcpy(params->unknowns, unknowns,
            sizeof(char) * (size_t)(num_tests * num_tests));
    memcpy(params->error_counts, error_counts,
            sizeof(int) * (size_t)num_tests);
    memcpy(params->expected_success, expected_success,
            sizeof(bool) * (size_t)num_tests);
}


TestParams* TestParams_new(
        int num_tests,
        utf_rune* references,
        int* unknowns_sz,
        char* unknowns,
        int* error_counts,
        bool* expected_success)
{
    TestParams* params = TestParams_alloc(num_tests);
    TestParams_init(params,
            num_tests,
            references,
            unknowns_sz,
            unknowns,
            error_counts,
            expected_success);
    return params;
}

void TestParams_destroy(TestParams* params)
{
    free(params->expected_success);
    params->expected_success = 0;

    free(params->error_counts);
    params->error_counts = 0;

    free(params->unknowns_sz);
    params->unknowns_sz = 0;

    free(params->unknowns);
    params->unknowns = 0;

    free(params->references);
    params->references = 0;

    free(params);
    params = 0;
}

