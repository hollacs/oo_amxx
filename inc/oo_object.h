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


#ifndef OO_OBJECT_H
#define OO_OBJECT_H


#include "amxxmodule.h"

#include <memory>

#include <unordered_map>
#include <vector>

#include <cassert>

namespace oo
{
	using IVar	= std::vector<cell>;


	struct Class;

	struct Object
	{
		std::weak_ptr<Class>					isa;
		std::unordered_map<std::string, IVar>	ivars;
	};
}


#endif // OO_OBJECT_H