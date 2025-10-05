/*
 * (C) Copyright 2025 Diomidis Spinellis
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

// GCC workarounds that depend on defined macros, such as __aarch64__.
/*
 * Typedefs for fixed-width SIMD vector types.
 * Most are defined in terms of CScout's __simd
 * qualifier which allows array subscripting.
 * This cannot be done with a typedef like
 * typedef signed char __Int8x8_t[8];
 * because in C, a typedef name used as an array type isn't re-usable
 * as a plain type name — it always expands back into an array.
 */

// arm definitions start here.
#define __fp16 float
#define __bf16 float
#define _Float16 float

#define __simd64_int8_t __simd signed char
#define __simd64_int16_t __simd short
#define __simd64_int32_t __simd int
#define __builtin_neon_di __simd long long

#define __simd64_uint8_t __simd signed char
#define __simd64_uint16_t __simd unsigned short
#define __simd64_uint32_t __simd unsigned int
#define __builtin_neon_udi __simd long long

#define __simd64_float16_t __simd float
#define __simd64_float32_t __simd float
#define __simd128_float32_t __simd float

#define __simd64_poly8_t __simd unsigned char
#define __simd64_poly16_t __simd unsigned short

#define __simd128_int8_t __simd signed char
#define __simd128_int16_t __simd short
#define __simd128_int32_t __simd int
#define __simd128_int64_t __simd long long

#define __simd128_uint8_t __simd unsigned char
#define __simd128_uint16_t __simd unsigned short
#define __simd128_uint32_t __simd unsigned int
#define __simd128_uint64_t __simd unsigned long long

#define __simd128_poly8_t __simd unsigned char
#define __simd128_poly16_t  __simd unsigned short

#define __simd128_bfloat16_t __simd __bf16
#define __simd64_bfloat16_t __simd __bf16

#define __builtin_neon_poly8 unsigned char
#define __builtin_neon_poly16 unsigned short
#define __builtin_neon_poly64 unsigned long
#define __builtin_neon_poly128 unsigned long long

// 8-bit signed/unsigned scalar elements
#define __builtin_neon_qi   int8_t
#define __builtin_neon_uqi  uint8_t

// 16-bit signed/unsigned scalar elements
#define __builtin_neon_hi   int16_t
#define __builtin_neon_uhi  uint16_t

// 32-bit signed/unsigned scalar elements
#define __builtin_neon_si   int32_t
#define __builtin_neon_usi  uint32_t


// Floating-point scalar elements
#define __builtin_neon_sf   float      // 32-bit IEEE-754
#define __builtin_neon_df   double     // 64-bit IEEE-754

#define __builtin_neon_ti   __int128
#define __builtin_neon_uti  unsigned __int128

#define __builtin_neon_ei   int   /* immediate integer (enum-like) */
#define __builtin_neon_oi   int
#define __builtin_neon_ci   int
#define __builtin_neon_xi   __int128 // Actually 512
#define __builtin_neon_bf   __bf16   /* 16-bit bfloat */


// arm64 definitions start here.

// 64-bit “d” register width (8 bytes total)
#define __Int8x8_t __simd signed char
#define __Int16x4_t __simd short
#define __Int32x2_t __simd int
#define __Int64x1_t __simd long long

#define __Uint8x8_t __simd unsigned char
#define __Uint16x4_t __simd unsigned short
#define __Uint32x2_t __simd unsigned int
#define __Uint64x1_t __simd unsigned long long

#define __Float16x4_t __simd _Float16
#define __Float32x2_t __simd float
#define __Float64x1_t __simd double

#define __Poly8x8_t __simd unsigned char
#define __Poly16x4_t __simd unsigned short
#define __Poly64x1_t __simd unsigned long long

#define __Bfloat16x4_t __simd __bf16

// 128-bit “q” register width (16 bytes total)
#define __Int8x16_t __simd signed char
#define __Int16x8_t __simd short
#define __Int32x4_t __simd int
#define __Int64x2_t __simd long long

#define __Uint8x16_t __simd unsigned char
#define __Uint16x8_t __simd unsigned short
#define __Uint32x4_t __simd unsigned int
#define __Uint64x2_t __simd unsigned long long

#define __Float16x8_t __simd _Float16
#define __Float32x4_t __simd float
#define __Float64x2_t __simd double

#define __Poly8x16_t __simd unsigned char
#define __Poly16x8_t __simd unsigned short
#define __Poly64x2_t __simd unsigned long long

#define __Bfloat16x4_t __simd __bf16
#define __Bfloat16x8_t __simd __bf16

// Scalar polynomial and FP base types
#define __Poly8_t unsigned char
#define __Poly16_t unsigned short
#define __Poly64_t unsigned long long
#define __Poly128_t unsigned __int128

// Builtin SIMD element types
#define __builtin_aarch64_simd_qi signed char
#define __builtin_aarch64_simd_hi short
#define __builtin_aarch64_simd_si int
#define __builtin_aarch64_simd_di long long

#define __builtin_aarch64_simd_hf _Float16
#define __builtin_aarch64_simd_bf __bf16
#define __builtin_aarch64_simd_sf float
#define __builtin_aarch64_simd_df double

#define __builtin_aarch64_simd_poly8 unsigned char
#define __builtin_aarch64_simd_poly16 unsigned short
#define __builtin_aarch64_simd_poly128 unsigned __int128

// Builtin x2 types
// gcc/gcc/config/aarch64/aarch64-builtins.cc handle_arm_neon_h
// These are not compatible with the 32-bit header
// arm-linux-gnueabihf/14/include/arm_neon.h
// so use a conditional based on a Linux kernel
// definition.
#if defined(__aarch64__)
#define int8x8x2_t struct { int8x8_t __val[2]; }
#define int16x4x2_t struct { int16x4_t __val[2]; }
#define int32x2x2_t struct { int32x2_t __val[2]; }
#define int64x1x2_t struct { int64x1_t __val[2]; }
#define float16x4x2_t struct { float16x4_t __val[2]; }
#define float32x2x2_t struct { float32x2_t __val[2]; }
#define poly8x8x2_t struct { poly8x8_t __val[2]; }
#define poly16x4x2_t struct { poly16x4_t __val[2]; }
#define uint8x8x2_t struct { uint8x8_t __val[2]; }
#define uint16x4x2_t struct { uint16x4_t __val[2]; }
#define uint32x2x2_t struct { uint32x2_t __val[2]; }
#define float64x1x2_t struct { float64x1_t __val[2]; }
#define uint64x1x2_t struct { uint64x1_t __val[2]; }
#define int8x16x2_t struct { int8x16_t __val[2]; }
#define int16x8x2_t struct { int16x8_t __val[2]; }
#define int32x4x2_t struct { int32x4_t __val[2]; }
#define int64x2x2_t struct { int64x2_t __val[2]; }
#define float16x8x2_t struct { float16x8_t __val[2]; }
#define float32x4x2_t struct { float32x4_t __val[2]; }
#define float64x2x2_t struct { float64x2_t __val[2]; }
#define poly8x16x2_t struct { poly8x16_t __val[2]; }
#define poly16x8x2_t struct { poly16x8_t __val[2]; }
#define poly64x2x2_t struct { poly64x2_t __val[2]; }
#define poly64x1x2_t struct { poly64x1_t __val[2]; }
#define uint8x16x2_t struct { uint8x16_t __val[2]; }
#define uint16x8x2_t struct { uint16x8_t __val[2]; }
#define uint32x4x2_t struct { uint32x4_t __val[2]; }
#define uint64x2x2_t struct { uint64x2_t __val[2]; }

#define int8x8x3_t struct { int8x8_t __val[3]; }
#define int16x4x3_t struct { int16x4_t __val[3]; }
#define int32x2x3_t struct { int32x2_t __val[3]; }
#define int64x1x3_t struct { int64x1_t __val[3]; }
#define float16x4x3_t struct { float16x4_t __val[3]; }
#define float32x2x3_t struct { float32x2_t __val[3]; }
#define poly8x8x3_t struct { poly8x8_t __val[3]; }
#define poly16x4x3_t struct { poly16x4_t __val[3]; }
#define uint8x8x3_t struct { uint8x8_t __val[3]; }
#define uint16x4x3_t struct { uint16x4_t __val[3]; }
#define uint32x2x3_t struct { uint32x2_t __val[3]; }
#define float64x1x3_t struct { float64x1_t __val[3]; }
#define uint64x1x3_t struct { uint64x1_t __val[3]; }
#define int8x16x3_t struct { int8x16_t __val[3]; }
#define int16x8x3_t struct { int16x8_t __val[3]; }
#define int32x4x3_t struct { int32x4_t __val[3]; }
#define int64x2x3_t struct { int64x2_t __val[3]; }
#define float16x8x3_t struct { float16x8_t __val[3]; }
#define float32x4x3_t struct { float32x4_t __val[3]; }
#define float64x2x3_t struct { float64x2_t __val[3]; }
#define poly8x16x3_t struct { poly8x16_t __val[3]; }
#define poly16x8x3_t struct { poly16x8_t __val[3]; }
#define poly64x2x3_t struct { poly64x2_t __val[3]; }
#define poly64x1x3_t struct { poly64x1_t __val[3]; }
#define uint8x16x3_t struct { uint8x16_t __val[3]; }
#define uint16x8x3_t struct { uint16x8_t __val[3]; }
#define uint32x4x3_t struct { uint32x4_t __val[3]; }
#define uint64x2x3_t struct { uint64x2_t __val[3]; }

#define int8x8x4_t struct { int8x8_t __val[4]; }
#define int16x4x4_t struct { int16x4_t __val[4]; }
#define int32x2x4_t struct { int32x2_t __val[4]; }
#define int64x1x4_t struct { int64x1_t __val[4]; }
#define float16x4x4_t struct { float16x4_t __val[4]; }
#define float32x2x4_t struct { float32x2_t __val[4]; }
#define poly8x8x4_t struct { poly8x8_t __val[4]; }
#define poly16x4x4_t struct { poly16x4_t __val[4]; }
#define uint8x8x4_t struct { uint8x8_t __val[4]; }
#define uint16x4x4_t struct { uint16x4_t __val[4]; }
#define uint32x2x4_t struct { uint32x2_t __val[4]; }
#define float64x1x4_t struct { float64x1_t __val[4]; }
#define uint64x1x4_t struct { uint64x1_t __val[4]; }
#define int8x16x4_t struct { int8x16_t __val[4]; }
#define int16x8x4_t struct { int16x8_t __val[4]; }
#define int32x4x4_t struct { int32x4_t __val[4]; }
#define int64x2x4_t struct { int64x2_t __val[4]; }
#define float16x8x4_t struct { float16x8_t __val[4]; }
#define float32x4x4_t struct { float32x4_t __val[4]; }
#define float64x2x4_t struct { float64x2_t __val[4]; }
#define poly8x16x4_t struct { poly8x16_t __val[4]; }
#define poly16x8x4_t struct { poly16x8_t __val[4]; }
#define poly64x2x4_t struct { poly64x2_t __val[4]; }
#define poly64x1x4_t struct { poly64x1_t __val[4]; }
#define uint8x16x4_t struct { uint8x16_t __val[4]; }
#define uint16x8x4_t struct { uint16x8_t __val[4]; }
#define uint32x4x4_t struct { uint32x4_t __val[4]; }
#define uint64x2x4_t struct { uint64x2_t __val[4]; }


#define bfloat16x4x2_t struct { __Bfloat16x4_t __val[2]; }
#define bfloat16x8x2_t struct { __Bfloat16x8_t __val[2]; }
#define bfloat16x4x3_t struct { __Bfloat16x4_t __val[3]; }
#define bfloat16x8x3_t struct { __Bfloat16x8_t __val[3]; }
#define bfloat16x4x4_t struct { __Bfloat16x4_t __val[4]; }
#define bfloat16x8x4_t struct { __Bfloat16x8_t __val[4]; }
#endif // defined(__aarch64__)

