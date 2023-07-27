#include "oo_utils.h"
#include <amtl/am-uniqueptr.h>
#include "assembly_create.h"
#include "memory_.h"
#include "oo_defs.h"
#include "oo_manager.h"

namespace oo { 
namespace util
{
	AmxxForward AddMethod(AMX *amx, const char *callback, const ArgList *args)
	{
		AssemblyCreate assembly;
		assembly.add<Inst_Enter>();

		int num_args = (args == nullptr) ? 0 : args->length();
		int start = num_args - 1;
		int end = 0;

		assembly.add<Inst_Push_VAL>()->set_long(FP_DONE);

		for (int i = start; i >= end; i--)
		{
			cell type = 0;

			switch (args->at(i))
			{
			case OO_CELL:
				type = FP_CELL;
				break;

			case OO_BYREF:
				type = FP_CELL_BYREF;
				break;

			case OO_STRING:
				type = FP_STRING;
				break;

			case OO_STRING_EX:
				type = FP_STRINGEX;
				break;

			default:
				if (args->at(i) > OO_CELL) // ARRAY
					type = FP_ARRAY;
				
				break;
			}

			assembly.add<Inst_Push_VAL>()->set_long(type);
		}

		assembly.add<Inst_Push_VAL>()->set_long((long)callback);
		assembly.add<Inst_Push_VAL>()->set_long((long)amx);

		Inst_Call* inst_call = assembly.add<Inst_Call>();

		assembly.add<Inst_Add_ESP_Val>()->set_inc(4 * (num_args + 3));
		assembly.add<Inst_Leave>();
		assembly.add<Inst_Ret>();

		int size = assembly.size();

		unsigned char* block = assembly.get_block();

		inst_call->set_address((long)MF_RegisterSPForwardByName);

		Memory m;
		m.make_writable_executable((long)block, size);

		return reinterpret_cast<long(*)()>(block)();
	}

	cell ExecuteMethod(AMX *amx, cell *params, AmxxForward forward_id, ObjectHash this_hash, const ArgList* args, int8_t start_param)
	{
		Manager::Instance()->PushThis(this_hash);

		int num_args = (args == nullptr) ? 0 : args->length();

		AssemblyCreate assembly;
		assembly.add<Inst_Enter>();

		struct StringData
		{
			ke::UniquePtr<char[]> str;
			int index;
		};
		
		ke::Vector<StringData> str_arr;
		str_arr.ensure(num_args);

		int start = num_args - 1;
		int end = 0;

		for (int i = start; i >= end; i--)
		{
			int p = i + start_param;
			int8_t type = args->at(i);

			switch (type)
			{
			case OO_CELL:
				assembly.add<Inst_Push_VAL>()->set_long(MF_GetAmxAddr(amx, params[p])[0]);
				break;

			case OO_BYREF:
				assembly.add<Inst_Push_VAL>()->set_long((long)&MF_GetAmxAddr(amx, params[p])[0]);
				break;

			case OO_STRING:
			{
				int length = 0;
				auto str = MF_GetAmxString(amx, params[p], 0, &length);
				auto chars = ke::MakeUnique<char[]>(length + 1);
				memcpy(chars.get(), str, length);
				chars[length] = '\0';
				str_arr.append(StringData{ke::Move(chars), 0});

				assembly.add<Inst_Push_VAL>()->set_long((long)(str_arr.back().str.get()));
				break;
			}

			case OO_STRING_EX:
			{
				int length = 0;
				auto str = MF_GetAmxString(amx, params[p], 0, &length);
				auto chars = ke::MakeUnique<char[]>((length < 255) ? 255 : length + 1);
				memcpy(chars.get(), str, length);
				chars[length] = '\0';
				str_arr.append(StringData{ke::Move(chars), p});

				assembly.add<Inst_Push_VAL>()->set_long((long)(str_arr.back().str.get()));
				break;
			}

			default:
				if (type > OO_CELL)
					assembly.add<Inst_Push_VAL>()->set_long(MF_PrepareCellArrayA(MF_GetAmxAddr(amx, params[p]), type, true));

				break;
			}
		}

		assembly.add<Inst_Push_VAL>()->set_long((long)forward_id);

		Inst_Call* inst_call = assembly.add<Inst_Call>();

		assembly.add<Inst_Add_ESP_Val>()->set_inc(4 * (num_args + 1));
		assembly.add<Inst_Leave>();
		assembly.add<Inst_Ret>();

		int size = assembly.size();

		unsigned char* block = assembly.get_block();

		inst_call->set_address((long)MF_ExecuteForward);

		Memory m;
		m.make_writable_executable((long)block, size);

		cell ret = reinterpret_cast<long(*)()>(block)();

		for (auto& e : str_arr)
		{
			if (e.index > 0)
				MF_SetAmxString(amx, params[e.index], e.str.get(), strlen(e.str.get()));
		}

		Manager::Instance()->PopThis();

		return ret;
	}
}}