#include <amxmodx>
#include <oo>

public plugin_precache()
{
	oo_hook_ctor("Animal", "Ctor", "OnAnimalCtor");
	oo_hook_mthd("Dog", "MakeSound", "OnDogMakeSound_Pre");
	oo_hook_mthd("Dog", "MakeSound", "OnDogMakeSound_Post", 1);
	oo_hook_mthd("Snake", "GetLegs", "OnSnakeGetLegs_1");
	oo_hook_mthd("Snake", "GetLegs", "OnSnakeGetLegs_2");
	oo_hook_dtor("Animal", "OnAnimalDtor");
}

public plugin_init()
{
	register_plugin("[OO] Hook Tests", "0.1", "holla");
}

public OnAnimalCtor(const name[], age)
{
	server_print("OnAnimalCtor(name=%s, age=%d)", name, age);
}

public OnDogMakeSound_Pre(msg[], len)
{
	server_print("OnDogMakeSound_Pre(msg=%s, len=%d)", msg, len);
}

public OnDogMakeSound_Post(msg[], len)
{
	server_print("OnDogMakeSound_Post(msg=%s, len=%d)", msg, len);
}

public OnSnakeGetLegs_1()
{
	server_print("OnSnakeGetLegs_1()");
	oo_hook_set_return(369);
	return OO_CONTINUE;
}

public OnSnakeGetLegs_2()
{
	server_print("OnSnakeGetLegs_2() => oo_hook_get_return()=%d", 
		oo_hook_get_return());

	return OO_SUPERCEDE;
}

public OnAnimalDtor()
{
	server_print("OnAnimalDtor()");
}