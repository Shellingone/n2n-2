/**
 * (C) 2007-20 - ntop.org and contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not see see <http://www.gnu.org/licenses/>
 *
 */

// taken from https://github.com/Logan007/pearson

// This is free and unencumbered software released into the public domain.

#include <stddef.h>
#include <stdint.h>

#include "pearson.h"


// table as in original paper "Fast Hashing of Variable-Length Text Strings" by Peter K. Pearson
// as published in The Communications of the ACM  Vol.33, No.  6 (June 1990), pp. 677-680.
static const uint8_t t[256] ={
  0x01, 0x57, 0x31, 0x0c, 0xb0, 0xb2, 0x66, 0xa6, 0x79, 0xc1, 0x06, 0x54, 0xf9, 0xe6, 0x2c, 0xa3,
  0x0e, 0xc5, 0xd5, 0xb5, 0xa1, 0x55, 0xda, 0x50, 0x40, 0xef, 0x18, 0xe2, 0xec, 0x8e, 0x26, 0xc8,
  0x6e, 0xb1, 0x68, 0x67, 0x8d, 0xfd, 0xff, 0x32, 0x4d, 0x65, 0x51, 0x12, 0x2d, 0x60, 0x1f, 0xde,
  0x19, 0x6b, 0xbe, 0x46, 0x56, 0xed, 0xf0, 0x22, 0x48, 0xf2, 0x14, 0xd6, 0xf4, 0xe3, 0x95, 0xeb,
  0x61, 0xea, 0x39, 0x16, 0x3c, 0xfa, 0x52, 0xaf, 0xd0, 0x05, 0x7f, 0xc7, 0x6f, 0x3e, 0x87, 0xf8,
  0xae, 0xa9, 0xd3, 0x3a, 0x42, 0x9a, 0x6a, 0xc3, 0xf5, 0xab, 0x11, 0xbb, 0xb6, 0xb3, 0x00, 0xf3,
  0x84, 0x38, 0x94, 0x4b, 0x80, 0x85, 0x9e, 0x64, 0x82, 0x7e, 0x5b, 0x0d, 0x99, 0xf6, 0xd8, 0xdb,
  0x77, 0x44, 0xdf, 0x4e, 0x53, 0x58, 0xc9, 0x63, 0x7a, 0x0b, 0x5c, 0x20, 0x88, 0x72, 0x34, 0x0a,
  0x8a, 0x1e, 0x30, 0xb7, 0x9c, 0x23, 0x3d, 0x1a, 0x8f, 0x4a, 0xfb, 0x5e, 0x81, 0xa2, 0x3f, 0x98,
  0xaa, 0x07, 0x73, 0xa7, 0xf1, 0xce, 0x03, 0x96, 0x37, 0x3b, 0x97, 0xdc, 0x5a, 0x35, 0x17, 0x83,
  0x7d, 0xad, 0x0f, 0xee, 0x4f, 0x5f, 0x59, 0x10, 0x69, 0x89, 0xe1, 0xe0, 0xd9, 0xa0, 0x25, 0x7b,
  0x76, 0x49, 0x02, 0x9d, 0x2e, 0x74, 0x09, 0x91, 0x86, 0xe4, 0xcf, 0xd4, 0xca, 0xd7, 0x45, 0xe5,
  0x1b, 0xbc, 0x43, 0x7c, 0xa8, 0xfc, 0x2a, 0x04, 0x1d, 0x6c, 0x15, 0xf7, 0x13, 0xcd, 0x27, 0xcb,
  0xe9, 0x28, 0xba, 0x93, 0xc6, 0xc0, 0x9b, 0x21, 0xa4, 0xbf, 0x62, 0xcc, 0xa5, 0xb4, 0x75, 0x4c,
  0x8c, 0x24, 0xd2, 0xac, 0x29, 0x36, 0x9f, 0x08, 0xb9, 0xe8, 0x71, 0xc4, 0xe7, 0x2f, 0x92, 0x78,
  0x33, 0x41, 0x1c, 0x90, 0xfe, 0xdd, 0x5d, 0xbd, 0xc2, 0x8b, 0x70, 0x2b, 0x47, 0x6d, 0xb8, 0xd1 };

/*
// alternative table as used in RFC 3074 and NOT as in original paper
static const uint8_t t[256] ={
0xfb, 0xaf, 0x77, 0xd7, 0x51, 0x0e, 0x4f, 0xbf, 0x67, 0x31, 0xb5, 0x8f, 0xba, 0x9d, 0x00, 0xe8,
0x1f, 0x20, 0x37, 0x3c, 0x98, 0x3a, 0x11, 0xed, 0xae, 0x46, 0xa0, 0x90, 0xdc, 0x5a, 0x39, 0xdf,
0x3b, 0x03, 0x12, 0x8c, 0x6f, 0xa6, 0xcb, 0xc4, 0x86, 0xf3, 0x7c, 0x5f, 0xde, 0xb3, 0xc5, 0x41,
0xb4, 0x30, 0x24, 0x0f, 0x6b, 0x2e, 0xe9, 0x82, 0xa5, 0x1e, 0x7b, 0xa1, 0xd1, 0x17, 0x61, 0x10,
0x28, 0x5b, 0xdb, 0x3d, 0x64, 0x0a, 0xd2, 0x6d, 0xfa, 0x7f, 0x16, 0x8a, 0x1d, 0x6c, 0xf4, 0x43,
0xcf, 0x09, 0xb2, 0xcc, 0x4a, 0x62, 0x7e, 0xf9, 0xa7, 0x74, 0x22, 0x4d, 0xc1, 0xc8, 0x79, 0x05,
0x14, 0x71, 0x47, 0x23, 0x80, 0x0d, 0xb6, 0x5e, 0x19, 0xe2, 0xe3, 0xc7, 0x4b, 0x1b, 0x29, 0xf5,
0xe6, 0xe0, 0x2b, 0xe1, 0xb1, 0x1a, 0x9b, 0x96, 0xd4, 0x8e, 0xda, 0x73, 0xf1, 0x49, 0x58, 0x69,
0x27, 0x72, 0x3e, 0xff, 0xc0, 0xc9, 0x91, 0xd6, 0xa8, 0x9e, 0xdd, 0x94, 0x9a, 0x7a, 0x0c, 0x54,
0x52, 0xa3, 0x2c, 0x8b, 0xe4, 0xec, 0xcd, 0xf2, 0xd9, 0x0b, 0xbb, 0x92, 0x9f, 0x40, 0x56, 0xef,
0xc3, 0x2a, 0x6a, 0xc6, 0x76, 0x70, 0xb8, 0xac, 0x57, 0x02, 0xad, 0x75, 0xb0, 0xe5, 0xf7, 0xfd,
0x89, 0xb9, 0x63, 0xa4, 0x66, 0x93, 0x2d, 0x42, 0xe7, 0x34, 0x8d, 0xd3, 0xc2, 0xce, 0xf6, 0xee,
0x38, 0x6e, 0x4e, 0xf8, 0x3f, 0xf0, 0xbd, 0x5d, 0x5c, 0x33, 0x35, 0xb7, 0x13, 0xab, 0x48, 0x32,
0x21, 0x68, 0x65, 0x45, 0x08, 0xfc, 0x53, 0x78, 0x4c, 0x87, 0x55, 0x36, 0xca, 0x7d, 0xbc, 0xd5,
0x60, 0xeb, 0x88, 0xd0, 0xa2, 0x81, 0xbe, 0x84, 0x9c, 0x26, 0x2f, 0x01, 0x07, 0xfe, 0x18, 0x04,
0xd8, 0x83, 0x59, 0x15, 0x1c, 0x85, 0x25, 0x99, 0x95, 0x50, 0xaa, 0x44, 0x06, 0xa9, 0xea, 0x97 }; */


#define ROR64(x,r) (((x)>>(r))|((x)<<(64-(r))))


void pearson_hash_256 (uint8_t *out, const uint8_t *in, size_t len) {

  /* initial values -  astonishingly, assembling using SHIFTs and ORs (in register)
   * works faster on well pipelined CPUs than loading the 64-bit value from memory.
   * however, there is one advantage to loading from memory: as we also store back to
   * memory at the end, we do not need to care about endianess! */
  uint8_t upper[8] = { 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08 };
  uint8_t lower[8] = { 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };

  uint64_t *upper_hash_mask_ptr = (uint64_t*)&upper;
  uint64_t *lower_hash_mask_ptr = (uint64_t*)&lower;
  uint64_t upper_hash_mask = *upper_hash_mask_ptr;
  uint64_t lower_hash_mask = *lower_hash_mask_ptr;
  uint64_t high_upper_hash_mask = upper_hash_mask + 0x1010101010101010;
  uint64_t high_lower_hash_mask = lower_hash_mask + 0x1010101010101010;

  uint64_t upper_hash = 0;
  uint64_t lower_hash = 0;
  uint64_t high_upper_hash = 0;
  uint64_t high_lower_hash = 0;
  size_t i;

  for (i = 0; i < len; i++) {
    // broadcast the character, xor into hash, make them different permutations
    uint64_t c = (uint8_t)in[i];
    c |= c <<  8;
    c |= c << 16;
    c |= c << 32;
    upper_hash ^= c ^ upper_hash_mask;
    lower_hash ^= c ^ lower_hash_mask;
    high_upper_hash ^= c ^ high_upper_hash_mask;
    high_lower_hash ^= c ^ high_lower_hash_mask;

    // table lookup
    uint8_t x;
    uint64_t h = 0;
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    upper_hash = h;

    h = 0;
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    lower_hash = h;

    h = 0;
    x = high_upper_hash; x = t[x]; high_upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_upper_hash; x = t[x]; high_upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_upper_hash; x = t[x]; high_upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_upper_hash; x = t[x]; high_upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_upper_hash; x = t[x]; high_upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_upper_hash; x = t[x]; high_upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_upper_hash; x = t[x]; high_upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_upper_hash; x = t[x]; high_upper_hash >>= 8; h |= x; h=ROR64(h,8);
    high_upper_hash = h;

    h = 0;
    x = high_lower_hash; x = t[x]; high_lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_lower_hash; x = t[x]; high_lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_lower_hash; x = t[x]; high_lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_lower_hash; x = t[x]; high_lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_lower_hash; x = t[x]; high_lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_lower_hash; x = t[x]; high_lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_lower_hash; x = t[x]; high_lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = high_lower_hash; x = t[x]; high_lower_hash >>= 8; h |= x; h=ROR64(h,8);
    high_lower_hash = h;
  }
  // store output
  uint64_t *o;
  o = (uint64_t*)&out[0];
  *o = high_upper_hash;
  o = (uint64_t*)&out[8];
  *o = high_lower_hash;
  o = (uint64_t*)&out[16];
  *o = upper_hash;
  o = (uint64_t*)&out[24];
  *o = lower_hash;
}


void pearson_hash_128 (uint8_t *out, const uint8_t *in, size_t len) {

  /* initial values -  astonishingly, assembling using SHIFTs and ORs (in register)
   * works faster on well pipelined CPUs than loading the 64-bit value from memory.
   * however, there is one advantage to loading from memory: as we also store back to
   * memory at the end, we do not need to care about endianess! */
  uint8_t upper[8] = { 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08 };
  uint8_t lower[8] = { 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };

  uint64_t *upper_hash_mask_ptr = (uint64_t*)&upper;
  uint64_t *lower_hash_mask_ptr = (uint64_t*)&lower;
  uint64_t upper_hash_mask = *upper_hash_mask_ptr;
  uint64_t lower_hash_mask = *lower_hash_mask_ptr;

  uint64_t upper_hash = 0;
  uint64_t lower_hash = 0;
  size_t i;

  for (i = 0; i < len; i++) {
    // broadcast the character, xor into hash, make them different permutations
    uint64_t c = (uint8_t)in[i];
    c |= c <<  8;
    c |= c << 16;
    c |= c << 32;
    upper_hash ^= c ^ upper_hash_mask;
    lower_hash ^= c ^ lower_hash_mask;
    // table lookup
    uint8_t x;
    uint64_t h = 0;
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    x = upper_hash; x = t[x]; upper_hash >>= 8; h |= x; h=ROR64(h,8);
    upper_hash = h;

    h = 0;
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    x = lower_hash; x = t[x]; lower_hash >>= 8; h |= x; h=ROR64(h,8);
    lower_hash = h;
  }
  // store output
  uint64_t *o;
  o = (uint64_t*)&out[0];
  *o = upper_hash;
  o = (uint64_t*)&out[8];
  *o = lower_hash;
}
