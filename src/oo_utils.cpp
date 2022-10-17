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


#include "oo_utils.h"

#include <algorithm>
#include <cctype>

#include "assembly_create.h"
#include "memory_.h"

#include "oo_defs.h"

namespace oo::utils
{
	constexpr int CHARBITS = (8 * sizeof(char));

	bool IsLegit(std::string_view identifier)
	{
		auto is_legit_char = [](auto &&c) { return c == '_' || std::isalnum(c); };
		return
			!std::isdigit(identifier[0])										&&
			std::all_of(identifier.begin(), identifier.end(), is_legit_char)	&&
			!identifier.empty() && identifier.length() <= IDENTIFIER_LEN;
	}

	bool IsLegitSize(int8_t size)
	{
		return size >= OO_STRING_EX;
	}

	int AddMethod(AMX* amx, const char* callback, const ArgList* args)
	{
		AssemblyCreate assembly;
		assembly.add<Inst_Enter>();

		int num_args = (args == nullptr) ? 0 : args->size();
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

		assembly.add<Inst_Add_ESP_Val>()->set_inc(4 * (num_args + 3)); // real + 3?
		assembly.add<Inst_Leave>();
		assembly.add<Inst_Ret>();

		int size = assembly.size();

		unsigned char* block = assembly.get_block();

		inst_call->set_address((long)MF_RegisterSPForwardByName);

		Memory m;
		m.make_writable_executable((long)block, size);

		return reinterpret_cast<long(*)()>(block)();
	}

	cell ExecuteMethod(AMX* amx, cell* params, int8_t num_params, int32_t forward_id, ObjectHash this_hash, const ArgList* args, int8_t start_param)
	{
		Manager::Instance()->PushThis(this_hash);

		int num_args = (args == nullptr) ? 0 : args->size();

		AssemblyCreate assembly;
		assembly.add<Inst_Enter>();

		std::vector<std::pair<std::string, int>> str_arr;
		str_arr.reserve(num_args);

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
			case OO_STRING_EX:
			{
				int len = 0;
				str_arr.push_back(std::make_pair(MF_GetAmxString(amx, params[p], 0, &len), type == OO_STRING ? 0 : p));
				assembly.add<Inst_Push_VAL>()->set_long((long)(str_arr.back().first.data()));
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

		// copyback string
		for (auto& e : str_arr)
		{
			int p = e.second;
			if (p > 0)
				MF_SetAmxString(amx, params[p], e.first.data(), strlen(e.first.data()));
		}

		Manager::Instance()->PopThis();

		return ret;
	}
}