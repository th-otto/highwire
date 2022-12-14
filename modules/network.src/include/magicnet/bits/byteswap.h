/* Macros to swap the order of bytes in integer values.  m68k version.
   Copyright (C) 1997, 2002, 2008, 2011 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#if !defined _BYTESWAP_H && !defined _NETINET_IN_H && !defined _ENDIAN_H
# error "Never use <bits/byteswap.h> directly; include <byteswap.h> instead."
#endif

#ifndef _BITS_BYTESWAP_H
#define _BITS_BYTESWAP_H 1

#include <bits/types.h>

/* Swap bytes in 16 bit value.  We don't provide an assembler version
   because GCC is smart enough to generate optimal assembler output, and
   this allows for better cse.  */
#define __bswap_constant_16(x) \
  ((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8))

#if (defined(__PUREC__) || defined(__TURBOC__)) && (defined(__mc68000__) && !defined(__mcoldfire__) && !defined(__AHCC__))
static __inline __uint16_t __bswap_16(__uint16_t x) 0xe058; /* ror.w d0 */
#elif defined(__mc68000__) && !defined(__mcoldfire__) && defined(__AHCC__)
#define __bswap_16(x) __bswp__(x)
#elif defined(__AHCC__) /* AHCC is not smart enough to emit that from object files */
#define __bswap_16(x) __bswap_constant_16(x)
#else
static __inline __uint16_t
__bswap_16 (__uint16_t __bsx)
{
  return __bswap_constant_16 (__bsx);
}
#endif

/* Swap bytes in 32 bit value.  */
#define __bswap_constant_32(x) \
  ((((x) & 0xff000000ul) >> 24) | (((x) & 0x00ff0000ul) >>  8) |		      \
   (((x) & 0x0000ff00ul) <<  8) | (((x) & 0x000000fful) << 24))

#if (defined(__PUREC__) || defined(__TURBOC__)) && (defined(__mc68000__) && !defined(__mcoldfire__) && !defined(__AHCC__))
static __inline __uint32_t __bswap_32_0(__uint32_t x) 0xe058; /* ror.w d0 */
static __inline __uint32_t __bswap_32_1(__uint32_t x) 0x4840; /* swap d0 */
static __inline __uint32_t __bswap_32_2(__uint32_t x) 0xe058; /* ror.w d0 */
#define __bswap_32(v) __bswap_32_2(__bswap_32_1(__bswap_32_0(v)))
#elif defined(__mc68000__) && !defined(__mcoldfire__) && defined(__AHCC__)
#define __bswap_32(x) __bswp__(x)
#elif defined __GNUC__ && !defined(__mcoldfire__)
static __inline __uint32_t
__bswap_32 (__uint32_t __bsx)
{
  if (__builtin_constant_p (__bsx))
    return __bswap_constant_32 (__bsx);
  __asm__ __volatile__ ("ror%.w %#8, %0;"
			"swap %0;"
			"ror%.w %#8, %0"
			: "+d" (__bsx));
  return __bsx;
}
#elif defined(__AHCC__) /* AHCC is not smart enough to emit that from object files */
#define __bswap_32(x) __bswap_constant_32(x)
#else
static __inline __uint32_t
__bswap_32 (__uint32_t __bsx)
{
  return __bswap_constant_32 (__bsx);
}
#endif

#if defined __GNUC__ && __GNUC__ >= 2
/* Swap bytes in 64 bit value.  */
# define __bswap_constant_64(x) \
  __extension__								      \
  ((((x) & 0xff00000000000000ull) >> 56)				      \
   | (((x) & 0x00ff000000000000ull) >> 40)				      \
   | (((x) & 0x0000ff0000000000ull) >> 24)				      \
   | (((x) & 0x000000ff00000000ull) >> 8)				      \
   | (((x) & 0x00000000ff000000ull) << 8)				      \
   | (((x) & 0x0000000000ff0000ull) << 24)				      \
   | (((x) & 0x000000000000ff00ull) << 40)				      \
   | (((x) & 0x00000000000000ffull) << 56))

/* Swap bytes in 64 bit value.  */
static __inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{
  if (__builtin_constant_p (__bsx))
    return __bswap_constant_64 (__bsx);
  return (__bswap_32 (__bsx >> 32)
	  | ((__uint64_t) __bswap_32 (__bsx) << 32));
}
#endif

#endif /* _BITS_BYTESWAP_H */
