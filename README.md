# oo_amxx
Object-Oriented Simulation for AMXXPawn<br>
Idea drafted with BB.O.<br>
Author: BB.O. & holla

Credits: OKAPI module for using variable arguments in MF_ExecuteForward() and MF_RegisterSpForward()

Example:
```sourcepawn
#include <amxmodx>
#include <oo>

#define main plugin_init

public oo_init()
{
	// A base class for all animals
	oo_class("Animal")
	{
		new cl[] = "Animal";

		oo_var(cl, "age", 1); // A attribute that stores the age of the animal
		oo_var(cl, "name", 32); // A attribute that stores the name of the animal

		// A constructor that takes the name and the age of the animal
		oo_ctor(cl, "Ctor", @str(name), @int(age));

		// A destructor of the animal
		oo_dtor(cl, "Dtor");

		// A function that returns the sound of the animal
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		// A function that returns the number of legs of the animal
		oo_mthd(cl, "GetLegs");

		// A function that prints the name and sound of the animal
		oo_mthd(cl, "Introduce");

		oo_smthd(cl, "Test"); // static method test
	}

	// A derived class for dogs that inherits from Animal
	oo_class("Dog", "Animal")
	{
		new cl[] = "Dog";

		// A constructor that calls the base class constructor with the name "Dog"
		oo_ctor(cl, "Ctor", @int(age));

		// An override function that returns the sound of a dog
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		// An override function that returns the number of legs of a dog
		oo_mthd(cl, "GetLegs");
	}

	// A derived class for cats that inherits from Animal
	oo_class("Cat", "Animal")
	{
		new cl[] = "Cat";

		// A constructor that calls the base class constructor with the name "Cat"
		oo_ctor(cl, "Ctor", @int(age));

		// An override function that returns the sound of a cat
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		// An override function that returns the number of legs of a cat
		oo_mthd(cl, "GetLegs");
	}

	// A derived class for birds that inherits from Animal
	oo_class("Bird", "Animal")
	{
		new cl[] = "Bird";

		// A constructor that calls the base class constructor with the name "Bird"
		oo_ctor(cl, "Ctor", @int(age));

		// An override function that returns the sound of a bird
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		// An override function that returns the number of legs of a bird
		oo_mthd(cl, "GetLegs");
	}

	// A derived class for snakes that inherits from Animal
	oo_class("Snake", "Animal")
	{
		new cl[] = "Snake";

		// A constructor that calls the base class constructor with the name "Snake"
		oo_ctor(cl, "Ctor", @int(age));

		// An override function that returns the sound of a snake
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		// An override function that returns the number of legs of a snake
		oo_mthd(cl, "GetLegs");
	}
}

// A constructor of Animal
public Animal@Ctor(const name[], age)
{
	new this = oo_this(); // Get this object hash
	oo_set_str(this, "name", name); // Assign string value to the member variable of "name"
	oo_set(this, "age", age); // Assign integer value to the member variable of "age"
}

// A destructor of Animal
public Animal@Dtor()
{
	new name[32];
	oo_get_str(oo_this(), "name", name, 32); // Get string value from the member variable of "name"
	server_print("%s has been euthanized.", name);
}

public Animal@MakeSound(msg[], len)
{
	// format the message to the msg[]
	formatex(msg, len, "I am an animal");
}

public Animal@GetLegs()
{
	return 0;
}

public Animal@Introduce()
{
	new this = oo_this();

	new name[32];
	oo_get_str(this, "name", name, 32);

	new age = oo_get(this, "age"); // Get integer value from the member variable of "age"

	new legs = oo_call(this, "GetLegs");

	// Call "MakeSound" function of this animal and retrieve the result to the msg[]
	new msg[64];
	oo_call(this, "MakeSound", msg, charsmax(msg));

	// Print the introduction
	server_print("Hello, my name is %s, I'm %d years old, I have %d legs, and I say %s", name, age, legs, msg);
}

public Animal@Test() { server_print("static method test"); }

public Dog@Ctor(age)
{
	// Call the parent constructor of this dog
	oo_super_ctor("Animal", "Dog", age);
}

public Dog@MakeSound(msg[], len)
{
	formatex(msg, len, "Woof woof");
}

public Dog@GetLegs()
{
	return 4;
}

public Cat@Ctor(age)
{
	oo_super_ctor("Animal", "Cat", age);
}

public Cat@MakeSound(msg[], len)
{
	formatex(msg, len, "Meow meow");
}

public Cat@GetLegs()
{
	return 4;
}

public Bird@Ctor(age)
{
	oo_super_ctor("Animal", "Bird", age);
}

public Bird@MakeSound(msg[], len)
{
	formatex(msg, len, "Tweet tweet");
}

public Bird@GetLegs()
{
	return 2;
}

public Snake@Ctor(age)
{
	oo_super_ctor("Animal", "Snake", age);
}

public Snake@MakeSound(msg[], len)
{
	formatex(msg, len, "Sss sss");
}

public Snake@GetLegs()
{
	return 0;
}

public main()
{
	register_plugin("[OO] Animal", "0.1", "holla");

	new Animal:animals[5];
	animals[0] = oo_new("Dog", 7);
	animals[1] = oo_new("Cat", 6);
	animals[2] = oo_new("Bird", 4);
	animals[3] = oo_new("Snake", 3);
	animals[4] = oo_new("Animal", "Unknown", 0);

	for (new i = 0; i < 5; i++)
	{
		oo_call(animals[i], "Introduce"); // Call Introduce function for each animals
	}

	// Tests
	oo_call(0, "Animal@Test"); // Test calling the static method

	server_print("Object #%d %s a Snake", animals[3], oo_isa(animals[3], "Snake") ? "IS" : "IS NOT");
	server_print("Object #%d %s a Dog", animals[3], oo_isa(animals[3], "Dog") ? "IS" : "IS NOT");

	server_print("Class Dog %s a subclass of Animal", oo_subclass_of("Dog", "Animal") ? "IS" : "IS NOT");
	server_print("Class Animal %s a subclass of Cat", oo_subclass_of("Animal", "Cat") ? "IS" : "IS NOT");

	server_print("Class Bird %s", oo_class_exists("Bird") ? "EXISTS" : "DOES NOT EXIST");
	server_print("Class Fish %s", oo_class_exists("Fish") ? "EXISTS" : "DOES NOT EXIST");

	new class[32];
	oo_get_classname(animals[0], class, charsmax(class));
	server_print("Object #%d's classname is %s", animals[0], class);

	server_print("Object #%d %s", animals[0], oo_object_exists(animals[0]) ? "EXISTS" : "DOES NOT EXIST");

	for (new i = 0; i < 5; i++)
	{
		oo_delete(animals[i]); // Delete each animal objects
	}

	server_print("Object #%d %s", animals[0], oo_object_exists(animals[0]) ? "EXISTS" : "DOES NOT EXIST");
}
```

outputs:
```
Hello, my name is Dog, I'm 7 years old, I have 4 legs, and I say Woof woof
Hello, my name is Cat, I'm 6 years old, I have 4 legs, and I say Meow meow
Hello, my name is Bird, I'm 4 years old, I have 2 legs, and I say Tweet tweet
Hello, my name is Snake, I'm 3 years old, I have 0 legs, and I say Sss sss
Hello, my name is Unknown, I'm 0 years old, I have 0 legs, and I say I am an animal
static method test
Object #1943714116 IS a Snake
Object #1943714116 IS NOT a Dog
Class Dog IS a subclass of Animal
Class Animal IS NOT a subclass of Cat
Class Bird EXISTS
Class Fish DOES NOT EXIST
Object #461123433's classname is Dog
Object #461123433 EXISTS
Dog has been euthanized.
Cat has been euthanized.
Bird has been euthanized.
Snake has been euthanized.
Unknown has been euthanized.
Object #461123433 DOES NOT EXIST
```

## Multiple Inheritance
For the version of 1.1.0, now support multiple inheritance! \
It's based on python's method resolution order (MRO) \
Example:
```sourcepawn
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
```

outputs:
```
B1@Method1()
B2@Method2()
B1@Print()
B2@Print()
D@Print(hp=100, a=689, b=777)
```
