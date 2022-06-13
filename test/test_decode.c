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
#include "util.h"

#include <string.h>
#include <stdlib.h>

void parametrized_decode(const TestParams* params)
{
    int i, j, n, max;
    int error_count;
    char *p = 0;
    utf_rune unknown;
    utf_rune reference;
    utf_state_p state = utf_state_new();

    for (i = 0; i < params->num_tests; i++) {
        error_count = 0;
        unknown = 0UL;
        reference = params->references[i];

        for (j = 0; j < params->unknowns_sz[i]; j += n) {
            n = 0;

            max = (params->unknowns_sz[i] < UTF_BYTES_MAX) ?
                params->unknowns_sz[i] : UTF_BYTES_MAX;

            p = &params->unknowns[i * params->num_tests + j];
            n = utf_decoderune_r(&unknown, p, max, state);

        if (utf_state_has_error(state)) {
                error_count++;
                utf_state_clear(state);
            }

            if (n == 0) {
                break;
            }
        }

        TEST_ASSERT_EQUAL_HEX32(reference, unknown);

        if (!params->expected_success[i]) {
            TEST_ASSERT_EQUAL_HEX32(0, unknown);
            TEST_ASSERT_EQUAL_MESSAGE(params->error_counts[i], error_count, "Has a correct error count");
        }
    }

    utf_state_destroy(state);
}

void setUp(void) {}

void tearDown(void) {}

void test_decode_kosme(void)
{
    utf_rune rbuf[6] = {0};
    char *kosme = "\xCE\xBA\xE1\xBD\xB9\xCF\x83\xCE\xBC\xCE\xB5";
    int n = 0;
    int i;

    for (i = 0; *kosme != 0; i++, kosme += n) {
        n = utf_decoderune(&rbuf[i], kosme, UTF_BYTES_MAX);
        if (n == 0) {
            break;
        }
    }

    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x03BA, rbuf[0], "U+03BA GREEK SMALL LETTER KAPPA");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x1F79, rbuf[1], "U+1F79 GREEK SMALL LETTER OMICRON WITH OXIA");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x03C3, rbuf[2], "U+03C3 GREEK SMALL LETTER SIGMA");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x03BC, rbuf[3], "U+03BC GREEK SMALL LETTER MU");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x03B5, rbuf[4], "U+03B5 GREEK SMALL LETTER EPSILON");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x0000, rbuf[5], "U+0000 NULL");
}
#undef NUM_TESTS

#define NUM_TESTS 6
void test_decode_boundary_start(void)
{
    TestParams* params = 0;

    utf_rune references[NUM_TESTS] = {
        0x00000000,
        0x00000080,
        0x00000800,
        0x00010000,
        0,
        0,
    };

    int unknowns_sz[NUM_TESTS] = { 1, 2, 3, 4, 5, 6 };

    char unknowns[NUM_TESTS * NUM_TESTS] = {
        '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', /* U-00000000 */
        '\xc2', '\x80', '\x00', '\x00', '\x00', '\x00', /* U-00000080 */
        '\xe0', '\xa0', '\x80', '\x00', '\x00', '\x00', /* U-00000800 */
        '\xf0', '\x90', '\x80', '\x80', '\x00', '\x00', /* U-00010000 */
        '\xf8', '\x88', '\x80', '\x80', '\x80', '\x00', /* U-00200000 */
        '\xfc', '\x84', '\x80', '\x80', '\x80', '\x80'  /* U-04000000 */
    };

    int error_counts[NUM_TESTS] = { 0, 0, 0, 0, 1, 1 };

    bool expected_success[NUM_TESTS] = { true, true, true, true, false, false };

    params = TestParams_new(
            NUM_TESTS,
            references,
            unknowns_sz,
            unknowns,
            error_counts,
            expected_success);

    parametrized_decode(params);

    TestParams_destroy(params);
}
#undef NUM_TESTS

#define NUM_TESTS 6
void test_decode_boundary_end(void)
{
    TestParams* params = 0;

    utf_rune references[NUM_TESTS] = {
        0x0000007F,
        0x000007FF,
        0,
        0,
        0,
        0,
    };

    int unknowns_sz[NUM_TESTS] = { 1, 2, 3, 4, 5, 6 };

    char unknowns[NUM_TESTS * NUM_TESTS] = {
        '\x7f', '\x00', '\x00', '\x00', '\x00', '\x00', /* U-0000007F */
        '\xdf', '\xbf', '\x00', '\x00', '\x00', '\x00', /* U-000007FF */
        '\xef', '\xbf', '\xbf', '\x00', '\x00', '\x00', /* U-0000FFFF */
        '\xf7', '\xbf', '\xbf', '\xbf', '\x00', '\x00', /* U-001FFFFF */
        '\xfb', '\xbf', '\xbf', '\xbf', '\xbf', '\x00', /* U-03FFFFFF */
        '\xfd', '\xbf', '\xbf', '\xbf', '\xbf', '\xbf'  /* U-7FFFFFFF */
    };

    int error_counts[NUM_TESTS] = { 0, 0, 1, 1, 1, 1 };

    bool expected_success[NUM_TESTS] = { true, true, true, false, false, false };

    params = TestParams_new(
            NUM_TESTS,
            references,
            unknowns_sz,
            unknowns,
            error_counts,
            expected_success);

    parametrized_decode(params);

    TestParams_destroy(params);
}
#undef NUM_TESTS

#define NUM_TESTS 5
void test_decode_boundary_other(void)
{
    TestParams* params = 0;

    utf_rune references[NUM_TESTS] = {
        0x0000D7FF,
        0x0000E000,
        0x0000FFFD,
        0,
        0,
    };

    char unknowns[NUM_TESTS * NUM_TESTS] = {
        '\xed', '\x9f', '\xbf', '\x00', '\x00',      /* U-0000D7FF */
        '\xee', '\x80', '\x80', '\x00', '\x00',      /* U-0000E000 */
        '\xef', '\xbf', '\xbd', '\x00', '\x00',      /* U-0000FFFD */
        '\xf4', '\x8f', '\xbf', '\xbf', '\x00',      /* U-0010FFFF */
        '\xf4', '\x90', '\x80', '\x80', '\x00',      /* U-00110000 */
    };

    int unknowns_sz[NUM_TESTS] = { 3, 3, 3, 4, 4 };

    int error_counts[NUM_TESTS] = { 0, 0, 0, 1, 1 };

    bool expected_success[NUM_TESTS] = { true, true, true, true, false };

    params = TestParams_new(
            NUM_TESTS,
            references,
            unknowns_sz,
            unknowns,
            error_counts,
            expected_success);

    parametrized_decode(params);

    TestParams_destroy(params);
}
#undef NUM_TESTS

#define NUM_TESTS 8
void test_decode_malformed_continuation_seq(void)
{
    TestParams* params = 0;

    utf_rune references[NUM_TESTS] = {0};

    char unknowns[NUM_TESTS * NUM_TESTS] = {
        '\x80', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\xbf', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x80', '\xbf', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x80', '\xbf', '\x80', '\xbf', '\x00', '\x00', '\x00', '\x00',
        '\x80', '\xbf', '\x80', '\xbf', '\x80', '\x00', '\x00', '\x00',
        '\x80', '\xbf', '\x80', '\xbf', '\x80', '\xbf', '\x00', '\x00',
        '\x80', '\xbf', '\x80', '\xbf', '\x80', '\xbf', '\x00', '\x00',
        '\x80', '\xbf', '\x80', '\xbf', '\x80', '\xbf', '\x80', '\x00',
    };

    int unknowns_sz[NUM_TESTS] = { 1, 1, 2, 3, 4, 5, 6, 7 };

    int error_counts[NUM_TESTS] = { 1, 1, 2, 3, 4, 5, 6, 7 };

    bool expected_success[NUM_TESTS] = { false, false, false, false, false, false, false, false };

    params = TestParams_new(
            NUM_TESTS,
            references,
            unknowns_sz,
            unknowns,
            error_counts,
            expected_success);

    parametrized_decode(params);

    TestParams_destroy(params);
}
#undef NUM_TESTS

void test_decode_malformed_continuation_all(void)
{
    utf_rune dest[65] = {0};
    char* src = 0;
    int n = 0;
    int i;
    int error_count = 0;

    src = (char*)calloc(65, sizeof(char));
    for (i = 0; i < 64; i++) {
        src[i] = (char)((unsigned)'\x80' + (unsigned int)i);
    }
    src[64] = '\x00';

    utf_errno = 0;

    for (i = 0; src[i] != 0; i += n) {
        n = utf_decoderune(&dest[i], &src[i], UTF_BYTES_MAX);
        TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
        error_count += 1;
        utf_errno = 0;
    }

    TEST_ASSERT_EQUAL_MESSAGE(64, error_count, "Has correct number of decode errors");

    free(src);
}

void test_decode_malformed_lonely_start_2(void)
{
    utf_rune dest[66] = {0};
    char* src = 0;
    int n = 0;
    int i;
    int error_count = 0;

    src = (char*)calloc(66, sizeof(char));
    for (i = 0; i < 64; i+=2) {
        src[i]   = (char)((unsigned)'\xC0' + (unsigned int)(i / 2));
        src[i+1] = ' ';
    }
    src[65] = '\x00';

    utf_errno = 0;

    for (i = 0; src[i] != 0 && i < 64; i++, i += n) {
        n = utf_decoderune(&dest[i], &src[i], UTF_BYTES_MAX);
        if (src[i] == ' ') {
            TEST_ASSERT_MESSAGE(!utf_errno, "Decode unexpectedly failed");
        } else {
            TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
            error_count += 1;
            utf_errno = 0;
        }
    }

    TEST_ASSERT_EQUAL_MESSAGE(32, error_count, "Has correct number of decode errors");

    free(src);
}

void test_decode_malformed_lonely_start_3(void)
{
    utf_rune dest[34] = {0};
    char* src = 0;
    int n = 0;
    int i;
    int error_count = 0;

    src = (char*)calloc(34, sizeof(char));
    for (i = 0; i < 32; i+=2) {
        src[i]   = (char)((unsigned)'\xE0' + (unsigned int)(i / 2));
        src[i+1] = ' ';
    }
    src[33] = '\x00';

    utf_errno = 0;

    for (i = 0; src[i] != 0 && i < 32; i += n) {
        n = utf_decoderune(&dest[i], &src[i], UTF_BYTES_MAX);
        if (src[i] == ' ') {
            TEST_ASSERT_MESSAGE(!utf_errno, "Decode unexpectedly failed");
        } else {
            TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
            error_count += 1;
            utf_errno = 0;
        }
    }

    TEST_ASSERT_EQUAL_MESSAGE(16, error_count, "Has correct number of decode errors");

    free(src);
}

void test_decode_malformed_lonely_start_4(void)
{
    utf_rune dest[18] = {0};
    char* src = 0;
    int n = 0;
    int i;
    int error_count = 0;

    src = (char*)calloc(18, sizeof(char));
    for (i = 0; i < 16; i+=2) {
        src[i]   = (char)((unsigned)'\xF0' + (unsigned int)(i / 2));
        src[i+1] = ' ';
    }
    src[17] = '\x00';

    utf_errno = 0;

    for (i = 0; src[i] != 0 && i < 16; i += n) {
        n = utf_decoderune(&dest[i], &src[i], UTF_BYTES_MAX);
        if (src[i] == ' ') {
            TEST_ASSERT_MESSAGE(!utf_errno, "Decode unexpectedly failed");
        } else {
            TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
            error_count += 1;
            utf_errno = 0;
        }
    }

    TEST_ASSERT_EQUAL_MESSAGE(8, error_count, "Has correct number of decode errors");

    free(src);
}

void test_decode_malformed_lonely_start_5(void)
{
    utf_rune dest[9] = {0};
    char* src = "\xF8 \xF9 \xFA \xFB ";
    int n = 0;
    int i;
    int error_count = 0;

    utf_errno = 0;

    for (i = 0; src[i] != 0 && i < 8; i += n) {
        n = utf_decoderune(&dest[i], &src[i], UTF_BYTES_MAX);
        if (src[i] == ' ') {
            TEST_ASSERT_MESSAGE(!utf_errno, "Decode unexpectedly failed");
        } else {
            TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
            error_count += 1;
            utf_errno = 0;
        }
    }

    TEST_ASSERT_EQUAL_MESSAGE(4, error_count, "Has correct number of decode errors");
}

void test_decode_malformed_lonely_start_6(void)
{
    utf_rune dest[5] = {0};
    char* src = "\xFC \xFD ";
    int n = 0;
    int i;
    int error_count = 0;

    utf_errno = 0;

    for (i = 0; src[i] != 0 && i < 4; i += n) {
        n = utf_decoderune(&dest[i], &src[i], UTF_BYTES_MAX);
        if (src[i] == ' ') {
            TEST_ASSERT_MESSAGE(!utf_errno, "Decode unexpectedly failed");
        } else {
            TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
            error_count += 1;
            utf_errno = 0;
        }
    }

    TEST_ASSERT_EQUAL_MESSAGE(2, error_count, "Has correct number of decode errors");
}

#define NUM_TESTS 5
void test_decode_boundary_start_last_missing(void)
{
    TestParams* params = 0;

    utf_rune references[NUM_TESTS] = {0};

    int unknowns_sz[NUM_TESTS] = { 1, 2, 3, 4, 5 };

    char unknowns[NUM_TESTS * NUM_TESTS] = {
        '\xc2', '\x00', '\x00', '\x00', '\x00', /* U-00000080 */
        '\xe0', '\xa0', '\x00', '\x00', '\x00', /* U-00000800 */
        '\xf0', '\x90', '\x80', '\x00', '\x00', /* U-00010000 */
        '\xf8', '\x88', '\x80', '\x80', '\x00', /* U-00200000 */
        '\xfc', '\x84', '\x80', '\x80', '\x80', /* U-04000000 */
    };

    int error_counts[NUM_TESTS] = { 1, 1, 1, 1, 1 };

    bool expected_success[NUM_TESTS] = { false, false, false, false, false };

    params = TestParams_new(
            NUM_TESTS,
            references,
            unknowns_sz,
            unknowns,
            error_counts,
            expected_success);

    parametrized_decode(params);

    TestParams_destroy(params);
}
#undef NUM_TESTS

#define NUM_TESTS 5
void test_decode_boundary_end_last_missing(void)
{
    TestParams* params = 0;

    utf_rune references[NUM_TESTS] = {0};

    int unknowns_sz[NUM_TESTS] = { 1, 2, 3, 4, 5 };

    char unknowns[NUM_TESTS * NUM_TESTS] = {
        '\xdf', '\x00', '\x00', '\x00', '\x00', /* U-000007FF */
        '\xef', '\xbf', '\x00', '\x00', '\x00', /* U-0000FFFF */
        '\xf7', '\xbf', '\xbf', '\x00', '\x00', /* U-001FFFFF */
        '\xfb', '\xbf', '\xbf', '\xbf', '\x00', /* U-03FFFFFF */
        '\xfd', '\xbf', '\xbf', '\xbf', '\xbf', /* U-7FFFFFFF */
    };

    int error_counts[NUM_TESTS] = { 1, 1, 1, 1, 1 };

    bool expected_success[NUM_TESTS] = { false, false, false, false, false };

    params = TestParams_new(
            NUM_TESTS,
            references,
            unknowns_sz,
            unknowns,
            error_counts,
            expected_success);

    parametrized_decode(params);

    TestParams_destroy(params);
}
#undef NUM_TESTS

void test_decode_incomplete_seq_concat(void)
{
    utf_rune dest[41] = {0};
    int n = 0;
    int i;
    int error_count = 0;
    char* src = "\xc2\xe0\xa0\xf0\x90\x80\xf8\x88\x80\x80\xfc\x84\x80\x80\x80\xdf\xef\xbf\xf7\xbf\xbf\xfb\xbf\xbf\xbf\xfd\xbf\xbf\xbf\xbf";

    utf_errno = 0;

    for (i = 0; *src != 0 && i < 41; i++, src += n) {
        n = utf_decoderune(&dest[i], src, UTF_BYTES_MAX);
        TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
        error_count += 1;
        utf_errno = 0;
    }

    TEST_ASSERT_EQUAL_MESSAGE(10, error_count, "Has correct number of decode errors");
}

void test_decode_impossible_seq(void)
{
    utf_rune dest[5] = {0};
    int n = 0;
    int i;
    int error_count = 0;
    char* seq1 = "\xfe";
    char* seq2 = "\xff";
    char* seq3 = "\xfe\xfe\xff\xff";

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq1, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(1, n, "Has correct return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq2, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(1, n, "Has correct return value");

    utf_errno = 0;

    for (i = 0; *seq3 != 0 && i < 5; i++, seq3 += n) {
        n = utf_decoderune(&dest[i], seq3, UTF_BYTES_MAX);
        TEST_ASSERT_MESSAGE(utf_errno, "Decode unexpectedly succeeded");
        error_count += 1;
        utf_errno = 0;
    }

    TEST_ASSERT_EQUAL_MESSAGE(2, error_count, "Has correct number of decode errors");
}

void test_decode_overlong_slash(void)
{
    utf_rune dest[7] = {0};
    int n = 0;
    char* seq1 = "\xc0\xaf";
    char* seq2 = "\xe0\x80\xaf";
    char* seq3 = "\xf0\x80\x80\xaf";
    char* seq4 = "\xf8\x80\x80\x80\xaf";
    char* seq5 = "\xfc\x80\x80\x80\x80\xaf";

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq1, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq1 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(2, n, "seq1 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq2, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq2 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq2 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq3, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq3 ecode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(4, n, "seq3 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq4, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq4 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(5, n, "seq5 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq5, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq5 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(6, n, "seq5 decode has an incorrect return value");
}

void test_decode_overlong_max(void)
{
    utf_rune dest[7] = {0};
    int n = 0;
    char* seq1 = "\xc1\xbf";
    char* seq2 = "\xe0\x9f\xbf";
    char* seq3 = "\xf0\x8f\xbf\xbf";
    char* seq4 = "\xf8\x87\xbf\xbf\xbf";
    char* seq5 = "\xfc\x83\xbf\xbf\xbf\xbf";

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq1, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq1 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(2, n, "seq1 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq2, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq2 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq2 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq3, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq3 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(4, n, "seq3 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq4, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq4 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(5, n, "seq4 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq5, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq5 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(6, n, "seq5 decode has an incorrect return value");
}

void test_decode_overlong_NUL(void)
{
    utf_rune dest[7] = {0};
    int n = 0;
    char* seq1 = "\xc0\x80";
    char* seq2 = "\xe0\x80\x80";
    char* seq3 = "\xf0\x80\x80\x80";
    char* seq4 = "\xf8\x80\x80\x80\x80";
    char* seq5 = "\xfc\x80\x80\x80\x80\x80";

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq1, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq1 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(2, n, "seq1 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq2, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq2 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq2 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq3, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq3 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(4, n, "seq3 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq4, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq4 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(5, n, "seq4 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq5, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq5 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(6, n, "seq5 decode has an incorrect return value");
}

void test_decode_illegal_single_surrogate(void)
{
    utf_rune dest[4] = {0};
    int n = 0;
    char* seq1 = "\xed\xa0\x80";
    char* seq2 = "\xed\xad\xbf";
    char* seq3 = "\xed\xae\x80";
    char* seq4 = "\xed\xaf\xbf";
    char* seq5 = "\xed\xb0\x80";
    char* seq6 = "\xed\xbe\x80";
    char* seq7 = "\xed\xbf\xbf";

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq1, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq1 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq1 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq2, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq2 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq2 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq3, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq3 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq3 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq4, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq4 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq4 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq5, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq5 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq5 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq6, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq6 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq6 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq7, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq7 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq7 decode has an incorrect return value");
}

void test_decode_illegal_paired_surrogates(void)
{
    utf_rune dest[7] = {0};
    int n = 0;
    char* seq1 = "\xed\xa0\x80\xed\xb0\x80";
    char* seq2 = "\xed\xa0\x80\xed\xbf\xbf";
    char* seq3 = "\xed\xad\xbf\xed\xb0\x80";
    char* seq4 = "\xed\xad\xbf\xed\xbf\xbf";
    char* seq5 = "\xed\xae\x80\xed\xb0\x80";
    char* seq6 = "\xed\xae\x80\xed\xbf\xbf";
    char* seq7 = "\xed\xaf\xbf\xed\xb0\x80";
    char* seq8 = "\xed\xaf\xbf\xed\xbf\xbf";

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq1, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq1 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq1 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq2, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq2 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq2 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq3, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq3 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq3 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq4, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq4 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq4 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq5, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq5 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq5 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq6, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq6 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq6 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq7, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq7 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq7 decode has an incorrect return value");

    utf_errno = 0;

    n = utf_decoderune(&dest[0], seq8, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq8 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq8 decode has an incorrect return value");
}

void test_decode_problematic_nonchar(void)
{
    utf_rune dest[121] = {0};
    int n;
    char* seq1 = "\xef\xbf\xbe";
    char* seq2 = "\xef\xbf\xbf";
    char* seq3 = 0;
    char* seq4 = 0;
    int i;
    utf_rune v;
    int error_count;

    seq3 = (char*)calloc(97, sizeof(char));
    for (i = 0; i < 96; i += 3) {
        seq3[i]   = '\xef';
        seq3[i+1] = '\xb7';
        seq3[i+2] = (char)((unsigned)'\x90' + (unsigned int)(i / 3));
    }
    seq3[96] = '\x00';

    seq4 = (char*)calloc(135, sizeof(char));
    for (i = 0, n = 0, v = 0xFFFE; i < 135 && v < 0x10FFFF; v+=0xFFFF) {
        n = utf_encoderune(&seq4[i], &v);
        i += n;
        v++;
        n = utf_encoderune(&seq4[i], &v);
        i += n;
    }
    seq4[134] = '\x00';

    utf_errno = 0;
    n = utf_decoderune(&dest[0], seq1, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq1 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq1 decode has an incorrect return value");

    utf_errno = 0;
    n = utf_decoderune(&dest[0], seq2, UTF_BYTES_MAX);
    TEST_ASSERT_MESSAGE(utf_errno, "seq2 decode unexpectedly succeeded");
    TEST_ASSERT_EQUAL_MESSAGE(3, n, "seq2 decode has an incorrect return value");

    utf_errno = 0;
    error_count = 0;
    for (i = 0, n = 0; seq3[i] != 0 && i < 94; i += n) {
        n = utf_decoderune(&dest[i], &seq3[i], UTF_BYTES_MAX);
        TEST_ASSERT_MESSAGE(utf_errno, "seq3 decode unexpectedly succeeded");
        error_count += 1;
        utf_errno = 0;
    }

    TEST_ASSERT_EQUAL_MESSAGE(32, error_count, "seq3 has incorrect number of decode errors");

    utf_errno = 0;
    error_count = 0;
    for (i = 0, n = 0; seq4[i] != 0 && i < 135; i += n) {
        n = utf_decoderune(&dest[i], &seq4[i], UTF_BYTES_MAX);
        TEST_ASSERT_MESSAGE(utf_errno, "seq4 decode unexpectedly succeeded");
        error_count += 1;
        utf_errno = 0;
    }

    TEST_ASSERT_EQUAL_MESSAGE(34, error_count, "seq4 has incorrect number of decode errors");

    free(seq4);
    free(seq3);
}

int main(void)
{
    UNITY_BEGIN();
    if (TEST_PROTECT()) {
        RUN_TEST(test_decode_kosme);
        RUN_TEST(test_decode_boundary_start);
        RUN_TEST(test_decode_boundary_end);
        RUN_TEST(test_decode_boundary_other);
        RUN_TEST(test_decode_malformed_continuation_seq);
        RUN_TEST(test_decode_malformed_continuation_all);
        RUN_TEST(test_decode_malformed_lonely_start_2);
        RUN_TEST(test_decode_malformed_lonely_start_3);
        RUN_TEST(test_decode_malformed_lonely_start_4);
        RUN_TEST(test_decode_malformed_lonely_start_5);
        RUN_TEST(test_decode_malformed_lonely_start_6);
        RUN_TEST(test_decode_boundary_start_last_missing);
        RUN_TEST(test_decode_boundary_end_last_missing);
        RUN_TEST(test_decode_incomplete_seq_concat);
        RUN_TEST(test_decode_impossible_seq);
        RUN_TEST(test_decode_overlong_slash);
        RUN_TEST(test_decode_overlong_max);
        RUN_TEST(test_decode_overlong_NUL);
        RUN_TEST(test_decode_illegal_single_surrogate);
        RUN_TEST(test_decode_illegal_paired_surrogates);
        RUN_TEST(test_decode_problematic_nonchar);
    }
    return UNITY_END();
}
