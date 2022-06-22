#if defined _oo_macros_included
	#endinput
#endif

#define _oo_macros_included


// @class("Class")
// @subclass("SubClass", "Class")
// @init_class("ClassName")
#define @class(%0) 							oo_decl_class(%0)
#define @subclass(%0,%1)					oo_decl_class(%0, %1)
#define @init_class(%0)						new _class_name_[64] = %0

// @var (OO_CELL:VariableName)
// @var (OO_ARRAY[32]:VariableName)
#define @var(%0:%1)							oo_decl_ivar(_class_name_, #%1, %0)

// @method :Function(any:...)
// @method0 :Function()
// @construct :Function(any:...)
// @construct0 :Function()
// @destruct :Function()
#define @construct:%0(%1)					oo_decl_ctor(_class_name_, #%0, %1)
#define @construct0:%0() 					oo_decl_ctor(_class_name_, #%0)
#define @destruct:%0()						oo_decl_dtor(_class_name_, #%0)
#define @method:%0(%1) 						oo_decl_msg(_class_name_, #%0, %1)
#define @method0:%0() 						oo_decl_msg(_class_name_, #%0)

// @new("Class", any:...)
// @new0("Class")
// @delete(object)
#define @new(%0,%1)							oo_new(%0, %1)
#define @new0(%0) 							oo_new(%0)
#define @delete(%0)							oo_delete(%0)

// @get(object.var) // cell
// @geta(object.var[begin..end] >> array_out[begin..end]) // array
// @gets(object.var[] >> string_out[size]) // string
#define @get(%0.%1)							oo_read(%0, #%1)
#define @geta(%0.%1[%2..%3]>>%4[%5..%6]) 	oo_read(%0, #%1, %2, %3, %4, %5, %6)
#define @gets(%0.%1[]>>%2[%3]) 				oo_read(%0, #%1, %2, %3)

// @set(object.var. = value) // cell
// @seta(object.var[begin..end] << array_value[begin..end]) // array
// @sets(object.var[] << "string") // string
#define @set(%0.%1:=%2)						oo_write(%0, #%1, %2)
#define @seta(%0.%1[%2..%3]<<%4[%5..%6])	oo_write(%0, #%1, %2, %3, %4, %5, %6)
#define @sets(%0.%1[]<<%2)					oo_write(%0, #%1, %2)

#define @call:%0.%1(%2)			oo_send(%0, #%1, %2)
#define @call0:%0.%1()			oo_send(%0, #%1)

// increment@object["ivar"]
// decrement@object["ivar"]
#define @increment(%0[%1]++) 	oo_write(%0, oo_read(%0, %1) + 1)
#define @decrement(%0[%1]--) 	oo_write(%0, oo_read(%0, %1) - 1)

#define @init_this(%0) 	new Obj:%0 = oo_this()
#define _this 			oo_this()

#define OO_CELL			(1)		// cell
#define OO_FLOAT		(1)		// float
#define OO_BYREF		(0) 	// cell or float by reference
#define OO_STRING		(-1) 	// const string[]
#define OO_STRING_EX	(-2) 	// string[]
#define OO_VEC2			(2)		// not necessary
#define OO_VEC3			(3)		// not necessary
#define OO_ARRAY[%0]	(%0)	// array[size]

#define @cell 			OO_CELL
#define @float 			OO_FLOAT
#define @byref 			OO_BYREF
#define @string 		OO_STRING
#define @stringex		OO_STRING_EX
#define @vec2			OO_VEC2
#define @vec3			OO_VEC3
#define @array[%0]		OO_ARRAY[%0]