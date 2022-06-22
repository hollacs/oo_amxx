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

enum Obj
{
	@null = 0
};

// class<"class identifier">@
#define class<%0>@					oo_decl_class(%0)

// sub_class<"sub" extends "super">@
#define sub_class<%0extends%1>@		oo_decl_class(%0, %1)

// ivar<"class">@Public/Protected/Private["name"]:OO_TYPE@
#define ivar<%0>@%1[%2]:%3@			oo_decl_ivar(%0, %2, %3)

// ctor<"class">@Public/Protected/Private."name"()@
#define ctor<%0>@%1.%2(%3)@			oo_decl_ctor(%0, %2, %3)

// void_ctor<"class">@Public/Protected/Private."name"()@
#define void_ctor<%0>@%1.%2()@		oo_decl_ctor(%0, %2)

// dtor<"class">@Public/Protected/Private."name"()@
#define dtor<%0>@%1.%2()@			oo_decl_dtor(%0, %2)

// void_dtor<"class">@Public/Protected/Private."name"()@
#define void_dtor<%0>@%1.%2()@		oo_decl_dtor(%0, %2)

// msg<"class">@Public/Protected/Private."name"(OO_TYPE, any: ...)@
#define msg<%0>@%1.%2(%3)@			oo_decl_msg(%0, %2, %3)

// void_msg<"class">@Public/Protected/Private."name"()@
#define void_msg<%0>@%1.%2()@		oo_decl_msg(%0, %2)

// isa<"class">(object) -> true/false
#define isa<%0>(%1)				oo_isa(%1, %0)

// subclass_of<"super">("sub")
#define subclass_of<%0>(%1)		oo_subclass_of(%1, %0)

// new<"class">(...ctor args) -> Obj: or @null
#define new<%0>(%1)				oo_new(%0, %1)
//#define new<%0>(%1)				{ __oo_set_next_class(%0); __oo_new(%1); }
//#define new(<%0> %1)				oo_new(%0 %1)

// alloc<"class">() -> Obj: or @null
// (Use this instead of new<>() when no argument involved)
#define alloc<%0>()				oo_new(%0)

// delete@(object)
#define delete@(%0)				oo_delete(%0)

// send@object->method(...args)
//#define send@%0->%1(%2)			oo_send(%0, "%1", %2)
#define send#%0.%1(%2)			oo_send(%0, %1, %2)

#define void_send#%0.%1()		oo_send(%0, %1)

// read@object["ivar"] -> _:									(for cells only)
#define read#%0[%1]								oo_read(%0, %1)

// read_a@object["ivar"][begin..end] >> array[begin..end]		(for arrays only)
#define read_a#%0[%1][%2..%3]>>{%4[%5..%6]}		oo_read(%0, %1, %2, %3, %4, %5, %6)

// read_sz@object["ivar"] >> string[]		(for zero-terminated strings only)
//#define read_sz@%0[%1]>>%2[%3]				oo_read(%0, %1, %2, %3)

// write@object["ivar"] << {value}
#define write#%0[%1]<<{%2}						oo_write(%0, %1, %2)

// write_a@object["ivar"][begin..end] << { value[begin..end] }
#define write_a#%0[%1][%2..%3]<<{%4[%5..%6]}	oo_write(%0, %1, %2, %3, %4, %5, %6)

// write_sz@object["ivar"] >> { string[] }		(for zero-terminated strings only)
//#define write_sz@%0[%1]<<{%2[]}					oo_write(%0, %1, %2)

// increment@object["ivar"]
#define increment#%0[%1]++		oo_write(%0, read@%0[%1] + 1)

// decrement@object["ivar"]
#define decrement#%0[%1]--		oo_write(%0, read@%0[%1] - 1)

// this -> Obj:
// (Use only in class methods.)
//#define this					oo_this()
//#define super					oo_super()

#define this_ctor(%0)			oo_this_ctor(%0)
#define super_ctor(%0)			oo_super_ctor(%0)


#define OO_CELL			(1)
#define OO_FLOAT		(1)
#define OO_BYREF		(-1)
//#define OO_STRING		(0)
#define OO_VEC2			(2)
#define OO_VEC3			(3)
#define OO_ARRAY[%0]	(%0)


forward oo_init();


native oo_decl_class(const class[], const base[] = "", version_no = OO_VERSION_NUM);
native oo_decl_ctor(const class[], const name[], any: ...);
native oo_decl_dtor(const class[], const name[]);
native oo_decl_msg(const class[], const name[], any: ...);
native oo_decl_ivar(const class[], const name[], size);

native oo_isa(Obj:_this, const _super[]);
native oo_subclass_of(const sub[], const _super[]);

native Obj:oo_new(const class[], any: ...);
native oo_delete(Obj:_this);

native oo_send(Obj:_this, const name[], any: ...);
native oo_read(Obj:_this, const name[], any: ...);
native oo_write(Obj:_this, const name[], any: ...);

native Obj:oo_this();

native oo_this_ctor(any: ...);
native oo_super_ctor(const _super[], any: ...);


// HACK for 0-arg new
/*static __s_next_class[24];

stock __oo_set_next_class(const next_class[])
{
	copy(__s_next_class, charsmax(__s_next_class), next_class);
}

#define __oo_new(%0)	{ oo_new(__s_next_class, %0); }*/