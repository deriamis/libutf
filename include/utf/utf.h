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
typedef uint_least32_t utf_rune;

/* UTF rune value ranges, defined by RFC-3629. */
#define UTF_BYTES_MAX 6
#define UTF_ASCII_MAX UTF_RUNE_C(0x7F)
#define UTF_UTF8_MIN UTF_RUNE_C(0x80)
#define UTF_MAX_VALUE UTF_RUNE_C(0x10FFFF)

#define UTF_API __attribute__((visibility("default")))

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"


/*
 * Error codes
 */
#define UTF_SUCCESS 0
#define UTF_ESTART  1
#define UTF_ETRUNC  2
#define UTF_EVALUE  3
#define UTF_ELEN    4
#define UTF_ESEQ    5
#define UTF_ELIMIT  6
#define UTF_MAX_ERRNO UTF_ELIMIT


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Decoder/Encoder state
 */

extern int utf_errno;
typedef struct utf_state_s utf_state_s;
typedef struct utf_state_s* utf_state_p;

/* Return a newly-constructed state struct for use in reentrant functions. */
UTF_API utf_state_p utf_state_new(void);

/* Destroy and deallocate a state struct. */
UTF_API void utf_state_destroy(utf_state_p state);

/* Clear a state struct to reuse as if newly-created. */
UTF_API void utf_state_clear(utf_state_p state);

/* Return the state error code from a state struct. */
UTF_API int utf_state_get_error(utf_state_p state);

/* Return whether a state struct has a non-success error code. */
UTF_API bool utf_state_has_error(utf_state_p state);

/* Return the number of bytes processed by the current encoder state. */
UTF_API int utf_state_bytes_processed(utf_state_p state);


/*
 * Rune property functions
 */

/* Return whether w is the start of a surrogate pair sequence. */
UTF_API bool utf_is_pair_start(uint_least32_t w);

/* Return whether w is the end of a surrogate pair sequence. */
UTF_API bool utf_is_pair_end(uint_least32_t w);

/* Return whether w is the end of a surrogate pair sequence. */
UTF_API bool utf_is_pair(uint_least32_t w);

/* Return whether c could be the first byte of an encoded rune. This does not
guarantee the byte is a valid rune by itself. */
UTF_API bool utf_is_rune_start(char c);

/* Return whether w is a valid UTF rune byte sequence. */
UTF_API bool utf_is_valid_rune(uint_least32_t w);

/* Return whether s is long enough to be decoded into a UTF string. This does
not guarantee that s is a valid UTF string. */
UTF_API bool utf_is_full_rune(const char* s, size_t len);

/* Return the number of runes (not bytes) in s. */
UTF_API size_t utf_runecount(const char* s, size_t len);

/* Return whether s is a valid UTF string. */
UTF_API bool utf_validstring(const char* s, size_t len);

/* Return the number of bytes required to encode a single word to a UTF rune. */
UTF_API int utf_runelen(uint_least32_t w);

/* Return the number of bytes required to encode all bytes up to len in s to
UTF runes. */
UTF_API int utf_runenlen(const char* s, size_t len);

/* Return whether r is either a digit or alphabetic. */
UTF_API bool utf_isalnum(utf_rune r);

/* Return whether r is printable but is neither a blank or a graphic. */
UTF_API bool utf_isalpha(utf_rune r);

/* Return whether r is a non-printable control sequence. */
UTF_API bool utf_iscntrl(utf_rune r);

/* Return whether r is a numeric digit. */
UTF_API bool utf_isdigit(utf_rune r);

/* Return whether r is a number. */
UTF_API bool utf_isnumber(utf_rune r);

/* Return whether r is an integer. */
UTF_API bool utf_isinteger(utf_rune r);

/* Return whether r is a graphic. */
UTF_API bool utf_isgraph(utf_rune r);

/* Return whether r is a lower-case letter. */
UTF_API bool utf_islower(utf_rune r);

/* Return whether r is printable. */
UTF_API bool utf_isprint(utf_rune r);

/* Return whether r is punctuation. */
UTF_API bool utf_ispunct(utf_rune r);

/* Return whether r is whitespace. */
UTF_API bool utf_isblank(utf_rune r);

/* Return whether r is a space character. */
UTF_API bool utf_isspace(utf_rune r);

/* Return whether r is an upper-case letter. */
UTF_API bool utf_isupper(utf_rune r);

/* Return whether r is a title-case letter. */
UTF_API bool utf_istitle(utf_rune r);

/* Return whether r is a symbol. */
UTF_API bool utf_issymbol(utf_rune r);

/* Return whether r is a currency symbol. */
UTF_API bool utf_iscurrency(utf_rune r);

/* Return whether r is a mathematics symbol. */
UTF_API bool utf_ismath(utf_rune r);

/* Return whether r is a hex digit. */
UTF_API bool utf_isxdigit(utf_rune r);


/*
 * Encoding functions
 */

/* Convert a single rune to its corresponding word. */
UTF_API int utf_encoderune_r(char* dest restrict, utf_rune* src restrict,
                             utf_state_p state);
UTF_API int utf_encoderune(char* dest restrict, utf_rune* src restrict);

/* Copy from src to dest one byte at a time. */
UTF_API int utf_utfsrtombs(char* dest, const utf_rune* src);

/* Copy up to n bytes from src to dest one byte at a time. */
UTF_API int utf_utfsnrtombs(char* dest, const utf_rune* src, size_t n);


/*
 * Decoding functions
 */

/* Convert a single word to its corresponding UTF rune. */
UTF_API int utf_decoderune_r(utf_rune* dest restrict, char* src restrict,
                             int len, utf_state_p state);
UTF_API int utf_decoderune(utf_rune* dest restrict, char* src restrict,
                           int len);

/* Copy from src to dest one rune at a time. */
UTF_API int utf_mbsrtoufs(utf_rune* dest, const char* src);

/* Copy up to n runes from src to dest one rune at a time. */
UTF_API int utf_mbsnrtoufs(utf_rune* dest, const char* src, size_t n);


/*
 * Transformation functions
 */

/* If r is a letter, return its lower-case version, if one exists. */
UTF_API utf_rune utf_tolower(utf_rune r);

/* If r is a letter, return its upper-case version, if one exists. */
UTF_API utf_rune utf_toupper(utf_rune r);

/* If r is a letter, return its title-case version, if one exists. */
UTF_API utf_rune utf_totitle(utf_rune r);

/* Return whether r is a numeric rune with a value. */
UTF_API bool utf_hasvalue(utf_rune r);

/* Return the integer value of r. */
UTF_API int utf_intvalue(utf_rune r);

/* Return the floating-point value of r. */
UTF_API double utf_valueof(utf_rune r);


/*
 * Rune standard string functions
*/

/* Append src onto the end of dest */
UTF_API utf_rune* utf_strcat(utf_rune* restrict dest, const utf_rune* restrict src);

/* Append src onto the end of dest, up to n runes */
UTF_API utf_rune* utf_strncat(utf_rune* restrict dest, const utf_rune* restrict src, size_t n);

/* Compare s1 to s2 */
UTF_API int utf_strcmp(const utf_rune* s1, const utf_rune* s2);

/* Compare s1 to s2 up to n runes */
UTF_API int utf_strncmp(const utf_rune* s1, const utf_rune* s2, size_t n);

/* Compare s1 to s2, ignoring case. */
UTF_API int utf_strcasecmp(const utf_rune* s1, const utf_rune* s2);

/* Compare s1 to s2 up to n runes, ignoring case. */
UTF_API int utf_strncasecmp(const utf_rune* s1, const utf_rune* s2, size_t n);

/* Copy src to dest and overwrite */
UTF_API utf_rune* utf_strcpy(utf_rune* restrict dest, const utf_rune* restrict src);

/* Copy src to dest and overwrite up to n runes */
UTF_API utf_rune* utf_strncpy(utf_rune* restrict dest, const utf_rune* restrict src, size_t n);

/* Return the number of runes in s up to but not including the terminating null
character. */
UTF_API size_t utf_strlen(const utf_rune* s);

/* Identical to utf_strlen(), except that 0 is returned if s is a null pointer
and n is returned if the null character was not found in the first n runes of
s. */
UTF_API size_t utf_strnlen_s(const utf_rune* s, size_t n);

/* Return a pointer to the first occurrence of r in s. */
UTF_API utf_rune* utf_strchr(const utf_rune* s, utf_rune r);

/* Return a pointer to the first occurrence of r in s, up to n runes. */
UTF_API utf_rune* utf_strnchr(const utf_rune* s, utf_rune r, size_t n);

/* Return a pointer to the last occurrence of r in s. */
UTF_API utf_rune* utf_strrchr(const utf_rune* s, utf_rune r);

/* Return a pointer to the last occurrence of r in s, up to n runes. */
UTF_API utf_rune* utf_strnrchr(const utf_rune* s, utf_rune r, size_t n);

/* Return a new copy of s. */
UTF_API utf_rune* utf_strdup(const utf_rune* s);

/* Find the first occurrence of the entire string s2 in s1, not including the
terminating null character, and return a pointer to the start of the substring
in s1. */
UTF_API utf_rune* utf_strstr(const utf_rune* s1, const utf_rune* s2);

/* Break s into a series of tokens separated by delim. */
UTF_API utf_rune* utf_strtok(const utf_rune* restrict s, const utf_rune* restrict delim);

/* Thread-safe utf_strtok(), with buf as a user-allocated buffer used during
string parsing. buf must remain the same between calls. */
UTF_API utf_rune* utf_strtok_r(const utf_rune* s, const utf_rune* delim, utf_rune** buf);

/* Calculate the length of the initial segment of s1 which consists entirely
of runes in s2. */
UTF_API size_t utf_strspn(const utf_rune* s1, const utf_rune* s2);

/* Calculate the number of runes in the initial segment of s1 which consists
entirely of runes *not* in s2. */
UTF_API size_t utf_strcspn(const utf_rune* s1, const utf_rune* s2);

/* Return the first rune in s1 that matches any rune in s2. */
UTF_API utf_rune* utf_strpbrk(const utf_rune* s1, const utf_rune* s2);

/* Read up to n runes from stdin until a newline is found or EOF occurs. Writes
only at most n-1 runes into s and always writes the terminating null character
unless s is a null pointer. The newline character, if found, is discarded and
does not count toward the number of bytes written to s. */
UTF_API utf_rune* utf_gets_s(utf_rune* s, size_t n);

/* Randomize a string s using rand() to swap runes. The returned result is an
anagram of s. */
UTF_API utf_rune* utf_strfry(utf_rune* s);

/* Return a pointer to an error message string corresponding to errnum. */
UTF_API char* utf_strerror(int errnum);

/* Thread-safe utf_strerror(). Either returns an immutable static string or a
string of buflen runes allocated in buf. The string in buf always includes a
terminating null byte. */
UTF_API int utf_strerror_r(int errnum, char* buf, size_t buflen);

/*
 * Rune extended string functions
*/

/* Copy input up to a null byte into output, compressing any C-language escape
sequences into the equivalent one-byte character, and return output. A null byte
is appended to the output. output must point to a buffer large enough to hold
the result. If output is at least as large as input, it is guaranteed to be big
enough. */
UTF_API utf_rune* utf_strccpy(utf_rune* restrict output, const utf_rune* restrict input);

/* Identical to utf_strccpy(), except that a pointer is returned to the null
byte at the end of output. */
UTF_API utf_rune* utf_strcadd(utf_rune* restrict output, const utf_rune* restrict input);

/* Copy input to output, expanding any non-graphic one-byte characters to their
equivalent C-language escape sequences (the reverse of utf_strccpy() above) and
return output. Any byte sequences in exceptions are not expanded. output must
point to a buffer large enough to hold the result. If output is at least four
times as large as input, it is guaranteed to be big enough. */
UTF_API utf_rune* utf_strecpy(utf_rune* restrict output, const utf_rune* restrict input, const utf_rune* restrict exceptions);

/* Identical to utf_strecpy(), except that a pointer is returned to the null
byte at the end of output. */
UTF_API utf_rune* utf_streadd(utf_rune* restrict output, const utf_rune* restrict input, const utf_rune* restrict exceptions);

/* Return the offset of the first occurence of the s2 if it is a substring of s1;
otherwise, return -1 */
UTF_API int utf_strfind(const utf_rune* restrict s1, const utf_rune* restrict s2);

/* Search from the end of s1 for the first rune that is not contained in s2 and
return a pointer to the next rune otherwise, return a pointer to s1 */
UTF_API utf_rune* utf_strrspn(const utf_rune* s1, const utf_rune* s2);

/* Copy runes from s into result. Any rune that appears in old will be
replaced with the rune in the same position in new. */
UTF_API utf_rune* utf_strtrns(const utf_rune* restrict s, const utf_rune* restrict _old, const utf_rune* restrict _new, utf_rune* restrict result);

#pragma clang diagnostic pop

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UTF_H */

