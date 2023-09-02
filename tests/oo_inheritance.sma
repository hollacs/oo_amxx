#include <amxmodx>
#include <oo>

#define main plugin_init

public oo_init()
{
	oo_class("Animal")
	{
		new cl[] = "Animal";

		oo_var(cl, "weight", 1);
		oo_var(cl, "height", 1);

		oo_mthd(cl, "SetWeight", @int{w});
		oo_mthd(cl, "SetHeight", @int{h});
	}

	oo_class("Human", "Animal")
	{
		new cl[] = "Human";
		oo_mthd(cl, "GetBmi");
	}
}

public Animal@SetWeight(w)
{
	oo_set(oo_this(), "weight", w);
}

public Animal@SetHeight(h)
{
	oo_set(oo_this(), "height", h);
}

public Float:Human@GetBmi()
{
	new this = oo_this();
	new Float:height = oo_get(this, "height") / 100.0;
	return oo_get(this, "weight") / (height * height);
}

public main()
{
	register_plugin("[OO] Inheritance", "0.1", "holla");

	new Human:human = oo_new("Human");
	oo_call(human, "SetWeight", 60);
	oo_call(human, "SetHeight", 170);

	server_print("BMI of this human is: %.1f", oo_call(human, "GetBmi"));

	oo_delete(human);
}