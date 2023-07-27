#include "oo_natives.h"
#include "oo_class.h"
#include "oo_object.h"
#include "oo_manager.h"
#include "oo_utils.h"

namespace oo {
namespace native
{

	cell AMX_NATIVE_CALL native_class(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_base = MF_GetAmxString(amx, params[2], 1, nullptr);
		int32_t _version = params[3];

		Class *dup_class = Manager::Instance()->ToClass(_class);
		if (dup_class != nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s: Duplicate class", _class);
			return 0;
		}

		Class *super = Manager::Instance()->ToClass(_base);
		if (_base[0] && super == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s: Base class (%s) not found", _class, _base);
			return 0;
		}

		Manager::Instance()->NewClass(_class, _version, _class, super);
		return 1;
	}

	cell AMX_NATIVE_CALL native_ctor(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_name = MF_GetAmxString(amx, params[2], 1, nullptr);

		Class *pclass = Manager::Instance()->ToClass(_class);
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class, _name);
			return 0;
		}

		ke::AString public_name;
		public_name.format("%s@%s", _class, _name);

		Ctor ctor;
		{
			uint8_t num_args = params[0] / sizeof(cell) - 2;

			for (uint8_t i = 1u; i <= num_args; i++)
			{
				int8_t size = (*MF_GetAmxAddr(amx, params[i + 2]));
				ctor.args.append(size);
			}

			ctor.forward_index = util::AddMethod(amx, public_name.chars(), &ctor.args);

			if (ctor.forward_index <= NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.chars());
				return 0;
			}
		}

		pclass->AddCtor(ke::Move(ctor));
		return 1;
	}

	cell AMX_NATIVE_CALL native_dtor(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_name = MF_GetAmxString(amx, params[2], 1, nullptr);

		Class *pclass = Manager::Instance()->ToClass(_class);
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class, _name);
			return 0;
		}

		ke::AString public_name;
		public_name.format("%s@%s", _class, _name);

		Dtor dtor;
		{
			dtor.forward_index = util::AddMethod(amx, public_name.chars());

			if (dtor.forward_index <= NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.chars());
				return 0;
			}
		}

		pclass->AssignDtor(ke::Move(dtor));
		return 1;
	}

	cell AMX_NATIVE_CALL native_mthd(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_name = MF_GetAmxString(amx, params[2], 1, nullptr);

		Class *pclass = Manager::Instance()->ToClass(_class);
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class, _name);
			return 0;
		}

		ke::AString public_name;
		public_name.format("%s@%s", _class, _name);

		Method mthd;
		{
			uint8_t num_args = params[0] / sizeof(cell) - 2;

			for (uint8_t i = 1u; i <= num_args; i++)
			{
				int8_t size = (*MF_GetAmxAddr(amx, params[i + 2]));
				mthd.args.append(size);
			}

			mthd.forward_index = util::AddMethod(amx, public_name.chars(), &mthd.args);
			mthd.is_static = false;

			if (mthd.forward_index <= NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.chars());
				return 0;
			}
		}

		pclass->AddMethod(_name, ke::Move(mthd));
		return 1;
	}

	cell AMX_NATIVE_CALL native_smthd(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_name = MF_GetAmxString(amx, params[2], 1, nullptr);

		Class *pclass = Manager::Instance()->ToClass(_class);
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class, _name);
			return 0;
		}

		ke::AString public_name;
		public_name.format("%s@%s", _class, _name);

		Method mthd;
		{
			uint8_t num_args = params[0] / sizeof(cell) - 2;

			for (uint8_t i = 1u; i <= num_args; i++)
			{
				int8_t size = (*MF_GetAmxAddr(amx, params[i + 2]));
				mthd.args.append(size);
			}

			mthd.forward_index = util::AddMethod(amx, public_name.chars(), &mthd.args);
			mthd.is_static = true;

			if (mthd.forward_index <= NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.chars());
				return 0;
			}
		}

		pclass->AddMethod(_name, ke::Move(mthd));
		return 1;
	}

	cell AMX_NATIVE_CALL native_var(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_name = MF_GetAmxString(amx, params[2], 1, nullptr);
		int8_t size = params[3];

		Class *pclass = Manager::Instance()->ToClass(_class);
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class, _name);
			return 0;
		}

		pclass->AddVar(_name, size);
		return 1;
	}

	cell AMX_NATIVE_CALL native_isa(AMX *amx, cell *params)
	{
		ObjectHash _this = params[1];
		const char *_class = MF_GetAmxString(amx, params[2], 0, nullptr);

		Object *pobj = Manager::Instance()->ToObject(_this);
		if (pobj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", _this);
			return -1;
		}

		Class *cls = Manager::Instance()->ToClass(_class);
		if (cls == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Class (%s) not found", _class);
			return -1;
		}

		if (params[3])
		{
			if (pobj->isa->IsClass(cls))
				return 1;

			return pobj->isa->IsSubclassOf(cls) ? 1 : 0;
		}

		return pobj->isa->IsClass(cls) ? 1 : 0;
	}

	cell AMX_NATIVE_CALL native_subclass_of(AMX *amx, cell *params)
	{
		const char *_sub = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_super = MF_GetAmxString(amx, params[2], 1, nullptr);
		
		Class *psub = Manager::Instance()->ToClass(_sub);
		if (psub == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Sub Class (%s) not found", _sub);
			return -1;
		}

		Class *super = Manager::Instance()->ToClass(_super);
		if (super == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Super Class (%s) not found", _super);
			return -1;
		}

		return psub->IsSubclassOf(super);
	}

	cell AMX_NATIVE_CALL native_new(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);

		Class *pclass = Manager::Instance()->ToClass(_class);
		if (pclass == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Class (%s) not found", _class);
			return OBJ_NULL;
		}

		ObjectHash hash = Manager::Instance()->NewObject(pclass);

		auto &&class_name = pclass->name;
		uint8_t num_args = params[0] / sizeof(cell) - 1;

		auto result = Manager::Instance()->FindCtor(pclass, num_args);
		if (result == nullptr)
		{
			if (num_args != 0)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "New %s: No such ctor (#args: %d)", class_name, num_args);
				return OBJ_NULL;
			}
		}
		else
		{
			util::ExecuteMethod(amx, params, result->forward_index, hash, &result->args, 2);
		}

		return hash;
	}

	cell AMX_NATIVE_CALL native_delete(AMX *amx, cell *params)
	{
		ObjectHash _this = params[1];

		Object *pobj = Manager::Instance()->ToObject(_this);
		if (pobj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", _this);
			return 0;
		}

		Class *pcurrent = pobj->isa;

		do
		{
			auto &&dtor = pcurrent->dtor;
			if (dtor.forward_index > NO_FORWARD)
			{
				util::ExecuteMethod(amx, params, dtor.forward_index, _this);
			}

		} while ((pcurrent = pcurrent->super_class) != nullptr);
		
		Manager::Instance()->DeleteObject(_this);
		return 0;
	}

	cell AMX_NATIVE_CALL native_call(AMX *amx, cell *params)
	{
		ObjectHash _this = params[1];
		ke::AString _name(MF_GetAmxString(amx, params[2], 0, nullptr));
		Class *pisa = nullptr;

		auto strvec = _name.split("@");
		if (_this == 0)
		{
			if (strvec.length() < 2)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Call of static method %s: Class name is required", _name.chars());
				return 0;
			}

			const char *cls_name = strvec[0].chars();
			Class *pcls = Manager::Instance()->ToClass(cls_name);
			if (pcls == nullptr)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s: No such class (%s)", _name.chars(), cls_name);
				return 0;
			}

			_name = strvec[1];
			pisa = pcls;
		}
		else
		{
			Object *pobj = Manager::Instance()->ToObject(_this);
			if (pobj == nullptr)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", _this);
				return 0;
			}

			pisa = pobj->isa;
			assert(pisa != nullptr);

			if (strvec.length() == 2)
			{
				const char* super_name = strvec[0].chars();
				Class *psuper = Manager::Instance()->ToClass(super_name);
				if (psuper == nullptr)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s (super): No such class (%s)", _name.chars(), super_name);
					return 0;
				}

				if (!pisa->IsSubclassOf(psuper))
				{
					auto &&classes = Manager::Instance()->GetClasses();
					auto &&isa_name = pisa->name.chars();
					MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s (super): %s is not a super class of %s", _name.chars(), super_name, isa_name);
					return 0;
				}

				_name = strvec[1];
				pisa = psuper;
			}
		}

		auto result = Manager::Instance()->FindMethod(pisa, _name.chars());
		if (result == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s@%s: No such method!", pisa->name.chars(), _name.chars());
			return 0;
		}

		uint8_t num_args = params[0] / sizeof(cell) - 2;
		if (num_args != result->args.length())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s@%s: #args doesn't match (expected: %d, now: %d)", pisa->name.chars(), _name.chars(), result->args.length(), num_args);
			return 0;
		}

		if (_this == 0 && !result->is_static)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s@%s: Not a static method", pisa->name.chars(), _name.chars());
			return 0;
		}

		cell ret = util::ExecuteMethod(amx, params, result->forward_index, _this, &result->args, 3);
		return ret;
	}

	cell AMX_NATIVE_CALL native_get(AMX *amx, cell *params)
	{
		ObjectHash _this = params[1];
		const char *_name = MF_GetAmxString(amx, params[2], 0, nullptr);

		Object *pobj = Manager::Instance()->ToObject(_this);
		if (pobj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s: Object (%d) not found", _name, _this);
			return 0;
		}

		Class *pisa = pobj->isa;
		assert(pisa != nullptr);

		auto var_r = pobj->vars.find(_name);
		if (!var_r.found())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s@%s: Not found", pisa->name.chars(), _name);
			return 0;
		}

		auto &&var = var_r->value;
		uint8_t num_args = params[0] / sizeof(cell) - 2;
		auto var_size = var.length();

		if (num_args == 0)
		{
			if (var_size == 1)
			{
				return var[0];
			}
			else
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s@%s: Is not a cell (size: %d), please copy by value instead", pisa->name.chars(), _name, var_size);
				return 0;
			}
		}
		else
		{
			if (var_size == 1)
			{
				MF_CopyAmxMemory(MF_GetAmxAddr(amx, params[3]), (cell*)&var[0], 1);
				return var[0];
			}
			else
			{
				if (num_args < 5)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s@%s: Required at least 5 args to get array values (now: %d)",
						pisa->name.chars(), _name, num_args);
					return 0;
				}

				size_t from_begin	= *MF_GetAmxAddr(amx, params[3]);
				size_t from_end		= *MF_GetAmxAddr(amx, params[4]);
				size_t from_diff	= from_end - from_begin;

				size_t to_begin		= *MF_GetAmxAddr(amx, params[6]);
				size_t to_end		= *MF_GetAmxAddr(amx, params[7]);
				size_t to_diff		= to_end - to_begin;

				size_t cell_count = util::clamp(from_diff, size_t(0), to_diff);

				if ((from_begin + cell_count) > var_size)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Reading IVar %s@%s: Is of size %d, but requested element #%d-%d",
						pisa->name.chars(), _name, var.length(), from_begin, (from_begin + cell_count));
					return 0;
				}

				MF_CopyAmxMemory(MF_GetAmxAddr(amx, params[5]) + to_begin, (cell*)&var[from_begin], cell_count);
				return var[0];
			}
		}
	}

	cell AMX_NATIVE_CALL native_set(AMX *amx, cell *params)
	{
		ObjectHash _this = params[1];
		const char* _name = MF_GetAmxString(amx, params[2], 0, nullptr);

		Object *pobj = Manager::Instance()->ToObject(_this);
		if (pobj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Writing IVar %s: Object (%d) not found", _name, _this);
			return 0;
		}

		Class *pisa = pobj->isa;
		assert(pisa != nullptr);

		auto var_r = pobj->vars.find(_name);
		if (!var_r.found())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Writing IVar %s@%s: Not found", pisa->name.chars(), _name);
			return 0;
		}

		auto &&var = var_r->value;
		uint8_t num_args = params[0] / sizeof(cell) - 2;
		auto &&var_size = var.length();

		if (var_size == 1)
		{
			if (num_args < 1)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Writing IVar %s@%s: Required at least 1 arg to set a cell value (now: %d)",
					pisa->name.chars(), _name, num_args);
				return 0;
			}

			var[0] = *MF_GetAmxAddr(amx, params[3]);
			return var[0];
		}
		else
		{
			if (num_args < 5)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Writing IVar %s@%s: Required at least 5 arg to set a cell value (now: %d)",
					pisa->name.chars(), _name, num_args);
				return 0;
			}

			size_t to_begin		= *MF_GetAmxAddr(amx, params[3]);
			size_t to_end		= *MF_GetAmxAddr(amx, params[4]);
			size_t to_diff		= (to_end == 0) ? var_size : to_end - to_begin;

			size_t from_begin	= *MF_GetAmxAddr(amx, params[6]);
			size_t from_end		= *MF_GetAmxAddr(amx, params[7]);
			size_t from_diff	= (from_end == 0) ? var_size : from_end - from_begin;

			size_t cell_count	= util::clamp(from_diff, size_t(0), to_diff);

			if ((to_begin + cell_count) > var_size)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Writing IVar %s@%s: Is of size %d, but requested element #%d-%d",
					pisa->name.chars(), _name, var.length(), to_begin, (to_begin + cell_count));
				return 0;
			}

			for (std::size_t i = 0; i < cell_count; i++)
				var[to_begin + i] = *(MF_GetAmxAddr(amx, params[5]) + from_begin + i);

			return var[0];
		}
	}

	cell AMX_NATIVE_CALL native_this(AMX *amx, cell *params)
	{
		return Manager::Instance()->GetThis();
	}

	cell AMX_NATIVE_CALL native_class_exists(AMX* amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);

		Class *pclass = Manager::Instance()->ToClass(_class);
		if (pclass == nullptr)
			return 0;	// no success
		
		return 1;
	}

	cell AMX_NATIVE_CALL native_object_exists(AMX* amx, cell *params)
	{
		ObjectHash _this = params[1];

		if (_this == OBJ_NULL)
			return 0;

		Object *pobj = Manager::Instance()->ToObject(_this);
		if (pobj == nullptr)
			return 0;	// no success

		return 1;
	}

	cell AMX_NATIVE_CALL native_get_classname(AMX* amx, cell *params)
	{
		ObjectHash _this = params[1];

		Object *pobj = Manager::Instance()->ToObject(_this);
		if (pobj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", _this);
			return 0;	// error
		}

		const char *classname = pobj->isa->name.chars();
		if (!classname[0])
		{
			MF_SetAmxString(amx, params[2], classname, params[3]);
		}

		return 1;
	}

	cell AMX_NATIVE_CALL native_super_ctor(AMX *amx, cell *params)
	{
		ObjectHash this_hash = Manager::Instance()->GetThis();

		Object *pthis = Manager::Instance()->ToObject(this_hash);
		if (pthis == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "*this* object is invalid");
			return 0;
		}

		const char *_superclass = MF_GetAmxString(amx, params[1], 0, nullptr);

		Class *psuper = Manager::Instance()->ToClass(_superclass);
		if (psuper == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Class (%s) not found", _superclass);
			return 0;
		}

		Class *pisa = pthis->isa;
		assert(pisa != nullptr);

		if (!pisa->IsSubclassOf(psuper))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s constructor (super): %s is not a super class of %s", _superclass, _superclass, pisa->name.chars());
			return 0;
		}

		uint8_t num_args = params[0] / sizeof(cell) - 1;

		auto result = Manager::Instance()->FindCtor(psuper, num_args);
		auto&& forward_index = result->forward_index;

		if (forward_index <= NO_FORWARD)
		{
			if (num_args != 0)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s: No such ctor (#args: %d)", _superclass, num_args);
				return 0;
			}

			// nullary constructor
		}
		else
		{
			util::ExecuteMethod(amx, params, result->forward_index, this_hash, &result->args, 2);
		}

		return 1;
	}
}}