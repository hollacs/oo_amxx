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


#include <amxmodx>
#include <oo>


public oo_init()
{
	class<"Animal">@
	{
		ivar<"Animal">@Public["m_nAge"]:OO_CELL@
		ivar<"Animal">@Public["m_LastEatFood"]:OO_ARRAY[32]@
		msg<"Animal">@Public."Eat"(OO_STRING)@
		msg<"Animal">@Public."Sleep"(OO_CELL)@
		msg<"Animal">@Public."Recycle"(OO_BYREF)@
	}
	
	sub_class<"Dog" extends "Animal">@
	{
		ctor<"Dog">@Public."Ctor_2arg"(OO_CELL, OO_STRING)@
		ivar<"Dog">@Public["m_nBarkCount"]:OO_CELL@
		msg<"Dog">@Public."Bark"(OO_VEC2)@
	}
	
	sub_class<"Cat" extends "Animal">@
	{
		ivar<"Cat">@Public["m_nMeowCount"]:OO_CELL@
		msg<"Cat">@Public."Meow"(OO_VEC3, OO_CELL)@
	}
	
	server_print("oo_subclass_of(^"Dog^", ^"Animal^"): %s", subclass_of<"Animal">("Dog") ? "true" : "false");
	
	new abc[4] = "abc";
	new Obj:dog = new<"Dog">(1, abc);
	server_print("abc is now: %s", abc);
	server_print("Obj:dog = %d", dog);
	
	send?dog."Eat"("banana");
	new times = 10;
	send?dog."Sleep"(5);
	new retval = send?dog."Recycle"(times);
	retval++;
	server_print("new times: %d", times);
	server_print("retval: %d", retval);
}

public Animal@Eat(const food[])
{
	write_a?this["m_LastEatFood"][0..32] << { food[0..strlen(food)] };
	server_print("Animal@Eat(const food[]: ^"%s^")", food);
	
	new member_LastEatFood[32];
	read_a?this["m_LastEatFood"][0..32] >> { member_LastEatFood[0..32] }
	server_print("Animal@m_LastEatFood is now: %s", member_LastEatFood);
}

public Animal@Sleep(days)
{
	server_print("Animal@Sleep(days: %d)", days);
}

public Animal@Recycle(&times)
{
	server_print("Animal@Recycle(++&times: %d)", ++times); return times;
}

public Dog@Ctor_2arg(arg1, arg2[])
{
	server_print("Dog@Ctor_2arg: ");
	server_print("arg1: %d", arg1);
	server_print("arg2: %s", arg2);
	
	new sz[5] = "def0";
	server_print("sz: %s", sz);
	Cao(sz);
	server_print("sz: %s", sz);
	
	arg2[2] = '9';
	
	/*new e = 5;
	new f = 0;
	e /= f;*/
}
public Cao(arg2[])
{
	arg2[3] = '1';
}
public Dog@Bark()		{}

public Cat@Meow()		{}