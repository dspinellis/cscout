/*
 * (C) Copyright 2002-2016 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef __cplusplus
#define __EXCEPTIONS
#endif

/*
 * Generic gcc workarounds.
 * Note that CScout has builtin support for __atribute__.
 */
#define __PRETTY_FUNCTION__ "UNKNOWN"
#define __builtin_bswap16(x) (unsigned short)(x)
#define __builtin_bswap32(x) (unsigned int)(x)
#define __builtin_bswap64(x) (unsigned long long)(x)
#define __builtin_constant_p(_x) ((_x), 0)
#define __builtin_expect(_x, _v) ((_v), (_x))
#define __builtin_fabs( _x) (double)(_x)
#define __builtin_fabsf(_x) (float)(_x)
#define __builtin_fabsl(_x) (long double)(_x)
#define __builtin_frame_address(_x) (_x, (void *)0)
#define __builtin_inf()     (double)0
#define __builtin_inff()    (float)0
#define __builtin_infl()    (long double)0
#define __builtin_memcpy(_d, _s, _n) (_s, _n, d)
#define __builtin_next_arg(_x) (_x, 0)
#define __builtin_offsetof(_t, _m) (((size_t) &((_t *)0)->_m))
#define __builtin_stdarg_start
#define __builtin_va_arg(_ap, _type) (*(_type *)(_ap))
#define __builtin_va_copy(_a, _b)
#define __builtin_va_end(_v)
#define __builtin_va_list void *
#define __builtin_va_start(_ap, _arg) ((void)(_ap),(void)(_arg))
#define __extension
#define __extension__
#define asm __asm__
#define typeof __typeof__
