#include <amxmodx>
#include <oo>

public oo_init()
{
	// Declare a class
	oo_class("PlayerClassInfo")
	{
		new cl[] = "PlayerClassInfo";
		
		// Declare variables
		oo_var(cl, "name", 32); // string
		oo_var(cl, "model", 32); // string
		oo_var(cl, "health", 1); // integer
		oo_var(cl, "gravity", 1); // float

		// Declare constructor
		oo_ctor(cl, "Ctor", @str{name}, @str{model}, @int{health}, @fl{gravity});

		// Declare destructor
		oo_dtor(cl, "Dtor");
	}

	oo_class("PlayerClass")
	{
		new cl[] = "PlayerClass";
		oo_var(cl, "player", 1);

		oo_ctor(cl, "Ctor", @int{player});
		oo_dtor(cl, "Dtor");

		oo_mthd(cl, "GetInfo"); // Get PlayerClassInfo object
		oo_mthd(cl, "AssignProps"); // Assign properties

	}
}

// Constructor content
public PlayerClassInfo@Ctor(const name[], const model[], health, Float:gravity, Float:speed)
{
	new this = oo_this(); // Get this object

	// Assign values to variables
	oo_set_str(this, "name", name); // set for string
	oo_set_str(this, "model", model);
	oo_set(this, "health", health); // set for integer
	oo_set(this, "gravity", gravity); // set for float
}

public PlayerClassInfo@Dtor() {}

public PlayerClass@Ctor(player)
{
	new this = oo_this();
	oo_set(this, "player", player);
}

public PlayerClass@Dtor() {}

// Get PlayerClassInfo object
public PlayerClass@GetInfo() {}

// Assign properties
public PlayerClass@AssignProps()
{
	new this = oo_this();

	new player = oo_get(this, "player"); // get value 'player' from 'this' object
	if (!is_user_alive(player))
		return;

	new info = oo_call(this, "GetInfo"); // call method 'GetInfo' from 'this' object
	if (info == @null)
		return;

	new health = oo_get(info, "health"); // get value 'health' from 'info' object
	set_user_health(player, health);

	new Float:gravity = Float:oo_get(info, "gravity");
	set_user_gravity(player, gravity);

	new model[32];
	oo_get_str(info, "model", model, charsmax(model)); // get string value 'model' from 'info' object
	cs_set_user_model(player, model);
}