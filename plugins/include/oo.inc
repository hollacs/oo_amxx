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

#if defined _oo_included
	#endinput
#endif

#define _oo_included

#define OO_VERSION		1.0.0
#define OO_VERSION_NUM	100
stock const OO_VERSION_STR[] = "1.0.0";

#include <oo_macros>

#define @null any:0

forward oo_init();

// declare a class
native oo_decl_class(const class[], const base[] = "", version_no = OO_VERSION_NUM);

// declare a constructor for a class
native oo_decl_ctor(const class[], const name[], any:...);
native oo_decl_dtor(const class[], const name[]);
native oo_decl_msg(const class[], const name[], any:...);
native oo_decl_ivar(const class[], const name[], size);

native oo_isa(any:this, const _super[]);
native bool:oo_subclass_of(const sub[], const _super[]);
native bool:oo_class_exists(const class[]);
native oo_get_class_name(any:this, classname[], len);

native any:oo_new(const class[], any: ...);
native oo_delete(any:this);

native oo_send(any:this, const name[], any: ...);
native oo_read(any:this, const name[], any: ...);
native oo_write(any:this, const name[], any: ...);

native any:oo_this();

//native oo_this_ctor(any: ...);
native oo_super_ctor(const _super[], any: ...);