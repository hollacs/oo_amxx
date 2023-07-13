#include <amxmodx>
#include <oo>

#define main plugin_init

public oo_init()
{
	oo_class("Person")
	{
		new cl[] = "Person";

		oo_var(cl, "name", 32);
		oo_var(cl, "age", 1);
		oo_var(cl, "height", 1);

		oo_ctor(cl, "Ctor");
		oo_ctor(cl, "Ctor2", @str{name}, @int{age}, @fl{height});
		oo_dtor(cl, "Dtor");

		oo_mthd(cl, "Display");
	}
}

public Person@Ctor()
{
	server_print("object constructed");
}

public Person@Ctor2(const name[], age, Float:height)
{
	new this = oo_this();
	oo_set_str(this, "name", name);
	oo_set(this, "age", age);
	oo_set(this, "height", height);

	server_print("object constructed with parameters");
}

public Person@Dtor()
{
	new name[32];
	oo_get_str(oo_this(), "name", name, sizeof(name));
	server_print("object '%s' destructed", name);
}

public Person@Display()
{
	new this = oo_this();

	new name[32];
	oo_get_str(this, "name", name, sizeof(name));

	server_print("  Name: %s^n  Age: %d^n  Height: %.1f", 
		name, 
		oo_get(this, "age"), 
		Float:oo_get(this, "height"));

	server_print("----------");
}

public main()
{
	register_plugin("[OO] Class and Objects", "0.1", "holla");

	new Person:p1 = oo_new("Person");
	oo_set_str(p1, "name", "John");
	oo_set(p1, "age", 20);
	oo_set(p1, "height", 180.0);
	oo_call(p1, "Display");

	new Person:p2 = oo_new("Person", "Mary", 18, 165.0);
	oo_call(p2, "Display");

	oo_delete(p1);
	oo_delete(p2);
}