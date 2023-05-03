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

//#define method 	public
#define @null 	any:0
#define @this 	oo_this()
#define @increment(%0[%1]) 	oo_set(%0, oo_get(%0, %1) + 1)
#define @decrement(%0[%1]) 	oo_set(%0, oo_get(%0, %1) - 1)

#define OO_CELL			(1)		// cell
#define OO_FLOAT		(1)		// float
#define OO_BYREF		(0) 	// cell or float by reference
#define OO_STRING		(-1) 	// const string[]
#define OO_STRING_EX 	(-2) 	// string[]
#define OO_VEC2			(2)		// 2d vector (not necessary)
#define OO_VEC3			(3)		// 3d vector (not necessary)
#define OO_ARRAY[%0]	(%0)	// array[size]

#define @cell 			OO_CELL
#define @float 			OO_FLOAT
#define @byref 			OO_BYREF
#define @string 		OO_STRING
#define @stringex		OO_STRING_EX
#define @vec2			OO_VEC2
#define @vec3			OO_VEC3
#define @array[%0]		OO_ARRAY[%0]

#define @int{%0} 		@cell
#define @bool{%0}		@cell
#define @fl{%0}			@float
#define @ref{%0}		@byref
#define @str{%0}		@string
#define @stref{%0}		@stringex
#define @vec{%0[%1]}	@array[%1]
#define @arr{%0[%1]}	@array[%1]

//#define oo_get_cell(%0,%1)			oo_get(%0, %1)
#define oo_get_str(%0,%1,%2,%3) 	oo_get(%0, %1, 0, %3, %2, 0, %3)
#define oo_get_arr(%0,%1,%2)		oo_get(%0, %1, 0, 0, %2, 0, 0)

//#define oo_set_cell(%0,%1,%2) 	oo_set(%0, %1, %2)
#define oo_set_arr(%0,%1,%2)	oo_set(%0, %1, 0, 0, %2, 0, 0)
#define oo_set_str(%0,%1,%2) 	oo_set(%0, %1, 0, 0, %2, 0, 0)

forward oo_init();

native oo_class(const class[], const base[] = "", version_no = OO_VERSION_NUM);
native oo_ctor(const class[], const name[], any:...);
native oo_dtor(const class[], const name[]);
native oo_mthd(const class[], const name[], any:...);
native oo_smthd(const class[], const name[], any:...);
native oo_var(const class[], const name[], size);

native bool:oo_isa(any:this, const _class[], bool:superclass=false);
native bool:oo_subclass_of(const sub[], const _super[]);
native bool:oo_class_exists(const class[]);
native bool:oo_object_exists(any:this);
native oo_get_classname(any:this, output[], len);

native any:oo_new(const class[], any: ...);
native oo_delete(any:this);

native oo_call(any:this, const name[], any: ...);
native oo_get(any:this, const name[], any: ...);
native oo_set(any:this, const name[], any: ...);

native any:oo_this();

//native oo_this_ctor(any: ...);
native oo_super_ctor(const _super[], any: ...);