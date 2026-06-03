#pragma once
#include <cstring>
#include <cassert>  //<assert.h>
#include <cstdio>  //<stdio.h>   //printf() std::fputs()
#include <cstdint>  //<stdint.h>   //uintmax_t, uint32_t
#include <string>
#include <vector>
#include <array>
#include <memory>  //std::shared_ptr

uint32_t NcgGetGamesaveChecksum2( uint32_t uBeginOffset2, uint32_t uEndOffset2, std::FILE* fp4 );
auto     NcgU32uToBytesBE2( uint32_t inp ) -> std::array<uint8_t,4>;
uint32_t NcgBytesBEToU32u2( const std::array<uint8_t,4>& bytes2 );
auto     NcgGetFileSizeLibc2( std::FILE* fp3, const char* flags2 ) -> std::pair<bool,uint32_t>;

const uint32_t NcgGSaveXeniaSize2              = 31744;
const uint32_t NcgGSaveXeniaDataStartOffs2     = 0;
const uint32_t NcgGSaveXeniaDataEndOffs2       = 30848;
const uint32_t NcgGSaveXeniaChecksumValueOffs2 = 30848;
//
const uint32_t NcgGSaveX360Size2              = 90112;
const uint32_t NcgGSaveX360DataStartOffs2     = 53248;  // 53248(d) = D000(h)
const uint32_t NcgGSaveX360DataEndOffs2       = 84096;  // 53248 + 30848 = 84096
const uint32_t NcgGSaveX360ChecksumValueOffs2 = 84096;  // 84096(d) = 14880(h)
//
static_assert( !(NcgGSaveXeniaChecksumValueOffs2 % 4), "" );
static_assert( !(NcgGSaveX360ChecksumValueOffs2 % 4), "" );
