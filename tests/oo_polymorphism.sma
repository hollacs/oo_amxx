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

		oo_ctor(cl, "Ctor");
		oo_ctor(cl, "Ctor2", @int{w}, @int{h});
		oo_mthd(cl, "Area");
	}

	oo_class("Rectangle", "Shape")
	{
		new cl[] = "Rectangle";
		oo_mthd(cl, "Area");
	}

	oo_class("Triangle", "Shape")
	{
		new cl[] = "Triangle";
		oo_mthd(cl, "Area");
	}
}

public Shape@Ctor() {}

public Shape@Ctor2(w, h)
{
	new this = oo_this();
	oo_set(this, "width", w);
	oo_set(this, "height", h);
}

public Shape@Area()
{
	server_print("Parent class area: 0");
	return 0;
}

public Rectangle@Area()
{
	new this = oo_this();
	new area = oo_get(this, "width") * oo_get(this, "height");
	server_print("Rectangle class area: %d", area);

	return area;
}

public Triangle@Area()
{
	new this = oo_this();
	new area = oo_get(this, "width") * oo_get(this, "height") / 2;
	server_print("Triangle class area: %d", area);

	return area;
}

public main()
{
	register_plugin("[OO] Polymorphism", "0.1", "holla");

	new Shape:shape = oo_new("Shape");
	oo_call(shape, "Area");

	new Rectangle:rec = oo_new("Rectangle", 10, 7);
	oo_call(rec, "Area");

	new Triangle:tri = oo_new("Triangle", 10, 5);
	oo_call(tri, "Area");

	oo_delete(shape);
	oo_delete(rec);
	oo_delete(tri);
}
