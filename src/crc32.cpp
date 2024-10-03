﻿/**
 * This file is part of Special K.
 *
 * Special K is free software : you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by The Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Special K is distributed in the hope that it will be useful,
 *
 * But WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Special K.
 *
 *   If not, see <http://www.gnu.org/licenses/>.
 *
**/

#include <SpecialK/stdafx.h>
#include <SpecialK/crc32.h>

extern "C"
uint32_t
SK_PUBLIC_API
SK_File_GetCRC32 (const wchar_t* wszFile, SK_HashProgressCallback_pfn callback)
{
  return SK_File_GetHash_32 (SK_CRC32_KAL, wszFile, callback);
}

extern "C"
uint32_t
SK_PUBLIC_API
SK_File_GetCRC32C (const wchar_t* wszFile, SK_HashProgressCallback_pfn callback)
{
  return SK_File_GetHash_32 (SK_CRC32C, wszFile, callback);
}




static uint32_t crc32_tab [] =
{
   0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
   0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
   0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
   0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
   0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
   0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
   0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
   0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
   0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
   0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
   0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
   0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
   0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
   0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
   0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
   0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
   0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
   0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
   0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
   0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
   0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
   0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
   0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
   0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
   0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
   0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
   0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
   0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
   0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
   0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
   0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
   0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
   0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
   0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
   0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
   0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
   0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
   0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
   0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
   0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
   0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
   0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
   0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
 };


// Initialize static member data
std::unique_ptr <InstructionSet::InstructionSet_Internal> InstructionSet::CPU_Rep;

extern "C"
uint32_t
__cdecl
crc32 (uint32_t crc, _Notnull_ const void *buf, size_t size)
{
  const auto *p =
       static_cast <const uint8_t *> (buf);

  crc = crc ^ ~0U;

  while (size--)
  {
    crc =
      crc32_tab [ (crc ^ *p++) & 0xFF ] ^ (crc >> 8);
  }

  return crc ^ ~0U;
}






/*
  Copyright (c) 2013 - 2014, 2016 Mark Adler, Robert Vazan, Max Vysokikh

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


#define POLY        0x82f63b78UL
#define LONG_SHIFT  8192UL
#define SHORT_SHIFT 256UL

using buffer = const uint8_t *;

static uint32_t table        [16][256];
static uint32_t long_shifts  [ 4][256];
static uint32_t short_shifts [ 4][256];

static bool _tableInitialized = false;

extern "C" void __cdecl calculate_table (void) ;

/* Table-driven software version as a fall-back.  This is about 15 times slower
   than using the hardware instructions.  This assumes little-endian integers,
   as is the case on Intel processors that the assembler code here is for. */
extern "C"
uint32_t
__cdecl
crc32c_append_sw (uint32_t crci, const void *input, size_t length)
{
  auto next =
    static_cast <buffer> (input);

#ifdef _M_X64
  uint64_t crc;
#else
  uint32_t crc;
#endif

  crc = crci ^ 0xffffffff;
#ifdef _M_X64
  while (length && ((uintptr_t)next & 7) != 0)
  {
    crc = table [0][(crc ^ *next++) & 0xff] ^ (crc >> 8);
    --length;
  }
  while (length >= 16)
  {
                   crc ^= *(uint64_t *)next;
    const uint64_t high = *(uint64_t *)(next + 8);

    crc = table[15][ crc         & 0xff]
        ^ table[14][(crc >> 8)   & 0xff]
        ^ table[13][(crc >> 16)  & 0xff]
        ^ table[12][(crc >> 24)  & 0xff]
        ^ table[11][(crc >> 32)  & 0xff]
        ^ table[10][(crc >> 40)  & 0xff]
        ^ table[ 9][(crc >> 48)  & 0xff]
        ^ table[ 8][ crc >> 56         ]
        ^ table[ 7][ high        & 0xff]
        ^ table[ 6][(high >> 8)  & 0xff]
        ^ table[ 5][(high >> 16) & 0xff]
        ^ table[ 4][(high >> 24) & 0xff]
        ^ table[ 3][(high >> 32) & 0xff]
        ^ table[ 2][(high >> 40) & 0xff]
        ^ table[ 1][(high >> 48) & 0xff]
        ^ table[ 0][ high >> 56        ];

    next   += 16;
    length -= 16;
  }
#else
  while (length && ((uintptr_t)next & 3) != 0)
  {
      crc = table[0][(crc ^ *next++) & 0xff] ^ (crc >> 8);
      --length;
  }
  while (length >= 12)
  {
              crc ^= *(uint32_t *)next;
    uint32_t high  = *(uint32_t *)(next + 4);
    uint32_t high2 = *(uint32_t *)(next + 8);

    crc = table[11][ crc          & 0xff]
        ^ table[10][(crc >>  8)   & 0xff]
        ^ table[ 9][(crc >> 16)   & 0xff]
        ^ table[ 8][ crc >> 24          ]
        ^ table[ 7][ high         & 0xff]
        ^ table[ 6][(high >>  8)  & 0xff]
        ^ table[ 5][(high >> 16)  & 0xff]
        ^ table[ 4][ high >> 24         ]
        ^ table[ 3][ high2        & 0xff]
        ^ table[ 2][(high2 >>  8) & 0xff]
        ^ table[ 1][(high2 >> 16) & 0xff]
        ^ table[ 0][ high2 >> 24        ];

    next   += 12;
    length -= 12;
  }
#endif
  while (length)
  {
    crc = table [0][(crc ^ *next++) & 0xff] ^ (crc >> 8);
    --length;
  }
  return (uint32_t)crc ^ 0xffffffff;
}

/* Apply the zeros operator table to crc. */
static
inline
uint32_t
shift_crc ( const uint32_t shift_table[][256], uint32_t crc )
{
  return shift_table [0][ crc        & 0xff]
       ^ shift_table [1][(crc >> 8)  & 0xff]
       ^ shift_table [2][(crc >> 16) & 0xff]
       ^ shift_table [3][ crc >> 24];
}

/* Comput e CRC-32C using the Intel hardware instruction. */
extern "C"
uint32_t
__cdecl
crc32c_append_hw (uint32_t crc, const void *buf, size_t len)
{
  if (buf == nullptr || len < 1)
    return crc;

  auto next =
    static_cast <buffer> (buf);

  ////MEMORY_BASIC_INFORMATION mi = { };
  ////
  ////size_t size =
  ////  VirtualQuery (buf, &mi, len);
  ////
  ////if ( size < len || (mi.Protect & PAGE_NOACCESS) ||
  ////                   (mi.State  != MEM_COMMIT)    ||
  ////                   (mi.AllocationProtect == PAGE_NOACCESS) )
  ////  return crc;

  buffer end = { };

#ifdef _M_X64
  /* need to be 64 bits for crc32q */
  uint64_t
#else
  uint32_t
#endif
  crc0, crc1, crc2;

  /* pre-process the crc */
  crc0 = crc ^ 0xffffffff;

  /* compute the crc for up to seven leading bytes to bring the data pointer
     to an eight-byte boundary */
  while (len && (reinterpret_cast <uintptr_t> (next) & 7) != 0)
  {
    crc0 = _mm_crc32_u8 (sk::narrow_cast <uint32_t> (crc0), *next);
    ++next;
    --len;
  }

#ifdef _M_X64
  /* compute the crc on sets of LONG_SHIFT*3 bytes, executing three independent crc
     instructions, each on LONG_SHIFT bytes -- this is optimized for the Nehalem,
     Westmere, Sandy Bridge, and Ivy Bridge architectures, which have a
     throughput of one crc per cycle, but a latency of three cycles */
  while (len >= 3 * LONG_SHIFT)
  {
    crc1 = 0;
    crc2 = 0;
    end  = next + LONG_SHIFT;

    do
    {
      crc0 = _mm_crc32_u64 (crc0, *reinterpret_cast <const uint64_t *>(next));
      crc1 = _mm_crc32_u64 (crc1, *reinterpret_cast <const uint64_t *>(next     + LONG_SHIFT));
      crc2 = _mm_crc32_u64 (crc2, *reinterpret_cast <const uint64_t *>(next + 2 * LONG_SHIFT));
      next += 8;
    } while (next < end);

    crc0 = shift_crc (long_shifts, sk::narrow_cast <uint32_t> (crc0)) ^ crc1;
    crc0 = shift_crc (long_shifts, sk::narrow_cast <uint32_t> (crc0)) ^ crc2;

    next += 2 * LONG_SHIFT;
    len  -= 3 * LONG_SHIFT;
  }

  /* do the same thing, but now on SHORT_SHIFT*3 blocks for the remaining data less
     than a LONG_SHIFT*3 block */
  while (len >= 3 * SHORT_SHIFT)
  {
    crc1 = 0;
    crc2 = 0;
    end  = next + SHORT_SHIFT;

    do
    {
      crc0 = _mm_crc32_u64 (crc0, *reinterpret_cast <const uint64_t *>(next));
      crc1 = _mm_crc32_u64 (crc1, *reinterpret_cast <const uint64_t *>(next     + SHORT_SHIFT));
      crc2 = _mm_crc32_u64 (crc2, *reinterpret_cast <const uint64_t *>(next + 2 * SHORT_SHIFT));
      next += 8;
    } while (next < end);

    crc0 = shift_crc (short_shifts, sk::narrow_cast <uint32_t> (crc0)) ^ crc1;
    crc0 = shift_crc (short_shifts, sk::narrow_cast <uint32_t> (crc0)) ^ crc2;

    next += 2 * SHORT_SHIFT;
    len  -= 3 * SHORT_SHIFT;
  }

  /* compute the crc on the remaining eight-byte units less than a SHORT_SHIFT*3
  block */
  end = next + (len - (len & 7));
  while (next < end)
  {
    crc0 = _mm_crc32_u64 (crc0, *reinterpret_cast <const uint64_t *> (next));
    next += 8;
  }
#else
  /* compute the crc on sets of LONG_SHIFT*3 bytes, executing three independent crc
  instructions, each on LONG_SHIFT bytes -- this is optimized for the Nehalem,
  Westmere, Sandy Bridge, and Ivy Bridge architectures, which have a
  throughput of one crc per cycle, but a latency of three cycles */
  while (len >= 3 * LONG_SHIFT)
  {
    crc1 = 0;
    crc2 = 0;
    end  = next + LONG_SHIFT;

    do
    {
      crc0 = _mm_crc32_u32 (crc0, *reinterpret_cast <const uint32_t *>(next));
      crc1 = _mm_crc32_u32 (crc1, *reinterpret_cast <const uint32_t *>(next     + LONG_SHIFT));
      crc2 = _mm_crc32_u32 (crc2, *reinterpret_cast <const uint32_t *>(next + 2 * LONG_SHIFT));
      next += 4;
    } while (next < end);

    crc0 = shift_crc (long_shifts, static_cast <uint32_t> (crc0)) ^ crc1;
    crc0 = shift_crc (long_shifts, static_cast <uint32_t> (crc0)) ^ crc2;

    next += 2 * LONG_SHIFT;
    len  -= 3 * LONG_SHIFT;
  }

  /* do the same thing, but now on SHORT_SHIFT*3 blocks for the remaining data less
  than a LONG_SHIFT*3 block */
  while (len >= 3 * SHORT_SHIFT)
  {
    crc1 = 0;
    crc2 = 0;
    end  = next + SHORT_SHIFT;

    do
    {
      crc0 = _mm_crc32_u32 (crc0, *reinterpret_cast <const uint32_t *>(next));
      crc1 = _mm_crc32_u32 (crc1, *reinterpret_cast <const uint32_t *>(next     + SHORT_SHIFT));
      crc2 = _mm_crc32_u32 (crc2, *reinterpret_cast <const uint32_t *>(next + 2 * SHORT_SHIFT));
      next += 4;
    } while (next < end);

    crc0 = shift_crc (short_shifts, static_cast <uint32_t> (crc0)) ^ crc1;
    crc0 = shift_crc (short_shifts, static_cast <uint32_t> (crc0)) ^ crc2;

    next += 2 * SHORT_SHIFT;
    len  -= 3 * SHORT_SHIFT;
  }

  /* compute the crc on the remaining eight-byte units less than a SHORT_SHIFT*3
  block */
  end = next + (len - (len & 7));
  while (next < end)
  {
    crc0 = _mm_crc32_u32(crc0, *reinterpret_cast<const uint32_t *>(next));
    next += 4;
  }
#endif
  len &= 7;

  /* compute the crc for up to seven trailing bytes */
  while (len)
  {
    crc0 = _mm_crc32_u8 (sk::narrow_cast <uint32_t> (crc0), *next);
    ++next;
    --len;
  }

  /* return a post-processed crc */
  return
    sk::narrow_cast <uint32_t> (crc0) ^ 0xffffffff;
}

extern "C"
int
__cdecl
crc32c_hw_available (void)
{
  int              info [4] = { 0 };
#ifndef SK_BUILT_BY_CLANG
  __cpuid         (info, 1);
#else
  __llvm_cpuid (1, info [0], info [1],
                   info [2], info [3]);
#endif
  return ( info [2] & (1 << 20)) != 0;
}

extern "C"
void
__cdecl
calculate_table (void)
{
  for (int i = 0; i < 256; i++)
  {
    auto res =
      sk::narrow_cast <uint32_t> (i);

    for (auto& t : table)
    {
      for (int k = 0; k < 8; k++)
        res = (res & 1) == 1 ? POLY ^ (res >> 1) :
                                      (res >> 1);
      t [i] = res;
    }
  }

  _tableInitialized = true;
}

extern "C"
void
__cdecl
calculate_table_hw (void)
{
  for (unsigned int i = 0; i < 256UL; i++)
  {
    auto res =
      sk::narrow_cast <uint32_t> (i);

    for (unsigned int k = 0; k < 8UL * (SHORT_SHIFT - 4UL); k++)
    {
      res = (res & 1UL) == 1UL ? POLY ^ (res >> 1UL) :
                                        (res >> 1UL);
    }

    for (unsigned int t = 0; t < 4UL; t++)
    {
      for (unsigned int k = 0; k < 8UL; k++)
      {
        res = (res & 1UL) == 1UL ? POLY ^ (res >> 1UL) :
                                          (res >> 1UL);
      }

      short_shifts [3 - t][i] = res;
    }

    for (unsigned int k = 0; k < 8UL * (LONG_SHIFT - 4UL - SHORT_SHIFT); k++)
    {
      res = (res & 1UL) == 1UL ? POLY ^ (res >> 1UL) :
                                        (res >> 1UL);
    }

    for (unsigned int t = 0; t < 4UL; t++)
    {
      for (unsigned int k = 0; k < 8UL; k++)
      {
        res = (res & 1UL) == 1UL ? POLY ^ (res >> 1UL) :
                                          (res >> 1UL);
      }

      long_shifts [3 - t][i] = res;
    }
  }
}

uint32_t (__cdecl *append_func)(uint32_t, const void*, size_t) = nullptr;


volatile LONG
  __crc32c_init = 0;

extern "C"
void __cdecl
__crc32_init (void)
{
  typedef
    uint32_t (__cdecl *appendfunc_pfn)(       uint32_t,
                                        const void      *,
                                              size_t      );
                       appendfunc_pfn pfnToSet = nullptr;

  if ( InterlockedCompareExchangeAcquire (&__crc32c_init, 1, 0)
                                                          == 0 )
  {
    InstructionSet::deferredInit ();

    if (crc32c_hw_available ())
    {
      //dll_log.Log (L"[ Checksum ] Using Hardware (SSE 4.2) CRC32C Algorithm");
      calculate_table_hw ();

      pfnToSet =
        crc32c_append_hw;
    }

    else
    {
      calculate_table ();

      //dll_log.Log (L"[ Checksum ] Using Software (Adler Optimized) CRC32C Algorithm");
      pfnToSet =
        crc32c_append_sw;
    }

    append_func =
      pfnToSet;

    InterlockedIncrementRelease (&__crc32c_init);
  }

  else
    SK_Thread_SpinUntilAtomicMin (&__crc32c_init, 2);
}

extern "C"
uint32_t __cdecl
crc32c (        uint32_t crc,
_Notnull_ const void    *input,
                size_t   length )
{
  if (append_func == nullptr)
  {
    static volatile LONG __init = 0;

    if (InterlockedCompareExchange (&__init, 1, 0) == 0)
    {
      __crc32_init ();

      InterlockedIncrement (&__init);
    }

    else
    {
      SK_Thread_SpinUntilAtomicMin (&__init, 2);
    }
  }

  if ( input       != nullptr &&
       length      >  0       &&
       append_func != nullptr )
  {
    return
      append_func (crc, input, length);
  }

  return crc;
}