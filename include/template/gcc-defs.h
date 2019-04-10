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
 */

/*
 * Generic gcc workarounds.
 * Note that CScout has builtin support for __atribute__.
 */
#ifdef __cplusplus
#define __EXCEPTIONS
#endif

/* GCC __builtin_* funtions and misc */
#define asm __asm__
#define __alignof__(x) (sizeof(x) & 0xf)
#define __attribute__(x)
#define __builtin_add_overflow(x,y,z) ((x), (y), (z), 1)
#define __builtin_alloca_with_align_and_max (x,y,z) ((x), (y), (z), 1)
#define __builtin_alloca_with_align (x,y) ((x), (y), 1)
#define __builtin_alloca(x) ((x), 1)
#define __builtin_assume_aligned(x,y,...) ((x), (y), 1)
#define __builtin_bswap16(x) (x)
#define __builtin_bswap32(x) (x)
#define __builtin_bswap64(x) (x)
#define __builtin_call_with_static_chain(x,y) ((x), (y), 0)
#define __builtin_choose_expr(a,v,c) ((a) ? (v) : (c))
#define __builtin___clear_cache(x,y) ((x), (y))
#define __builtin_clrsbll(x) (x)
#define __builtin_clrsbl(x) (x)
#define __builtin_clrsb(x) (x)
#define __builtin_clz(z) (z)
#define __builtin_complex(x,y) ((x), (y))
#define __builtin_constant_p(_x) ((_x), 0)
#define __builtin_constant(x) ((x), 0)
#define __builtin_ctzll(x) ((x),1)
#define __builtin_ctzl(x) ((x),1)
#define __builtin_ctz(z) ((z),1)
#define __builtin_expect_with_probability(x,y) 1
#define __builtin_expect(_x, _v) ((_v), (_x))
#define __builtin_extend_pointer(x) ((x), 1) 
#define __builtin_fabsf(_x) (float)(_x)
#define __builtin_fabsl(_x) (long double)(_x)
#define __builtin_fabs( _x) (double)(_x)
#define __builtin_ffsll(x) (x)
#define __builtin_ffsl(x) (x)
#define __builtin_ffs(x) (x)
#define __builtin_FILE() 1
#define __builtin_fpclassify(x,y,z,w,r, ...)  ((x), (y), (z), (w), (r), 1)
#define __builtin___fprintf_chk(x,y,z,...) ((x), (y), (z), 1)
#define __builtin_frame_address(_x) (_x, (void *)0)
#define __builtin_FUNCTION() 1
#define __builtin_goacc_parlevel_id(x) (x)
#define __builtin_goacc_parlevel_size(x) (x)
#define __builtin_huge_val() 1
#define __builtin_huge_valf() 1
#define __builtin_huge_valfn() 1
#define __builtin_huge_valfnx() 1
#define __builtin_huge_vall() 1
#define __builtin_infd32() 1
#define __builtin_infd64() 1
#define __builtin_inf()     (double)0
#define __builtin_inff()    (float)0
#define __builtin_inffnx() 1
#define __builtin_infl()    (long double)0
#define __builtin_isinf_sign(...) 1
#define __builtin_LINE() 1
#define __builtin___memcpy_chk(x,y,z,w) ((x), (y), (z), (w), 0)
#define __builtin_memcpy(_d, _s, _n) (_s, _n, _d)
#define __builtin___memmove_chk(x,y,z,w) ((x), (y), (z), (w), 1)
#define __builtin___mempcpy_chk(x,y,z,w) ((x), (y), (z), (w), 0)
#define __builtin_memset(x,y,z) ((x), (y), (z), 1)
#define __builtin___memset_chk(x,y,z,w) ((x), (y), (z), (w), 0)
#define __builtin_mul_overflow(x,y,z) ((x), (y), (z), 1)
#define __builtin_nand128(x) ((x), 1.)
#define __builtin_nand32(x) ((x), 1f)
#define __builtin_nand64(x) ((x), 1.)
#define __builtin_nanfn(x) ((x), 1.)
#define __builtin_nanfnx(x) ((x), 1.)
#define __builtin_nanf(x)  ((x), 1.)
#define __builtin_nanl(x) ((x), 1)
#define __builtin_nansfn(x) ((x), 1f)
#define __builtin_nansfnx(x) ((x), 1)
#define __builtin_nansl(x) ((x), 1)
#define __builtin_nans(x) ((x), 1)
#define __builtin_nan(x) ((x), 1)
#define __builtin_next_arg(_x) (_x, 0)
#define __builtin_object_size(x,y)  ((x), (y), 1)
#define __builtin_offsetof(_t, _m) (((size_t) &((_t *)0)->_m))
#define __builtin_parityll(x) ((x), 1ll)
#define __builtin_parityl(x) ((x), 1l)
#define __builtin_parity(x) ((x), 1)
#define __builtin_popcountll(x) ((x), 1ll)
#define __builtin_popcountl(x) ((x), 1l)
#define __builtin_popcount(x) ((x), 1)
#define __builtin_powi(x) ((x), 1)
#define __builtin_powl(x) ((x), 1l)
#define __builtin_prefetch(x,...) ((x), 0)
#define __builtin___printf_chk(x,y,...) ((x), (y), 1)
#define __builtin_return_address(x) ((x), 0)
#define __builtin___snprintf_chk(x,y,z,w)  ((x), (y), (z), (w), 1)
#define __builtin_speculation_safe_value(x,y) ((x), (y), 1)
#define __builtin___sprintf_chk(x,y,z,w,...) ((x), (y), (z), (w), 1)
#define __builtin_stdarg_start
#define __builtin___stpcpy_chk(x,y,z) ((x), (y), (z), 0)
#define __builtin___strcat_chk(x,y,z) ((x), (y), (z), 0)
#define __builtin_strchr(x,y) ((x), (y), 0)
#define __builtin_strcmp(x,y) ((x), (y), 0)
#define __builtin___strcpy_chk(x,y,z) ((x), (y), (z), 0)
#define __builtin_strcspn(x,y) ((x), (y), 0)
#define __builtin_strlen(x) ((x), 1)
#define __builtin___strncat_chk(x,y,z,w) ((x), (y), (z), (w), 1)
#define __builtin___strncpy_chk(x,y,z,w) ((x), (y), (z), (w), 1)
#define __builtin_tgmath(x, y) ((x), (y), 0)
#define __builtin_trap() 1
#define __builtin_types_compatible_p(x,y) (1)
#define __builtin_unreachable() 1
#define __builtin_va_arg(_ap, _type) (*(_type *)(_ap))
#define __builtin_va_arg_pack() 1
#define __builtin_va_arg_pack_len() 1
#define __builtin_va_copy(_a, _b) ((_a), (_b), 0)
#define __builtin_va_end(_v) ((void)(_v))
#define __builtin_va_list void *
#define __builtin_va_start(_ap, _arg) ((void)(_ap),(void)(_arg))
#define __builtin___vprintf_chk(x,y,z) ((x), (y), (z), 1)
#define __builtin___vsnprintf_chk(x,y,z,w,r) ((x), (y), (z), (w), (r), 1)
#define __builtin___vsprintf_chk(x,y,z,w,r) ((x), (y), (z), (w), (r), 1)
#define __COUNTER__ 0
#define __extension
#define __extension__
#define __int128 long
#define __float128 double
#define __PRETTY_FUNCTION__ "UNKNOWN"
#define typeof __typeof__
