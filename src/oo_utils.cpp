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
		unsigned int arg_count = (args != nullptr) ? args->size() : 0;
		std::vector<ForwardParam> arguments(arg_count);
		//arguments[0] = FP_CELL;

		if (arg_count)
		{
			for (size_t i = 0; i < args->size(); i++)
			{
				switch (args->at(i))
				{
				case OO_CELL:
					arguments[i] = FP_CELL;
					break;

				case OO_BYREF:
					arguments[i] = FP_CELL_BYREF;
					break;

				case OO_STRING:
					arguments[i] = FP_ARRAY;
					break;

				case OO_STRING_EX:
					arguments[i] = FP_ARRAY;
					break;

				default:
					arguments[i] = FP_ARRAY;
					break;
				}
			}
		}

		long espDislocation = sizeof(long) * arg_count;
		long espDislocationAddHook = espDislocation + sizeof(long) + sizeof(char*) + sizeof(AMX*);
		long forward;
		ForwardParam* arg_ptr = arguments.data();

#ifdef WIN32

		unsigned int start = arg_count;
		long addressStart = (long)&arg_ptr[0];
		espDislocation = espDislocationAddHook;

		_asm
		{
			mov eax, addressStart
			mov ebx, start
			push FP_DONE
			loopStartAdd :
			cmp ebx, 0
				je loopEndAdd
				dec ebx
				mov  ecx, [eax + ebx * 4]
				push ecx
				jmp loopStartAdd
				loopEndAdd :
			push callback
				push amx
				call MF_RegisterSPForwardByName
				mov forward, eax
				add esp, espDislocation
		}
#else
		asm volatile(
			"mov %1, %%eax;"
			"mov %2, %%ebx;"
			"pushl $-1;"
			"loopStart:"
			"cmp $0, %%ebx;"
			"je loopEnd;"
			"dec %%ebx;"
			"mov (%%eax,%%ebx,4),%%ecx;"
			"pushl %%ecx;"
			"jmp loopStart;"
			"loopEnd:"
			"pushl %3;"
			"pushl %4;"
			"call *%5;"
			"mov %%eax, %0;"
			"add %6, %%esp;"
			:"=r"(forward) : "m"(arg_ptr), "m"(arg_count), "r"(callback), "r"(amx), "m"(MF_RegisterSPForwardByName), "m"(espDislocationAddHook) : "eax", "ebx", "ecx");
#endif

		return forward;
	}

	cell ExecuteMethod(AMX* amx, cell* params, int32_t forward_id, ObjectHash this_hash, const ArgList* args, int8_t start_param)
	{
		Manager::Instance()->PushThis(this_hash);

		unsigned int arg_count = (args != nullptr) ? args->size() : 0;
		//MF_PrintSrvConsole("arg_count = %d\n", arg_count);

		std::vector<cell> arguments(arg_count);
		//arguments[0] = this_hash;

		std::vector<std::pair<int, std::string>> str_arr;
		//MF_PrintSrvConsole("argument.size = %d\n", arguments.size());

		long returnResult = 0;
		long espDislocation = sizeof(long) * arg_count;
		long espDislocationCallForward = espDislocation + sizeof(long);
		cell* arg_ptr = arguments.data();

		if (arg_count)
		{
			str_arr.reserve(args->size());

			for (size_t i = 0; i < args->size(); i++)
			{
				int p = i + start_param;
				long type = args->at(i);

				switch (type)
				{
				case OO_CELL:
					arguments[i] = MF_GetAmxAddr(amx, params[p])[0];
					break;

				case OO_BYREF:
					arguments[i] = (long)&MF_GetAmxAddr(amx, params[p])[0];
					break;

				case OO_STRING:
				case OO_STRING_EX:
				{
					int len = 0;
					int pp = (type == OO_STRING) ? 0 : p;
					str_arr.push_back(std::make_pair(pp, MF_GetAmxString(amx, params[p], 0, &len)));
					auto& back = str_arr.back();

					if (pp > 0)
					{
						back.second.resize(256, '\0');
						len = 256;
					}
					
					arguments[i] = MF_PrepareCharArrayA(back.second.data(), len+1, pp > 0);
					break;
				}
				default:
					if (type > OO_CELL)
						arguments[i] = MF_PrepareCellArrayA(MF_GetAmxAddr(amx, params[p]), type, true);
					break;
				}
			}
		}

#if defined WIN32

		unsigned int start = arg_count;
		long addressStart = (long)&arg_ptr[0];
		espDislocation = espDislocationCallForward;

		_asm
		{
			mov eax, addressStart
			mov ebx, start
			loopStart :
			cmp ebx, 0
				je loopEnd
				dec ebx
				mov  ecx, [eax + ebx * 4]
				push ecx
				jmp loopStart
				loopEnd :
			push forward_id
				call MF_ExecuteForward
				mov returnResult, eax
				add esp, espDislocation
		}

#else

		asm volatile (
			"mov %1, %%eax;"
			"mov %2, %%ebx;"
			"0:"
			"cmp $0, %%ebx;"
			"je 1f;"
			"dec %%ebx;"
			"mov (%%eax,%%ebx,4),%%ecx;"
			"pushl %%ecx;"
			"jmp 0b;"
			"1:"
			"pushl %3;"
			"call *%4;"
			"mov %%eax, %0;"
			"add %5, %%esp;"
			:"=r"(returnResult) : "m"(arg_ptr), "m"(arg_count), "r"(forward_id), "m"(MF_ExecuteForward), "r"(espDislocationCallForward) : "eax", "ebx", "ecx");
#endif

		// copyback string
		for (auto& e : str_arr)
		{
			int p = e.first;
			if (p > 0)
			{
				//MF_Log("diao=[%s][%d][%d]", e.second.data(), strlen(e.second.data()), p);
				//MF_Log("pp is >>%d<<", p);
				MF_SetAmxString(amx, params[p], e.second.data(), strlen(e.second.data()));
			}
		}

		Manager::Instance()->PopThis();

		return returnResult;
	}
}