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


#ifndef OO_UTILS_H
#define OO_UTILS_H

#include <string_view>

#include "sdk/amxxmodule.h"
#include "oo_manager.h"

namespace oo::utils
{
	bool IsLegit(std::string_view identifier);
	bool IsLegitSize(int8_t size);
	int AddMethod(AMX* amx, const char* callback, const ArgList* args = nullptr);
	cell ExecuteMethod(AMX* amx, cell* params, int8_t num_params, int32_t forward_id, ObjectHash this_hash, const ArgList* args = nullptr, int8_t start_param = 0);
}


#endif // OO_UTILS_H