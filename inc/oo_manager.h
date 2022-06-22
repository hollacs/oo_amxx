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


#ifndef OO_MANAGER_H
#define OO_MANAGER_H


#include <memory>

#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cstddef>

#include "oo_class.h"

namespace oo
{
	using ObjectHash	= std::size_t;
	constexpr ObjectHash OBJ_NULL = 0u;

	struct Object;

	class Manager
	{
	public:

		Manager(void);

		std::weak_ptr<Class>	NewClass(std::string_view class_name, Class &&new_class);
		ObjectHash				NewObject(std::weak_ptr<Class> isa);
		void					DeleteObject(ObjectHash object_hash);

		std::weak_ptr<Class>	ToClass(std::string_view class_name)	const;
		std::weak_ptr<Object>	ToObject(ObjectHash object_hash)		const;

		const Ctor*		FindCtor(std::weak_ptr<Class> cl, uint8_t num_args)			const;
		const Method*	FindMethod(std::weak_ptr<Class> cl, std::string_view name)	const;


		ObjectHash				GetThis() const;
		void					PushThis(ObjectHash next_this);
		ObjectHash				PopThis();

		const std::unordered_map<std::string, std::shared_ptr<Class>>	&GetClasses(void) const	{ return m_classes; }
		const std::unordered_set<std::shared_ptr<Object>>				&GetObjects(void) const { return m_objects; }

		std::string_view GetObjectClassName(std::weak_ptr<Object> object) const;

		void Clear();

		static Manager *Instance(void);


	private:

		std::unordered_map<std::string, std::shared_ptr<Class>>	m_classes;
		std::unordered_set<std::shared_ptr<Object>>				m_objects;

		std::stack<ObjectHash>	m_these;
	};
}


#endif // OO_MANAGER_H