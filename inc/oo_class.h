//		OO (Object-Oriention) support as a module enabled for AMX Mod X plugins.
//		Copyright (C) 2022  Hon Fai
// 
//		This program is free software: you can redistribute itand /or modify 
//		it under the terms of the GNU General Public License as published by 
//		the Free Software Foundation, either version 3 of the License, or 
//		(at your option) any later version.
// 
//		This program is distributed in the hope that it will be useful, 
//		but WITHOUT ANY WARRANTY; without even the implied warranty of 
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//		GNU General Public License for more details.
// 
//		You should have received a copy of the GNU General Public License 
//		along with this program. If not, see <https://www.gnu.org/licenses/>.
// 


#ifndef OO_CLASS_H
#define OO_CLASS_H


#include <memory>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <cassert>
#include <cstdint>

namespace oo
{
	using AmxxForward = int;
	constexpr AmxxForward NO_FORWARD = -1;
	using ArgList	  = std::vector<int8_t>;

	struct Ctor
	{
		AmxxForward	forward_index;
		ArgList		args;
	};

	struct Dtor
	{
		AmxxForward	forward_index;
	};

	struct Method
	{
		AmxxForward	forward_index;
		ArgList		args;
	};

	struct Class : std::enable_shared_from_this<Class>
	{
		std::string									name;
		std::weak_ptr<Class>						super_class;
		int32_t										version;
		int32_t										instance_size;

		std::unordered_map<uint8_t, Ctor>			ctors;			// <key: #args>
		Dtor										dtor;			// only one destructor

		std::unordered_map<std::string, int8_t>		ivars;			// <key: ivar name,		value: ivar size>
		std::unordered_map<std::string, Method>		methods;		// <key: method name>

		Class()
			: version(0), instance_size(0), dtor({NO_FORWARD})
		{}

		Class(int32_t version, std::weak_ptr<Class> super_class, std::string name)
			: super_class(super_class), name(name)
		{
			this->version = version;

			if (std::shared_ptr psuper = super_class.lock(); psuper != nullptr)
				this->instance_size = psuper->instance_size;
			else
				this->instance_size = 0;

			this->dtor.forward_index = NO_FORWARD;
		}

		Class(int32_t version)
			: Class(version, std::weak_ptr<Class>(), std::string()) {}

		void AddCtor(Ctor ctor)
		{
			std::size_t size = ctor.args.size();
			this->ctors.insert(std::make_pair(size, std::move(ctor)));
		}

		void AssignDtor(Dtor dtor)
		{
			this->dtor = std::move(dtor);
		}

		void AddIvar(std::string_view name, int8_t size)
		{
			this->instance_size += size;
			this->ivars.insert(std::make_pair(name, size));
		}

		void AddMethod(std::string_view name, Method mthd)
		{
			this->methods.insert(std::make_pair(name, std::move(mthd)));
		}

		bool IsClass(std::weak_ptr<Class> _class)
		{
			assert(!_class.expired());

			return shared_from_this() == _class.lock();
		}

		bool IsSubclassOf(std::weak_ptr<Class> super)
		{
			assert(!super.expired());

			std::shared_ptr<Class> pcurrent = this->super_class.lock();

			while (pcurrent != nullptr)
			{
				if (pcurrent == super.lock())
					return true;

				pcurrent = pcurrent->super_class.lock();
			}

			return false;
		}
	};
}


#endif // OO_CLASS_H