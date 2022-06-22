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


#include "oo_utils.h"

#include <algorithm>

#include <cctype>

#include "oo_defs.h"
#include "oo_manager.h"

namespace oo::utils
{
	constexpr int CHARBITS = (8 * sizeof(char));

	bool IsLegit(std::string_view identifier)
	{
		auto is_legit_char = [](auto &&c) { return c == '_' || std::isalnum(c); };
		return
			!std::isdigit(identifier[0])										&&
			std::all_of(identifier.begin(), identifier.end(), is_legit_char)	&&
			!identifier.empty() && identifier.length() <= IDENTIFIER_LEN;
	}

	bool IsLegitSize(int8_t size)
	{
		return size >= OO_STRING_EX;
	}
}