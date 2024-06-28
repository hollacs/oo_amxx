# TEST
In the last commit version, now you can hook all the oo methods that was created by plugin
**Please note that this is still under testing and I'll release a new version when the testing is finished.**

Example: (we're going to hook some methods in [oo_animal.sma](https://github.com/hollacs/oo_amxx/blob/no-std/scripting/oo_animal.sma))
```sourcepawn
#include <amxmodx>
#include <oo>

public plugin_init()
{
    // Hook the Animal@Ctor() constructor
    oo_hook_ctor("Animal", "Ctor", "OnAnimalCtor");

    // Hook the Dog@MakeSound() as post
    oo_hook_mthd("Dog", "MakeSound", "OnDogMakeSound_Post", 1);

    // Hook the Snake@GetLegs() method
	oo_hook_mthd("Snake", "GetLegs", "OnSnakeGetLegs_1");
	oo_hook_mthd("Snake", "GetLegs", "OnSnakeGetLegs_2");

    // Hook the Snake@Test() method
    oo_hook_mthd("Snake", "Test", "OnSnakeTest");

    // Hook the Animal@Dtor() destructor
    oo_hook_dtor("Animal", "OnAnimalDtor");
}

public OnAnimalCtor(const name[], age)
{
	server_print("OnAnimalCtor(name=%s, age=%d)", name, age);
}

public OnDogMakeSound_Post(msg[], len)
{
	server_print("OnDogMakeSound_Post(msg=%s, len=%d)", msg, len);
}

public OnSnakeGetLegs_1()
{
	server_print("OnSnakeGetLegs_1()");
	oo_hook_set_return(369); // change the method actual return value
	return OO_CONTINUE;
}

public OnSnakeGetLegs_2()
{
	server_print("OnSnakeGetLegs_2() => oo_hook_get_return()=%d", 
		oo_hook_get_return()); // get current return value

	return OO_SUPERCEDE; // block the orignal method call
}

public OnSnakeTest(a, &b, const c[], d[], e[5])
{
	server_print("OnSnakeTest(%d, %d, %s, %s, {%d,%d,%d,%d,%d})", a, b, c ,d, e[0], e[1], e[2], e[3], e[4]);

    // we going to change the param(a) to 11
	oo_hook_set_param(1, OO_CELL, 11);

    // change b to 22 (OO_BYREF can be changed directly)
	b = 22;

    // change param(c) to "33"
	oo_hook_set_param(3, OO_STRING, "33");

    // change d to "44" (OO_STRING_REF can be changed directly)
	copy(d, 31, "44"); // string_ref can change directly

    // change e to {11, 22, 33, 44, 55} (OO_ARRAY can be changed directly)
	new arr[5] = {11, 22, 33, 44, 55};
	e = arr;
}

public OnAnimalDtor()
{
	server_print("OnAnimalDtor()");
}

```