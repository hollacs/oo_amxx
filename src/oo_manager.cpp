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
		if (isa.expired())
			return OBJ_NULL;

		std::shared_ptr<Object> pobj = std::make_shared<Object>(isa);
		std::shared_ptr<Class> pcurrent(isa);

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

		auto obj_iter = m_objects.insert(std::move(pobj)).first;
		return m_objects.hash_function()(*obj_iter);
	}

	void Manager::DeleteObject(ObjectHash object_hash)
	{
		//std::size_t bucket = object_hash % m_objects.bucket_count();
		auto is_hash = [&](auto &&pobj) { return object_hash == m_objects.hash_function()(pobj); };
		std::erase_if(m_objects, is_hash);
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
		std::size_t bucket = object_hash % m_objects.bucket_count();
		auto pobj_iter = std::find_if(m_objects.begin(bucket), m_objects.end(bucket), [&](auto &&pobj) { return object_hash == m_objects.hash_function()(pobj); });
		if (pobj_iter == m_objects.end(bucket))
			return std::weak_ptr<Object>();

		return (*pobj_iter);
	}

	const Ctor* Manager::FindCtor(std::weak_ptr<Class> cl, uint8_t num_args) const
	{
		if (cl.expired())
			return nullptr;

		std::shared_ptr<Class> pcurrent = cl.lock();

		while (pcurrent != nullptr)
		{
			auto ctor_iter = pcurrent->ctors.find(num_args);
			if (ctor_iter != pcurrent->ctors.end())
				return &ctor_iter->second;

			pcurrent = pcurrent->super_class.lock();
		}

		return nullptr;
	}

	const Method* Manager::FindMethod(std::weak_ptr<Class> cl, std::string_view name) const
	{
		if (cl.expired())
			return nullptr;

		std::shared_ptr<Class> pcurrent = cl.lock();

		while (pcurrent != nullptr)
		{
			auto mthd_iter = pcurrent->methods.find(std::string{ name });
			if (mthd_iter != pcurrent->methods.end())
				return &mthd_iter->second;

			pcurrent = pcurrent->super_class.lock();
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
		std::weak_ptr<Class> obj_class = object.lock()->isa;

		for (auto&& _class : m_classes)
		{
			if (_class.second.get() == obj_class.lock().get())
				return _class.first;
		}

		return {};
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