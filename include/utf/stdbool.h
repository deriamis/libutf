/*
 * 	This source code has been placed into the PUBLIC DOMAIN by its author.
*/

#if !defined(_STDBOOL_H) && !defined(_INC_STDBOOL)
#define _STDBOOL_H
#define _INC_STDBOOL

#if !defined(__cplusplus)
/* Applications can define these... */
#undef bool
#undef true
#undef false
#undef __bool_true_false_are_defined

#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1

#if !defined(__clang__) && !defined(__GNUC__)
#  if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
/* Not built into pre-C99 compilers. */
typedef int _Bool;
#  endif /* !defined(__STDC_VERSION__) || __STDC_VERSION__ <= 199901L */
#endif /* !defined(__clang__) && !defined(__GNUC__) */

#endif /* __cplusplus */
#endif /* !defined(_STDBOOL_H) && !defined(_INC_STDBOOL) */
