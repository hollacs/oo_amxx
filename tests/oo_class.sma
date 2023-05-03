#include <amxmodx>
#include <oo>

public oo_init()
{
	oo_class("Person")
	{
		new cl[] = "Person";

		oo_var(cl, "id", 16); // one cell variable
		oo_var(cl, "name", 32); // string variable (length 32)
		oo_var(cl, "sex", 1);
		oo_var(cl, "phone_no", 32);
		oo_var(cl, "email", 64);
		oo_var(cl, "test_array", 3);

		// constructor
		oo_ctor(cl, "Ctor", @str{id}, @str{name}, @int{sex}, @str{phone_no}, @str{email});

		// destructor
		oo_dtor(cl, "Dtor");

		oo_mthd(cl, "SetArray", @arr{test[3]});
	}
}

public Person@Ctor(const id[], const name[], sex, const phone_no[], const email[])
{
	new this = oo_this();

	oo_set_str(this, "id", id);
	oo_set_str(this, "name", name);
	oo_set(this, "sex", sex);
	oo_set_str(this, "phone_no", phone_no);
	oo_set_str(this, "email", email);
}

public Person@Dtor()
{

}

public Person@SetArray(test[3])
{
	oo_set_arr(this, "test_array", test);
}