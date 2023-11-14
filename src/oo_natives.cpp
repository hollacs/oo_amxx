// 
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

#include <utility>
#include <algorithm>

#include <cmath>
#include <cstring>

#include "oo_natives.h"

#include "oo_defs.h"
#include "oo_class.h"
#include "oo_object.h"
#include "oo_manager.h"
#include "oo_utils.h"

namespace oo::natives
{
	// native oo_decl_class(const class[], any: ...);
	// e.g.
	// oo_decl_class("Base",	"");
	// oo_decl_class("Derived",	"Base");
	cell AMX_NATIVE_CALL native_decl_class(AMX *amx, cell *params)
	{
		std::string_view _class		= MF_GetAmxString(amx, params[1], 0, nullptr);
		int32_t _version_no = params[3];

		if (!utils::IsLegit(_class))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s: Not a legit class name", _class.data());
			return 0;	// no success
		}

		if (std::weak_ptr<Class> dup_class = Manager::Instance()->ToClass(_class); !dup_class.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s: Duplicate class", _class.data());
			return 0;	// no success
		}

		uint8_t num_args = params[0] / sizeof(cell) - 1;
		if (num_args == 0)
		{
			Class new_class(OO_VERSION, std::string{_class});
			auto c = Manager::Instance()->NewClass(_class, std::move(new_class));
			c.lock()->InitMRO();
			return 1;
		}

		std::vector<std::weak_ptr<Class>> supers;

		for (uint8_t i = 1u; i <= num_args; i++)
		{
			std::string _base = MF_GetAmxString(amx, params[i + 1], 1, nullptr);
			std::weak_ptr<Class> super = Manager::Instance()->ToClass(_base);
			if (!_base.empty() && super.expired())
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s: Base class (%s) not found", _class.data(), _base.data());
				return 0;	// no success
			}

			supers.push_back(super);
		}

		Class new_class(OO_VERSION, supers, std::string{_class});
		auto c = Manager::Instance()->NewClass(_class, std::move(new_class));
		c.lock()->InitMRO();

		return 1;	// success
	}


	// native oo_decl_ctor(const class[], const name[], any: ...);
	// e.g.
	// oo_decl_ctor("Derived",	"Ctor_0arg");
	// oo_decl_ctor("Base",		"Ctor_4arg", OO_CELL, OO_VECTOR, OO_ARRAY[5], OO_STRING);
	cell AMX_NATIVE_CALL native_decl_ctor(AMX *amx, cell *params)
	{
		std::string_view _class	= MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string_view _name	= MF_GetAmxString(amx, params[2], 1, nullptr);

		std::shared_ptr<Class> pclass = Manager::Instance()->ToClass(_class).lock();
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class.data(), _name.data());
			return 0;	// no success
		}

		if (!utils::IsLegit(_name))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Not a legit ctor name", _class.data(), _name.data());
			return 0;	// no success
		}

		std::string public_name(_class.length() + 1 + _name.length(), ' ');
		public_name.replace(0, _class.length(), _class);
		public_name[_class.length()] = '@';
		public_name.replace(_class.length() + 1, _name.length(), _name);

		Ctor ctor;
		{
			uint8_t num_args = params[0] / sizeof(cell) - 2;
			//ctor.arg_sizes.resize(num_args);
			for (uint8_t i = 1u; i <= num_args; i++)
			{
				int8_t size = (*MF_GetAmxAddr(amx, params[i + 2]));
				if (!utils::IsLegitSize(size))
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Not a legit size (%d)", _class.data(), _name.data(), size);
					return 0;	// no success
				}


				ctor.args.push_back(size);
			}

			ctor.forward_index = utils::AddMethod(amx, public_name.c_str(), &ctor.args);

			if (ctor.forward_index <= NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.c_str());
				return 0;
			}
		}

		pclass->AddCtor(std::move(ctor));
		return 1;	// success
	}


	// native oo_decl_dtor(const class[], const name[], any: ...);
	// e.g.
	// oo_decl_dtor("Base",	"Dtor");
	cell AMX_NATIVE_CALL native_decl_dtor(AMX *amx, cell *params)
	{
		std::string_view _class	= MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string_view _name	= MF_GetAmxString(amx, params[2], 1, nullptr);

		std::shared_ptr<Class> pclass = Manager::Instance()->ToClass(_class).lock();
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class.data(), _name.data());
			return 0;	// no success
		}

		if (!utils::IsLegit(_name))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Not a legit dtor name", _class.data(), _name.data());
			return 0;	// no success
		}

		std::string public_name(_class.length() + 1 + _name.length(), ' ');
		public_name.replace(0, _class.length(), _class);
		public_name[_class.length()] = '@';
		public_name.replace(_class.length() + 1, _name.length(), _name);

		Dtor dtor;
		{
			dtor.forward_index = utils::AddMethod(amx, public_name.c_str());

			if (dtor.forward_index <= NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.c_str());
				return 0;
			}
		}

		pclass->AssignDtor(std::move(dtor));
		return 1;	// success
	}


	// native oo_decl_msg(const class[], const name[], any: ...);
	// e.g.
	// oo_decl_msg("Player", "EmitSound", OO_CELL, OO_VECTOR, OO_FLOAT);
	cell AMX_NATIVE_CALL native_decl_msg(AMX *amx, cell *params)
	{
		std::string_view _class = MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string_view _name	= MF_GetAmxString(amx, params[2], 1, nullptr);

		std::shared_ptr<Class> pclass = Manager::Instance()->ToClass(_class).lock();
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class.data(), _name.data());
			return 0;	// no success
		}

		if (!utils::IsLegit(_name))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Not a legit method name", _class.data(), _name.data());
			return 0;	// no success
		}

		std::string public_name(_class.length() + 1 + _name.length(), ' ');
		public_name.replace(0, _class.length(), _class);
		public_name[_class.length()] = '@';
		public_name.replace(_class.length() + 1, _name.length(), _name);

		//::MessageBoxA(nullptr, public_name.c_str(), "", MB_OK);

		Method mthd;
		{
			//::MessageBoxA(nullptr, public_name.c_str(), "2", MB_OK);

			uint8_t num_args = params[0] / sizeof(cell) - 2;
			//mthd.arg_sizes.resize(num_args);
			for (uint8_t i = 1u; i <= num_args; i++)
			{
				int8_t size = (*MF_GetAmxAddr(amx, params[i + 2]));
				if (!utils::IsLegitSize(size))
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Not a legit size (%d)", _class.data(), _name.data(), size);
					return 0;	// no success
				}

				mthd.args.push_back(size);
			}

			mthd.forward_index = utils::AddMethod(amx, public_name.c_str(), &mthd.args);
			mthd.is_static = false;

			if (mthd.forward_index <= NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.c_str());
				return 0;
			}
		}


		pclass->AddMethod(_name, std::move(mthd));
		return 1;	// success
	}

	// native oo_decl_static_msg(const class[], const name[], any: ...);
	// e.g.
	// oo_decl_static_msg("Player", "EmitSound", OO_CELL, OO_VECTOR, OO_FLOAT);
	cell AMX_NATIVE_CALL native_decl_static_msg(AMX* amx, cell *params)
	{
		std::string_view _class = MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string_view _name = MF_GetAmxString(amx, params[2], 1, nullptr);

		std::shared_ptr<Class> pclass = Manager::Instance()->ToClass(_class).lock();
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class.data(), _name.data());
			return 0;	// no success
		}

		if (!utils::IsLegit(_name))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Not a legit method name", _class.data(), _name.data());
			return 0;	// no success
		}

		std::string public_name(_class.length() + 1 + _name.length(), ' ');
		public_name.replace(0, _class.length(), _class);
		public_name[_class.length()] = '@';
		public_name.replace(_class.length() + 1, _name.length(), _name);

		//::MessageBoxA(nullptr, public_name.c_str(), "", MB_OK);

		Method mthd;
		{
			//::MessageBoxA(nullptr, public_name.c_str(), "2", MB_OK);

			uint8_t num_args = params[0] / sizeof(cell) - 2;
			//mthd.arg_sizes.resize(num_args);
			for (uint8_t i = 1u; i <= num_args; i++)
			{
				int8_t size = (*MF_GetAmxAddr(amx, params[i + 2]));
				if (!utils::IsLegitSize(size))
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Not a legit size (%d)", _class.data(), _name.data(), size);
					return 0;	// no success
				}

				mthd.args.push_back(size);
			}

			mthd.forward_index = utils::AddMethod(amx, public_name.c_str(), &mthd.args);
			mthd.is_static = true;

			if (mthd.forward_index <= NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.c_str());
				return 0;
			}
		}


		pclass->AddMethod(_name, std::move(mthd));
		return 1;	// success
	}


	// native oo_decl_ivar(const class[], const name[], size);
	// e.g.
	// oo_decl_ivar("Player", "m_Items", OO_ARRAY[4]);
	cell AMX_NATIVE_CALL native_decl_ivar(AMX *amx, cell *params)
	{
		std::string_view _class	= MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string_view _name	= MF_GetAmxString(amx, params[2], 1, nullptr);
		int8_t size				= params[3];

		std::shared_ptr<Class> pclass = Manager::Instance()->ToClass(_class).lock();
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class.data(), _name.data());
			return 0;	// no success
		}

		if (!utils::IsLegit(_name))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Not a legit method name", _class.data(), _name.data());
			return 0;	// no success
		}

		if (!utils::IsLegitSize(size))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Not a legit size (%d)", _class.data(), _name.data(), size);
			return 0;	// no success
		}

		pclass->AddIvar(_name, size);
		return 1;	// success
	}


	// native oo_isa(Obj:this, const class[], bool:superclass);
	// e.g.
	// oo_isa(derived, "Base");
	cell AMX_NATIVE_CALL native_isa(AMX *amx, cell *params)
	{
		ObjectHash _this		= params[1];
		std::string_view _class	= MF_GetAmxString(amx, params[2], 0, nullptr);

		std::shared_ptr<Object> pobj = Manager::Instance()->ToObject(_this).lock();
		if (pobj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", _this);
			return -1;	// error
		}

		std::weak_ptr<Class> cls = Manager::Instance()->ToClass(_class);
		if (cls.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Class (%s) not found", _class.data());
			return -1;	// error
		}

		if (params[3])
		{
			if (pobj->isa.lock()->IsClass(cls))
				return 1;

			return pobj->isa.lock()->IsSubclassOf(cls) ? 1 : 0;
		}

		return pobj->isa.lock()->IsClass(cls) ? 1 : 0;
	}


	// native oo_subclass_of(const sub[], const super[]);
	// e.g.
	// oo_subclass_of("Derived", "Base");
	cell AMX_NATIVE_CALL native_subclass_of(AMX *amx, cell *params)
	{
		std::string_view _sub	= MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string_view _super	= MF_GetAmxString(amx, params[2], 1, nullptr);

		std::shared_ptr<Class> psub = Manager::Instance()->ToClass(_sub).lock();
		if (psub == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Sub Class (%s) not found", _sub.data());
			return -1;	// error
		}

		std::weak_ptr<Class> super = Manager::Instance()->ToClass(_super);
		if (super.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Super Class (%s) not found", _super.data());
			return -1;	// error
		}

		return psub->IsSubclassOf(super);
	}


	// native Obj:oo_new(const class[], any: ...);
	// e.g.
	// oo_new("Player", "player name", 100, 100.0);
	cell AMX_NATIVE_CALL native_new(AMX *amx, cell *params)
	{
		std::string_view _class = MF_GetAmxString(amx, params[1], 0, nullptr);

		std::shared_ptr<Class> pclass = Manager::Instance()->ToClass(_class).lock();
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Class (%s) not found", _class.data());
			return OBJ_NULL;	// 0 - null
		}

		ObjectHash hash = Manager::Instance()->NewObject(pclass);

		uint8_t num_args = params[0] / sizeof(cell) - 1;

		auto result = Manager::Instance()->FindCtor(pclass, num_args);
		if (result == nullptr)
		{
			if (num_args != 0)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "New %s: No such ctor (#args: %d)", pclass->name.c_str(), num_args);
				return OBJ_NULL;	// 0- null
			}

			// nullary constructor
		}
		else
		{
			utils::ExecuteMethod(amx, params, result->forward_index, hash, &result->args, 2);
		}

		return hash;
	}


	// native oo_delete(Obj:this);
	// e.g.
	// oo_delete(oo_new("Player"));
	cell AMX_NATIVE_CALL native_delete(AMX *amx, cell *params)
	{
		ObjectHash _this = params[1];

		std::shared_ptr<Object> pobj = Manager::Instance()->ToObject(_this).lock();
		if (pobj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", _this);
			return 0;
		}

		for (auto current : pobj->isa.lock()->mro)
		{
			auto && dtor = current.lock()->dtor;
			if (dtor.forward_index > NO_FORWARD)
			{
				// call destructors
				utils::ExecuteMethod(amx, params, dtor.forward_index, _this);
			}
		}
/*
		Class::Iterator it(pobj->isa.lock());
		auto current = pobj->isa.lock();
		while (current != nullptr)
		{
			auto && dtor = current->dtor;
			if (dtor.forward_index > NO_FORWARD)
			{
				// call destructors
				utils::ExecuteMethod(amx, params, dtor.forward_index, _this);
			}
			current = it.GetNext();
		}
*/
		/*
		std::shared_ptr<Class> pcurrent = pobj->isa.lock();
		assert(pcurrent != nullptr);

		// calling any potential dtors (from sub to super classes)
		do
		{
			auto && dtor = pcurrent->dtor;
			if (dtor.forward_index > NO_FORWARD)
			{
				// call destructors
				utils::ExecuteMethod(amx, params, dtor.forward_index, _this);
			}
		} while ((pcurrent = pcurrent->super_class.lock()) != nullptr);
		*/
		Manager::Instance()->DeleteObject(_this);
		return 0;
	}


	// native oo_send(Obj:this, const name[], any: ...);
	// e.g.
	// oo_send(player, "BaseEntity@EmitSound", 100, origin, 1.0);
	// oo_send(player, "EmitSound", 100, origin, 1.0);
	cell AMX_NATIVE_CALL native_send(AMX *amx, cell *params)
	{
		ObjectHash _this		= params[1];
		std::string_view _name	= MF_GetAmxString(amx, params[2], 0, nullptr);
		std::shared_ptr<Class> pisa = nullptr;

		// calling any super class' method
		auto &&class_limiter_pos = _name.find('@');
		if (_this == 0)
		{
			if (class_limiter_pos == std::string_view::npos)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Call of static method %s: Class name is required", _name.data());
				return 0;
			}

			std::string_view cls_name = _name.substr(0, class_limiter_pos);
			std::shared_ptr<Class> pcls = Manager::Instance()->ToClass(cls_name).lock();
			if (pcls == nullptr)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s: No such class (%s)", _name.data(), cls_name.data());
				return 0;
			}

			_name = _name.substr(class_limiter_pos + 1);
			pisa = pcls;
		}
		else
		{
			std::shared_ptr<Object> pobj = Manager::Instance()->ToObject(_this).lock();
			if (pobj == nullptr)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", _this);
				return 0;
			}

			pisa = pobj->isa.lock();
			assert(pisa != nullptr);

			if (class_limiter_pos != std::string_view::npos)
			{
				std::string_view super_name = _name.substr(0, class_limiter_pos);
				std::shared_ptr<Class> psuper = Manager::Instance()->ToClass(super_name).lock();
				if (psuper == nullptr)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s (super): No such class (%s)", _name.data(), super_name.data());
					return 0;
				}
				if (!pisa->IsSubclassOf(psuper))
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s (super): %s is not a super class of %s", _name.data(), pisa->name.data(), pisa->name.c_str());
					return 0;
				}

				_name = _name.substr(class_limiter_pos + 1);
				pisa = psuper;
			}
		}

		auto &&classes = Manager::Instance()->GetClasses();

		auto result = Manager::Instance()->FindMethod(pisa, std::string{_name});
		if (result == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s@%s: No such method!", pisa->name.c_str(), _name.data());
			return 0;
		}

		uint8_t num_args = params[0] / sizeof(cell) - 2;
		if (num_args != result->args.size())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s@%s: #args doesn't match (expected: %d, now: %d)", pisa->name.c_str(), _name.data(), result->args.size(), num_args);
			return 0;
		}

		if (_this == 0 && !result->is_static)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s@%s: Not a static method", pisa->name.c_str(), _name.data());
			return 0;
		}

		//MF_PrintSrvConsole("calling %s@%s\n", pisa->name.c_str(), std::string(_name).c_str());

		cell ret = utils::ExecuteMethod(amx, params, result->forward_index, _this, &result->args, 3);
		return ret;
	}


	// native oo_read(Obj:this, const name[], any: ...);
	// e.g.
	// health = oo_read(player, "m_nHealth");
	// oo_read(player, "BaseEntity@m_nHealth", health);
	// oo_read(player, "m_nHealth", health);
	// oo_read(player, "m_Items", 0, 3, items, 0, 3);
	cell AMX_NATIVE_CALL native_read(AMX *amx, cell *params)
	{
		ObjectHash _this 	= params[1];
		std::string _name 	= MF_GetAmxString(amx, params[2], 0, nullptr);

		std::shared_ptr<Object> pobj = Manager::Instance()->ToObject(_this).lock();
		if (pobj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s: Object (%d) not found", _name.c_str(), _this);
			return 0;
		}

		auto isa = pobj->isa.lock();
		auto ivar = Manager::Instance()->FindIVar(pobj, _name);
		if (ivar == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s: Not found", _name.c_str());
			return 0;
		}

		uint8_t num_args = params[0] / sizeof(cell) - 2;
		auto && ivar_size = ivar->size();

		if (num_args == 0)	// get by return value
		{
			if (ivar_size == 1)
				return (*ivar)[0];
			else
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s: Is not a cell (size: %d), please copy by value instead", _name.c_str(), ivar_size);
				return 0;
			}
		}
		else				// get by value copying
		{
			if (ivar_size == 1)
			{
				MF_CopyAmxMemory(MF_GetAmxAddr(amx, params[3]), &(*ivar)[0], 1);
				return (*ivar)[0];
			}
			else
			{
				if (num_args < 5)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s: Required at least 5 args to get array values (now: %d)",
						_name.c_str(), num_args);
					return 0;
				}

				std::size_t from_begin	= *MF_GetAmxAddr(amx, params[3]);
				std::size_t from_end	= *MF_GetAmxAddr(amx, params[4]);
				std::size_t from_diff	= from_end - from_begin;

				std::size_t to_begin	= *MF_GetAmxAddr(amx, params[6]);
				std::size_t to_end		= *MF_GetAmxAddr(amx, params[7]);
				std::size_t to_diff		= to_end - to_begin;

				std::size_t cell_count	= std::clamp(from_diff, 0u, to_diff);

				if ((from_begin + cell_count) > ivar_size)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s: Is of size %d, but requested element #%d-%d",
						_name.c_str(), ivar->size(), from_begin, (from_begin + cell_count));
					return 0;
				}

				MF_CopyAmxMemory(MF_GetAmxAddr(amx, params[5]) + to_begin, &(*ivar)[from_begin], cell_count);
				return (*ivar)[0];
			}
		}


		// support for OO_STRING?
	}


	// native oo_write(Obj:this, const name[], any: ...);
	// e.g.
	// oo_write(player, "BaseEntity@m_nHealth", 100);
	// oo_write(player, "m_nHealth", 100);
	// oo_write(player, "m_Items", 0, 3, items, 0, 3);
	cell AMX_NATIVE_CALL native_write(AMX *amx, cell *params)
	{
		ObjectHash _this		= params[1];
		std::string _name	= MF_GetAmxString(amx, params[2], 0, nullptr);

		std::shared_ptr<Object> pobj = Manager::Instance()->ToObject(_this).lock();
		if (pobj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Writing IVar %s: Object (%d) not found", _name.c_str(), _this);
			return 0;
		}

		auto isa = pobj->isa.lock();
		auto ivar = Manager::Instance()->FindIVar(pobj, _name);
		if (ivar == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s: Not found", _name.c_str());
			return 0;
		}

		uint8_t num_args = params[0] / sizeof(cell) - 2;
		auto && ivar_size = ivar->size();

		if (ivar_size == 1)
		{
			if (num_args < 1)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Writing IVar %s: Required at least 1 arg to set a cell value (now: %d)",
					_name.c_str(), num_args);
				return 0;
			}

			(*ivar)[0] = *MF_GetAmxAddr(amx, params[3]);
			return (*ivar)[0];
		}
		else
		{
			if (num_args < 5)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Writing IVar %s: Required at least 5 arg to set a cell value (now: %d)",
					_name.c_str(), num_args);
				return 0;
			}

			std::size_t to_begin	= *MF_GetAmxAddr(amx, params[3]);
			std::size_t to_end		= *MF_GetAmxAddr(amx, params[4]);
			std::size_t to_diff		= (to_end == 0) ? ivar_size : to_end - to_begin;

			std::size_t from_begin	= *MF_GetAmxAddr(amx, params[6]);
			std::size_t from_end	= *MF_GetAmxAddr(amx, params[7]);
			std::size_t from_diff	= (from_end == 0) ? ivar_size : from_end - from_begin;

			std::size_t cell_count	= std::clamp(from_diff, size_t(0), to_diff);

			if ((to_begin + cell_count) > ivar_size)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Writing IVar %s: Is of size %d, but requested element #%d-%d",
					_name.c_str(), ivar->size(), to_begin, (to_begin + cell_count));
				return 0;
			}

			for (std::size_t i = 0; i < cell_count; i++)
				(*ivar)[to_begin + i] = *(MF_GetAmxAddr(amx, params[5]) + from_begin + i);

			return (*ivar)[0];
		}

		// support for OO_STRING?
	}


	// native oo_this();
	// e.g.
	// oo_send(oo_this(), "EmitSound");
	cell AMX_NATIVE_CALL native_this(AMX *amx, cell *params)
	{
		return Manager::Instance()->GetThis();
	}


	// native oo_this_ctor(any: ...);
	// e.g.
	// oo_this_ctor(100, origin, 1.0);
	cell AMX_NATIVE_CALL native_this_ctor(AMX *amx, cell *params)
	{
		return cell();
	}

	cell AMX_NATIVE_CALL native_class_exists(AMX* amx, cell *params)
	{
		std::string_view _class = MF_GetAmxString(amx, params[1], 0, nullptr);

		std::shared_ptr<Class> pclass = Manager::Instance()->ToClass(_class).lock();
		if (pclass == nullptr)
			return 0;	// no success
		
		return 1;
	}

	cell AMX_NATIVE_CALL native_object_exists(AMX* amx, cell *params)
	{
		ObjectHash _this = params[1];

		if (_this == OBJ_NULL)
			return 0;

		std::shared_ptr<Object> pobj = Manager::Instance()->ToObject(_this).lock();
		if (pobj == nullptr)
			return 0;	// no success

		return 1;
	}

	cell AMX_NATIVE_CALL native_get_class_name(AMX* amx, cell *params)
	{
		ObjectHash _this = params[1];

		std::weak_ptr<Object> pobj = Manager::Instance()->ToObject(_this);
		if (pobj.lock() == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", _this);
			return 0;	// error
		}

		std::string_view classname = Manager::Instance()->GetObjectClassName(pobj);
		if (!classname.empty())
		{
			MF_SetAmxString(amx, params[2], classname.data(), params[3]);
		}

		return 1;
	}

	// native oo_super_ctor(const super[], any: ...);
	// e.g.
	// oo_super_ctor("BaseEntity", 100, origin, 1.0);
	cell AMX_NATIVE_CALL native_super_ctor(AMX *amx, cell *params)
	{
		ObjectHash this_hash = Manager::Instance()->GetThis();

		std::shared_ptr<Object> pthis = Manager::Instance()->ToObject(this_hash).lock();
		if (pthis == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "*this* object is invalid");
			return 0;
		}

		std::string_view _superclass = MF_GetAmxString(amx, params[1], 0, nullptr);

		std::shared_ptr<Class> psuper = Manager::Instance()->ToClass(_superclass).lock();
		if (psuper == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Class (%s) not found", _superclass.data());
			return 0;
		}

		std::shared_ptr<Class> pisa = pthis->isa.lock();
		assert(pisa != nullptr);

		if (!pisa->IsSubclassOf(psuper))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s constructor (super): %s is not a super class of %s", _superclass.data(), _superclass.data(), pisa->name.c_str());
			return 0;
		}

		uint8_t num_args = params[0] / sizeof(cell) - 1;

		auto result = Manager::Instance()->FindCtor(psuper, num_args);
		auto&& forward_index = result->forward_index;

		if (forward_index <= NO_FORWARD)
		{
			if (num_args != 0)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s: No such ctor (#args: %d)", _superclass.data(), num_args);
				return 0;
			}

			// nullary constructor
		}
		else
		{
			utils::ExecuteMethod(amx, params, result->forward_index, this_hash, &result->args, 2);
		}

		return 1;
	}
}