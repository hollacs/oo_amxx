// 
//		OO (Object-Oriention) support as a module enabled for AMX Mod X plugins.
//		Copyright (C) 2022  Hon Fai
// 
//		This program is free software: you can redistribute itand /or modify 
//		it under the terms of the GNU General Public License as published by 
//		the Free Software Foundation, either version 3 of the License, or 
//		(at your option) any later version.
// 
//		This program is distributed in the hope that it will be useful, 
//		but WITHOUT ANY WARRANTY; without even the implied warranty of 
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//		GNU General Public License for more details.
// 
//		You should have received a copy of the GNU General Public License 
//		along with this program. If not, see <https://www.gnu.org/licenses/>.
// 


#ifndef OO_DEFS_H
#define OO_DEFS_H


#include <cstdint>

namespace oo
{
	constexpr uint8_t IDENTIFIER_LEN	= 24u;

	constexpr int8_t OO_STRING_EX		= -2;
	constexpr int8_t OO_STRING			= -1;
	constexpr int8_t OO_BYREF			= 0;
	constexpr int8_t OO_FLOAT			= 1;
	constexpr int8_t OO_CELL			= 1;
	constexpr int8_t OO_VEC2			= 2;
	constexpr int8_t OO_VEC3			= 3;
}


#endif // OO_DEFS_H