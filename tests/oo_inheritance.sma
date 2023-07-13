#include <amxmodx>
#include <oo>

#define main plugin_init

public oo_init()
{
	oo_class("Shape")
	{
		new cl[] = "Shape";

		oo_var(cl, "width", 1);
		oo_var(cl, "height", 1);

		oo_mthd(cl, "SetWidth", @int{w});
		oo_mthd(cl, "SetHeight", @int{h});
	}

	oo_class("Rectangle", "Shape")
	{
		new cl[] = "Rectangle";
		oo_mthd(cl, "GetArea");
	}
}

public Shape@SetWidth(w)
{
	oo_set(oo_this(), "width", w);
}

public Shape@SetHeight(h)
{
	oo_set(oo_this(), "height", h);
}

public Rectangle@GetArea()
{
	new this = oo_this();
	return oo_get(this, "width") * oo_get(this, "height");
}

public main()
{
	register_plugin("[OO] Inheritance", "0.1", "holla");

	new Rectangle:rect = oo_new("Rectangle");
	oo_call(rect, "SetWidth", 5);
	oo_call(rect, "SetHeight", 7);

	server_print("Area of the rectangle is: %d", oo_call(rect, "GetArea"));

	oo_delete(rect);
}