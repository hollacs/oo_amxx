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


#include "oo_manager.h"

#include <algorithm>
#include <utility>
#include <queue>
#include <unordered_set>

#include "oo_class.h"
#include "oo_object.h"


namespace oo
{
	Manager::Manager(void)
	{

	}

	std::weak_ptr<Class> Manager::NewClass(std::string_view class_name, Class &&new_class)
	{
		auto &&pair = m_classes.insert(std::make_pair(std::string{ class_name }, std::make_shared<Class>(new_class)));
		auto &&class_iter = pair.first;
		return class_iter->second;
	}

	ObjectHash Manager::NewObject(std::weak_ptr<Class> isa)
	{
		std::shared_ptr<Object> pobj = std::make_shared<Object>(Object{ isa });

		for (auto current : isa.lock()->mro)
		{
			for (auto&& mi : current.lock()->ivars)
			{
				auto mi_name = current.lock()->name + "@" + mi.first;
				auto mi_size = mi.second;
				pobj->ivars.try_emplace(mi_name, mi_size);
			}
		}

/*
		while (pcurrent != nullptr)
		{
			for (auto&& mi : pcurrent->ivars)
			{
				auto&& mi_name = mi.first;
				auto&& mi_size = mi.second;
				pobj->ivars.try_emplace(mi_name, mi_size);
			}

			pcurrent = pcurrent->super_class.lock();
		}
*/

		auto obj_hash = std::hash<std::shared_ptr<Object>>{}(pobj);
		m_objects.insert(std::make_pair(obj_hash, std::move(pobj)));

		return obj_hash;
	}

	void Manager::DeleteObject(ObjectHash object_hash)
	{
		m_objects.erase(object_hash);
	}

	std::weak_ptr<Class> Manager::ToClass(std::string_view class_name) const
	{
		auto class_iter = m_classes.find(std::string{ class_name });
		if (class_iter == m_classes.end())
			return std::weak_ptr<Class>();

		return class_iter->second;
	}

	std::weak_ptr<Object> Manager::ToObject(ObjectHash object_hash) const
	{
		auto obj_iter = m_objects.find(object_hash);
		if (obj_iter == m_objects.end())
			return std::weak_ptr<Object>();
		
		return obj_iter->second;
	}

	const Ctor* Manager::FindCtor(std::weak_ptr<Class> cl, uint8_t num_args) const
	{
		assert(!cl.expired());

		for (auto current : cl.lock()->mro)
		{
			auto ctor_iter = current.lock()->ctors.find(num_args);
			if (ctor_iter != current.lock()->ctors.end())
				return &ctor_iter->second;
		}
/*
		Class::Iterator it(cl.lock());
		auto current = cl.lock();
		while (current != nullptr)
		{
			auto ctor_iter = current->ctors.find(num_args);
			if (ctor_iter != current->ctors.end())
				return &ctor_iter->second;

			current = it.GetNext();
		}
*/
/*
		std::shared_ptr<Class> pcurrent = cl.lock();

		while (pcurrent != nullptr)
		{
			auto ctor_iter = pcurrent->ctors.find(num_args);
			if (ctor_iter != pcurrent->ctors.end())
				return &ctor_iter->second;

			pcurrent = pcurrent->super_class.lock();
		}
*/
		return nullptr;
	}

	const Method* Manager::FindMethod(std::weak_ptr<Class> cl, const std::string &name) const
	{
		assert(!cl.expired());

		for (auto current : cl.lock()->mro)
		{
			auto mthd_iter = current.lock()->methods.find(name);
			if (mthd_iter != current.lock()->methods.end())
				return &mthd_iter->second;
		}
/*
		Class::Iterator it(cl.lock());
		auto current = cl.lock();
		while (current != nullptr)
		{
			auto mthd_iter = current->methods.find(std::string{ name });
			if (mthd_iter != current->methods.end())
				return &mthd_iter->second;

			current = it.GetNext();
		}
*/
		/*
		std::shared_ptr<Class> pcurrent = cl.lock();

		while (pcurrent != nullptr)
		{
			auto mthd_iter = pcurrent->methods.find(std::string{ name });
			if (mthd_iter != pcurrent->methods.end())
				return &mthd_iter->second;

			pcurrent = pcurrent->super_class.lock();
		}*/

		return nullptr;
	}

	IVar* Manager::FindIVar(std::weak_ptr<Object> obj, const std::string &name) const
	{
		auto isa = obj.lock()->isa.lock();
		if (name.find('@') == std::string::npos)
		{
			for (auto current : isa->mro)
			{
				auto &&ivar_iter = obj.lock()->ivars.find(std::string(current.lock()->name + "@" + name));
				if (ivar_iter != obj.lock()->ivars.end())
					return &ivar_iter->second;
			}
		}
		else
		{
			auto &&ivar_iter = obj.lock()->ivars.find(name);
			if (ivar_iter != obj.lock()->ivars.end())
				return &ivar_iter->second;
		}

		return nullptr;
	}

	ObjectHash Manager::GetThis() const
	{
		return m_these.top();
	}

	void Manager::PushThis(ObjectHash next_this)
	{
		m_these.push(next_this);
	}

	ObjectHash Manager::PopThis()
	{
		ObjectHash result = m_these.top();
		m_these.pop();
		return result;
	}

	std::string_view Manager::GetObjectClassName(std::weak_ptr<Object> object) const
	{
		assert(!object.expired());

		return object.lock()->isa.lock()->name;
	}

	void Manager::Clear()
	{
		m_classes.clear();
		m_objects.clear();
		m_these = {};
	}

	Manager *Manager::Instance(void)
	{
		static Manager manager;
		return &manager;
	}
}