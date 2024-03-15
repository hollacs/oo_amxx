#include <amxmodx>
#include <oo>

public oo_init()
{
	oo_class("B1");
	{
		oo_var("B1", "a", 1);
		oo_ctor("B1", "Ctor", @cell);
		oo_mthd("B1", "Print");
		oo_mthd("B1", "Method1");
	}

	oo_class("B2");
	{
		oo_var("B2", "b", 1);
		oo_ctor("B2", "Ctor", @cell);
		oo_mthd("B2", "Print");
		oo_mthd("B2", "Method2");
	}

	oo_class("D", "B1", "B2");
	{
		oo_var("D", "hp", 1);
		oo_ctor("D", "Ctor", @cell, @cell, @cell);
		oo_mthd("D", "Print");
	}
}

public plugin_init()
{
	new obj = oo_new("D", 100, 689, 777);
	oo_call(obj, "Print");
}

public B1@Ctor(a) { oo_set(oo_this(), "a", a); }

public B2@Ctor(b) { oo_set(oo_this(), "b", b); }

public D@Ctor(hp, a, b)
{
	oo_super_ctor("B1", a);
	oo_super_ctor("B2", b);
	oo_set(@this, "hp", hp);
}

public D@Print()
{
	oo_call(@this, "Method1");
	oo_call(@this, "Method2");

	oo_call(@this, "B1@Print");
	oo_call(@this, "B2@Print");
	server_print("D@Print(hp=%d, a=%d, b=%d)", 
		oo_get(@this, "hp"),
		oo_get(@this, "a"),
		oo_get(@this, "b"));
}

public B1@Method1() { server_print("B1@Method1()"); }
public B2@Method2() { server_print("B2@Method2()"); }

public B1@Print() { server_print("B1@Print()"); }
public B2@Print() { server_print("B2@Print()"); }