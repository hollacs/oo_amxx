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

#ifndef OO_NATIVES_H
#define OO_NATIVES_H

#include "amxxmodule.h"

namespace oo::natives
{
	// native oo_decl_class(const class[], const base[] = "", const version_no = OO_VERSION_NUM);
	// e.g.
	// oo_decl_class("Base",	"");
	// oo_decl_class("Derived",	"Base");
	cell AMX_NATIVE_CALL native_decl_class(AMX *amx, cell *params);


	// native oo_decl_ctor(const class[], const name[], any: ...);
	// e.g.
	// oo_decl_ctor("Derived",	"Ctor_0arg");
	// oo_decl_ctor("Base",		"Ctor_4arg", OO_CELL, OO_VECTOR, OO_ARRAY[5], OO_STRING);
	cell AMX_NATIVE_CALL native_decl_ctor(AMX *amx, cell *params);


	// native oo_decl_dtor(const class[], const name[], any: ...);
	// e.g.
	// oo_decl_dtor("Base",	"Dtor");
	cell AMX_NATIVE_CALL native_decl_dtor(AMX *amx, cell *params);


	// native oo_decl_msg(const class[], const name[], any: ...);
	// e.g.
	// oo_decl_msg("Player", "EmitSound", OO_CELL, OO_VECTOR, OO_FLOAT);
	cell AMX_NATIVE_CALL native_decl_msg(AMX *amx, cell *params);

	cell AMX_NATIVE_CALL native_decl_static_msg(AMX* amx, cell *params);


	// native oo_decl_ivar(const class[], const name[], size);
	// e.g.
	// oo_decl_ivar("Player", "m_Items", OO_ARRAY[4]);
	cell AMX_NATIVE_CALL native_decl_ivar(AMX *amx, cell *params);


	// native oo_isa(Obj:this, const super[]);
	// e.g.
	// oo_isa(derived, "Base");
	cell AMX_NATIVE_CALL native_isa(AMX *amx, cell *params);


	// native oo_subclass_of(const sub[], const super[]);
	// e.g.
	// oo_subclass_of("Derived", "Base");
	cell AMX_NATIVE_CALL native_subclass_of(AMX *amx, cell *params);


	// native Obj:oo_new(const class[], any: ...);
	// e.g.
	// oo_new("Player", "player name", 100, 100.0);
	cell AMX_NATIVE_CALL native_new(AMX *amx, cell *params);


	// native oo_delete(Obj:this);
	// e.g.
	// oo_delete(oo_new("Player"));
	cell AMX_NATIVE_CALL native_delete(AMX *amx, cell *params);


	// native oo_send(Obj:this, const name[], any: ...);
	// e.g.
	// oo_send(player, "BaseEntity@EmitSound", 100, origin, 1.0);
	// oo_send(player, "EmitSound", 100, origin, 1.0);
	cell AMX_NATIVE_CALL native_send(AMX *amx, cell *params);


	// native oo_read(Obj:this, const name[], any: ...);
	// e.g.
	// health = oo_read(player, "m_nHealth");
	// oo_read(player, "BaseEntity@m_nHealth", health);
	// oo_read(player, "m_nHealth", health);
	// oo_read(player, "m_Items", 0, 3, items, 0, 3);
	cell AMX_NATIVE_CALL native_read(AMX *amx, cell *params);
	//cell AMX_NATIVE_CALL native_read_str(AMX *amx, cell *params);

	// native oo_write(Obj:this, const name[], any: ...);
	// e.g.
	// oo_write(player, "BaseEntity@m_nHealth", 100);
	// oo_write(player, "m_nHealth", 100);
	// oo_write(player, "m_Items", 0, 3, items, 0, 3);
	cell AMX_NATIVE_CALL native_write(AMX *amx, cell *params);
	//cell AMX_NATIVE_CALL native_write_str(AMX *amx, cell *params);

	// native oo_this();
	// e.g.
	// oo_send(oo_this(), "EmitSound");
	cell AMX_NATIVE_CALL native_this(AMX *amx, cell *params);


	// native oo_this_ctor(any: ...);
	// e.g.
	// oo_this_ctor(100, origin, 1.0);
	cell AMX_NATIVE_CALL native_this_ctor(AMX *amx, cell *params);


	// native oo_super_ctor(const super[], any: ...);
	// e.g.
	// oo_super_ctor("BaseEntity", 100, origin, 1.0);
	cell AMX_NATIVE_CALL native_super_ctor(AMX *amx, cell *params);

	cell AMX_NATIVE_CALL native_class_exists(AMX* amx, cell *params);
	cell AMX_NATIVE_CALL native_object_exists(AMX* amx, cell *params);
	cell AMX_NATIVE_CALL native_method_exists(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL native_var_exists(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL native_get_class_name(AMX* amx, cell *params);
}


#endif // OO_NATIVES_H