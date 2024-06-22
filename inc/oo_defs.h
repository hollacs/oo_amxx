#ifndef OO_DEFS_H
#define OO_DEFS_H

#include <cstdint>
#include <amtl/am-cxx.h>

KE_CONSTEXPR int OO_VERSION = 110;

namespace oo
{
	KE_CONSTEXPR uint8_t IDENTIFIER_LEN	= 24u;

	KE_CONSTEXPR int8_t OO_CONTINUE = 0;
	KE_CONSTEXPR int8_t OO_SUPERCEDE = 1;
	KE_CONSTEXPR int8_t OO_BREAK = 2;

	KE_CONSTEXPR int8_t OO_STRING_EX    = -2;
	KE_CONSTEXPR int8_t OO_STRING       = -1;
	KE_CONSTEXPR int8_t OO_BYREF        = 0;
	KE_CONSTEXPR int8_t OO_FLOAT        = 1;
	KE_CONSTEXPR int8_t OO_CELL			= 1;
	KE_CONSTEXPR int8_t OO_VEC2			= 2;
	KE_CONSTEXPR int8_t OO_VEC3			= 3;
}

#endif // OO_DEFS_H