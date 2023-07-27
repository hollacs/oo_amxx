#ifndef OO_CLASS_H
#define OO_CLASS_H

#include <amtl/am-string.h>
#include <amtl/am-hashmap.h>
#include <amtl/am-vector.h>
#include "oo_policies.h"

namespace oo
{
	using AmxxForward 	= int;
	using ArgList 		= ke::Vector<int8_t>;
	KE_CONSTEXPR AmxxForward NO_FORWARD = -1;

	struct Ctor
	{
		AmxxForward forward_index;
		ArgList 	args;
	};

	struct Dtor
	{
		AmxxForward forward_index;
	};

	struct Method
	{
		AmxxForward forward_index;
		ArgList 	args;
		bool 		is_static;
	};

	struct Class
	{
		ke::AString name;
		Class		*super_class;
		int32_t 	version;
		int32_t 	instance_size;

		ke::HashMap<uint32_t, Ctor, IntegerPolicy> ctors;
		Dtor dtor;

		ke::HashMap<ke::AString, int8_t, StringPolicy> vars;
		ke::HashMap<ke::AString, Method, StringPolicy> methods;

		Class() 
			: version(0), instance_size(0), dtor({NO_FORWARD})
		{
			Init();
		}

		Class(int32_t version, ke::AString name, Class *super)
			: name(name), super_class(super), version(version), dtor({NO_FORWARD})
		{
			if (super != nullptr)
				this->instance_size = super->instance_size;
			else
				this->instance_size = 0;
			
			Init();
		}

		Class(int32_t version)
			: Class(version, ke::AString(), nullptr)
		{
			Init();
		}

		void Init()
		{
			ctors.init();
			vars.init();
			methods.init();
		}

		void AddCtor(Ctor ctor)
		{
			size_t size = ctor.args.length();
			auto in = this->ctors.findForAdd(size);
			if (!in.found())
				this->ctors.add(in, size, ke::Move(ctor));
		}

		void AssignDtor(Dtor dtor)
		{
			this->dtor = ke::Move(dtor);
		}

		void AddVar(const char *name, int8_t size)
		{
			this->instance_size += size;
			auto in = this->vars.findForAdd(name);
			if (!in.found())
				this->vars.add(in, ke::AString(name), size);
		}

		void AddMethod(const char *name, Method mthd)
		{
			auto in = this->methods.findForAdd(name);
			if (!in.found())
				this->methods.add(in, ke::AString(name), ke::Move(mthd));
		}

		bool IsClass(Class *_class)
		{
			return this == _class;
		}

		bool IsSubclassOf(Class *super)
		{
			Class *current = this->super_class;

			while (current != nullptr)
			{
				if (current == super)
					return true;
				
				current = current->super_class;
			}

			return false;
		}
	};
}

#endif