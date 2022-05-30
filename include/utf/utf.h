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

#ifndef UTF_H
#define UTF_H

#include <stddef.h>
#include "platform.h"

#define UTF_RUNE_C(c) UINT32_C(c)
typedef uint_least32_t UTF_Rune;

/* UTF rune value ranges, defined by RFC-3629. */
#define UTF_RUNE_BYTES_MAX 4
#define UTF_RUNE_VALUE_MIN RUNE_C(0x80)
#define UTF_RUNE_VALUE_MAX RUNE_C(0x10FFFF)

/* Indicates an error decoding a UTF rune */
#define UTF_RUNE_ERROR ((UTF_RUNE) - 1)

/* Masks for processing UTF byte streams. */
#define UTF_MASK_SQU_TAG RUNE_C(0x0E)
#define UTF_MASK_BOM_BE RUNE_C(0xFEFF)
#define UTF_MASK_BOM_LE RUNE_C(0xFFFE)
#define UTF_MASK_WORD_JOIN RUNE_C(0x2060)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Utility functions
*/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"
/* Return whether c could be the first byte of an encoded rune. This does not
guarantee the byte is a valid rune by itself. */
bool utf_runestart(char c);

/* Return whether w is a valid UTF rune byte sequence. */
bool utf_validrune(uint_least32_t w);

/* Return whether s is long enough to be decoded into a UTF string. This does
not guarantee that s is a valid UTF string. */
bool utf_fullrune(const char* s, size_t len);

/* Return the number of runes (not bytes) in s. */
size_t utf_runecount(const char* s, size_t len);

/* Return whether s is a valid UTF string. */
bool utf_validstring(const char* s, size_t len);

/* Return the number of bytes required to encode a single word to a UTF rune. */
size_t utf_runelen(uint_least32_t w);

/* Return the number of bytes required to encode all bytes up to len in s to
UTF runes. */
size_t utf_runenlen(const char* s, size_t len);



/*
 * Conversion functions
*/

/* Copy from src to dest one rune at a time. */
size_t utf_mbsrtoufs(UTF_Rune* dest, const char* src);

/* Copy up to n runes from src to dest one rune at a time. */
size_t utf_mbsnrtoufs(UTF_Rune* dest, const char* src, size_t n);

/* Copy from src to dest one byte at a time. */
size_t utf_utfsrtombs(char* dest, const UTF_Rune* src);

/* Copy up to n bytes from src to dest one byte at a time. */
size_t utf_utfsnrtombs(char* dest, const UTF_Rune* src, size_t n);

/* Convert a single word to its corresponding UTF rune. */
size_t utf_decoderune(UTF_Rune* dest, const char* src, size_t len);

/* Convert a single rune to its corresponding word. */
size_t utf_encoderune(char* dest, const UTF_Rune* r);


/*
 * Single-rune functions
*/

/* Return whether r is either a number or alpha. */
bool utf_isalnum(UTF_Rune r);

/* Return whether r is printable but is neither a blank or a graphic. */
bool utf_isalpha(UTF_Rune r);

/* Return whether r is a non-printable control sequence. */
bool utf_iscntrl(UTF_Rune r);

/* Return whether r is a numeric digit. */
bool utf_isdigit(UTF_Rune r);

/* Return whether r is a graphic. */
bool utf_isgraph(UTF_Rune r);

/* Return whether r is a lower-case letter. */
bool utf_islower(UTF_Rune r);

/* Return whether r is printable. */
bool utf_isprint(UTF_Rune r);

/* Return whether r is punctuation. */
bool utf_ispunct(UTF_Rune r);

/* Return whether r is whitespace. */
bool utf_isblank(UTF_Rune r);

/* Return whether r is a space character. */
bool utf_isspace(UTF_Rune r);

/* Return whether r is an upper-case letter. */
bool utf_isupper(UTF_Rune r);

/* Return whether r is a hex digit. */
bool utf_isxdigit(UTF_Rune r);

/* If r is a letter, return its lower-case version, if one exists. */
UTF_Rune utf_tolower(UTF_Rune r);

/* If r is a letter, return its upper-case version, if one exists. */
UTF_Rune utf_toupper(UTF_Rune r);


/*
 * Rune standard string functions
*/

/* Append src onto the end of dest */
UTF_Rune* utf_strcat(UTF_Rune* restrict dest, const UTF_Rune* restrict src);

/* Append src onto the end of dest, up to n runes */
UTF_Rune* utf_strncat(UTF_Rune* restrict dest, const UTF_Rune* restrict src, size_t n);

/* Compare s1 to s2 */
int utf_strcmp(const UTF_Rune* s1, const UTF_Rune* s2);

/* Compare s1 to s2 up to n runes */
int utf_strncmp(const UTF_Rune* s1, const UTF_Rune* s2, size_t n);

/* Compare s1 to s2, ignoring case. */
int utf_strcasecmp(const UTF_Rune* s1, const UTF_Rune* s2);

/* Compare s1 to s2 up to n runes, ignoring case. */
int utf_strncasecmp(const UTF_Rune* s1, const UTF_Rune* s2, size_t n);

/* Copy src to dest and overwrite */
UTF_Rune* utf_strcpy(UTF_Rune* restrict dest, const UTF_Rune* restrict src);

/* Copy src to dest and overwrite up to n runes */
UTF_Rune* utf_strncpy(UTF_Rune* restrict dest, const UTF_Rune* restrict src, size_t n);

/* Return the number of runes in s up to but not including the terminating null
character. */
size_t utf_strlen(const UTF_Rune* s);

/* Identical to utf_strlen(), except that 0 is returned if s is a null pointer
and n is returned if the null character was not found in the first n runes of
s. */
size_t utf_strnlen_s(const UTF_Rune* s, size_t n);

/* Return a pointer to the first occurrence of r in s. */
UTF_Rune* utf_strchr(const UTF_Rune* s, UTF_Rune r);

/* Return a pointer to the first occurrence of r in s, up to n runes. */
UTF_Rune* utf_strnchr(const UTF_Rune* s, UTF_Rune r, size_t n);

/* Return a pointer to the last occurrence of r in s. */
UTF_Rune* utf_strrchr(const UTF_Rune* s, UTF_Rune r);

/* Return a pointer to the last occurrence of r in s, up to n runes. */
UTF_Rune* utf_strnrchr(const UTF_Rune* s, UTF_Rune r, size_t n);

/* Return a new copy of s. */
UTF_Rune* utf_strdup(const UTF_Rune* s);

/* Find the first occurrence of the entire string s2 in s1, not including the
terminating null character, and return a pointer to the start of the substring
in s1. */
UTF_Rune* utf_strstr(const UTF_Rune* s1, const UTF_Rune* s2);

/* Break s into a series of tokens separated by delim. */
UTF_Rune* utf_strtok(const UTF_Rune* restrict s, const UTF_Rune* restrict delim);

/* Thread-safe utf_strtok(), with buf as a user-allocated buffer used during
string parsing. buf must remain the same between calls. */
UTF_Rune* utf_strtok_r(const UTF_Rune* s, const UTF_Rune* delim, UTF_Rune** buf);

/* Calculate the length of the initial segment of s1 which consists entirely
of runes in s2. */
size_t utf_strspn(const UTF_Rune* s1, const UTF_Rune* s2);

/* Calculate the number of runes in the initial segment of s1 which consists
entirely of runes *not* in s2. */
size_t utf_strcspn(const UTF_Rune* s1, const UTF_Rune* s2);

/* Return the first rune in s1 that matches any rune in s2. */
UTF_Rune* utf_strpbrk(const UTF_Rune* s1, const UTF_Rune* s2);

/* Read up to n runes from stdin until a newline is found or EOF occurs. Writes
only at most n-1 runes into s and always writes the terminating null character
unless s is a null pointer. The newline character, if found, is discarded and
does not count toward the number of bytes written to s. */
UTF_Rune* utf_gets_s(UTF_Rune* s, size_t n);

/* Randomize a string s using rand() to swap runes. The returned result is an
anagram of s. */
UTF_Rune* utf_strfry(UTF_Rune* s);

/* Return a pointer to an error message string corresponding to errnum. */
UTF_Rune* utf_strerror(int errnum);

/* Thread-safe utf_strerror(). Either returns an immutable static string or a
string of buflen runes allocated in buf. The string in buf always includes a
terminating null byte. */
UTF_Rune* utf_strerror_r(int errnum, UTF_Rune* buf, size_t buflen);

/*
 * Rune extended string functions
*/

/* Copy input up to a null byte into output, compressing any C-language escape
sequences into the equivalent one-byte character, and return output. A null byte
is appended to the output. output must point to a buffer large enough to hold
the result. If output is at least as large as input, it is guaranteed to be big
enough. */
UTF_Rune* utf_strccpy(UTF_Rune* restrict output, const UTF_Rune* restrict input);

/* Identical to utf_strccpy(), except that a pointer is returned to the null
byte at the end of output. */
UTF_Rune* utf_strcadd(UTF_Rune* restrict output, const UTF_Rune* restrict input);

/* Copy input to output, expanding any non-graphic one-byte characters to their
equivalent C-language escape sequences (the reverse of utf_strccpy() above) and
return output. Any byte sequences in exceptions are not expanded. output must
point to a buffer large enough to hold the result. If output is at least four
times as large as input, it is guaranteed to be big enough. */
UTF_Rune* utf_strecpy(UTF_Rune* restrict output, const UTF_Rune* restrict input, const UTF_Rune* restrict exceptions);

/* Identical to utf_strecpy(), except that a pointer is returned to the null
byte at the end of output. */
UTF_Rune* utf_streadd(UTF_Rune* restrict output, const UTF_Rune* restrict input, const UTF_Rune* restrict exceptions);

/* Return the offset of the first occurence of the s2 if it is a substring of s1;
otherwise, return -1 */
int utf_strfind(const UTF_Rune* restrict s1, const UTF_Rune* restrict s2);

/* Search from the end of s1 for the first rune that is not contained in s2 and
return a pointer to the next rune otherwise, return a pointer to s1 */
UTF_Rune* utf_strrspn(const UTF_Rune* s1, const UTF_Rune* s2);

/* Copy runes from s into result. Any rune that appears in old will be
replaced with the rune in the same position in new. */
UTF_Rune* utf_strtrns(const UTF_Rune* restrict s, const UTF_Rune* restrict _old, const UTF_Rune* restrict _new, UTF_Rune* restrict result);

#pragma clang diagnostic pop

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UTF_H */
