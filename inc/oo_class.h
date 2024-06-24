#ifndef OO_CLASS_H
#define OO_CLASS_H

#include <amtl/am-string.h>
#include <amtl/am-hashmap.h>
#include <amtl/am-hashset.h>
#include <amtl/am-vector.h>
#include <amtl/am-deque.h>
#include "oo_policies.h"

namespace oo
{
	using AmxxForward 	= int;
	using ArgList 		= ke::Vector<int8_t>;
	using HookChain		= ke::Vector<AmxxForward>;
	KE_CONSTEXPR AmxxForward NO_FORWARD = -1;

	struct Ctor
	{
		AmxxForward forward_index = NO_FORWARD;
		ArgList 	args;

		HookChain 	pre;
		HookChain 	post;
	};

	struct Dtor
	{
		AmxxForward forward_index = NO_FORWARD;

		HookChain 	pre;
		HookChain 	post;
	};

	struct Method
	{
		AmxxForward forward_index = NO_FORWARD;
		ArgList 	args;
		bool 		is_static = false;

		HookChain 	pre;
		HookChain 	post;
	};

	struct Class
	{
		int32_t 	version;
		ke::AString name;
		int32_t 	instance_size;
		Dtor 		dtor;

		ke::Vector<Class *> 							super_classes;
		ke::HashMap<uint32_t, Ctor, IntegerPolicy> 		ctors;
		ke::HashMap<ke::AString, Ctor *, StringPolicy> 	ctor_map;
		ke::HashMap<ke::AString, int8_t, StringPolicy> 	vars;
		ke::HashMap<ke::AString, Method, StringPolicy> 	methods;
		ke::Vector<Class *> 							mro;

		Class() 
			: version(0), name(""), instance_size(0)
		{
			Init();
		}

		Class(int32_t version, const char *name) : version(version), name(name), instance_size(0)
		{
			Init();
		}

		Class(int32_t version, const char *name, ke::Vector<Class *> *supers)
			: version(version), name(name)
		{
			if (supers != nullptr)
			{
				for (size_t i = 0; i < supers->length(); i++)
				{
					auto super = supers->at(i);
					super_classes.append(super);
					this->instance_size += super->instance_size;
				}
			}
			else
				this->instance_size = 0;

			Init();
		}

		Class(int32_t version)
			: version(version), name(""), instance_size(0)
		{
			Init();
		}

		void Init()
		{
			ctors.init();
			vars.init();
			methods.init();
			ctor_map.init();
		}

		void AddCtor(Ctor ctor, const char *name)
		{
			size_t size = ctor.args.length();
			auto in = this->ctors.findForAdd(size);
			if (!in.found())
				this->ctors.add(in, size, ke::Move(ctor));

			auto in2 = this->ctor_map.findForAdd(name);
			if (!in2.found())
				this->ctor_map.add(in2, ke::AString(name), &in->value);
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
			for (size_t i = 0; i < mro.length(); i++)
			{
				if (mro[i] == super)
					return true;
			}

			return false;
		}

		void InitMro()
		{
			ke::Deque<Class *> to_visit;
			ke::HashSet<Class *, ke::PointerPolicy<Class>> visited;
			visited.init();
			to_visit.append(this);

			while (!to_visit.empty())
			{
				auto current = to_visit.front();
				to_visit.popFront();

				auto r = visited.find(current);
				if (!r.found())
				{
					visited.add(current);

					for (size_t i = 0; i < current->super_classes.length(); i++)
					{
						to_visit.append(current->super_classes[i]);
					}

					mro.append(current);
				}
			}
		}
	};
}


#endif