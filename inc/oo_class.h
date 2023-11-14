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
#include <stack>
#include <vector>
#include <unordered_set>
#include <queue>

#include <cassert>
#include <cstdint>

namespace oo
{
	using AmxxForward = int;
	constexpr AmxxForward NO_FORWARD = -1;
	using ArgList	  = std::vector<long>;

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
		bool		is_static;
	};

	struct Class : std::enable_shared_from_this<Class>
	{
		std::string									name;
		std::vector<std::weak_ptr<Class>>			super_classes;
		int32_t										version;
		int32_t										instance_size;

		std::unordered_map<uint8_t, Ctor>			ctors;			// <key: #args>
		Dtor										dtor;			// only one destructor

		std::unordered_map<std::string, int8_t>		ivars;			// <key: ivar name,		value: ivar size>
		std::unordered_map<std::string, Method>		methods;		// <key: method name>

		std::vector<std::weak_ptr<Class>>			mro;

		Class()
			: version(0), instance_size(0), dtor({NO_FORWARD})
		{}

		Class(int32_t version, const std::string &name)
			: name(name)
		{
			this->version = version;
			this->dtor.forward_index = NO_FORWARD;
		}

		Class(int32_t version, const std::vector<std::weak_ptr<Class>> &supers, const std::string &name)
			: name(name)
		{
			this->super_classes = supers;
			this->version = version;

			for (size_t i = 0; i < supers.size(); i++)
			{
				this->instance_size += supers[i].lock()->instance_size;
			}

			this->dtor.forward_index = NO_FORWARD;
		}

		Class(int32_t version)
			: Class(version, "") {}

		void AddCtor(Ctor &&ctor)
		{
			std::size_t size = ctor.args.size();
			this->ctors.insert(std::make_pair(size, std::move(ctor)));
		}

		void AssignDtor(Dtor &&dtor)
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

		bool IsClass(std::weak_ptr<Class> cls)
		{
			assert(!cls.expired());

			return shared_from_this() == cls.lock();
		}

		bool IsSubclassOf(std::weak_ptr<Class> cls)
		{
			assert(!cls.expired());

			for (auto current : mro)
			{
				if (current.lock() == cls.lock())
					return true;
			}

			return false;
		}

		void InitMRO()
		{
			std::queue<std::weak_ptr<Class>> to_visit;
			std::unordered_set<std::shared_ptr<Class>> visited;
			to_visit.push(shared_from_this());

			while (!to_visit.empty())
			{
				auto current = to_visit.front().lock();
				to_visit.pop();

				if (visited.find(current) == visited.end())
				{
					visited.insert(current);

					for (auto super : current->super_classes)
					{
						to_visit.push(super);
					}

					mro.push_back(current);
				}
			}
		}
	};
}


#endif // OO_CLASS_H