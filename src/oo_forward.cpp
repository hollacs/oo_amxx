#include "oo_forward.h"

#include "oo_defs.h"
#include <amtl/am-uniqueptr.h>
#include "assembly_create.h"
#include "memory_.h"
//#include <ctime>

namespace oo
{
	AmxxForward Forward::Create(AMX *amx, const char *callback, const ArgTypeList *args)
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

	ke::Deque<CallStack> *Forward::GetCallStack() // static method
	{
		static ke::Deque<CallStack> calls;
		return &calls;
	}

	void Forward::PushCall(ObjectHash hash, const ArgTypeList *args)
	{
		CallStack call;
		call.result = 0;
		call.state = OO_CONTINUE;
		call._this = hash;
		call.arg_list = args;

		GetCallStack()->append(ke::Move(call));
	}

	void Forward::PopCall()
	{
		auto call = GetCall();

		int num_args = (call->arg_list == nullptr) ? 0 : call->arg_list->length();

		for (int i = 0; i < num_args; i++)
		{
			call->arg_data.at(i).FreeData();
		}

		GetCallStack()->popBack();
	}

	CallStack *Forward::GetCall()
	{
		return &GetCallStack()->back();
	}

	void Forward::ClearCallStack()
	{
		while (!GetCallStack()->empty())
		{
			PopCall(); // this will release memory
		}
	}

	void Forward::CollectArgDataList(ArgDataList *data)
	{
		int num_args = (m_arg_list == nullptr) ? 0 : m_arg_list->length();

		for (int i = 0; i < num_args; i++)
		{
			int p = i + m_start_param;

			auto type = m_arg_list->at(i);
			switch (type)
			{
				case OO_CELL:
					data->append(ArgData(MF_GetAmxAddr(m_amx, m_params[p])[0]));
					break;

				case OO_BYREF:
					data->append(ArgData(&MF_GetAmxAddr(m_amx, m_params[p])[0]));
					break;

				case OO_STRING:
				case OO_STRING_EX:
				{
					int len = 0;
					char *str = MF_GetAmxString(m_amx, m_params[p], 0, &len);
					len = (type == OO_STRING_EX && len < 255) ? 255 : len;
					data->append(ArgData(str, (size_t)len));
					break;
				}

				default:
				{
					if (type > OO_CELL)
					{
						cell *arr = MF_GetAmxAddr(m_amx, m_params[p]);
						data->append(ArgData(arr));
					}
					break;
				}
			}
		}
	}

	cell Forward::ExecuteHooks(const HookChain *hooks)
	{
		cell ret = GetCall()->state;
		for (size_t i = 0; i < hooks->length(); i++)
		{
			int r = Execute(hooks->at(i));			
			ret = (r > ret) ? r : ret;
			GetCall()->state = ret;
			if (ret >= OO_BREAK)
				break;
		}

		return ret;
	}

	cell Forward::Call(AmxxForward fwd)
	{
		PushCall(m_this, m_arg_list);
		CollectArgDataList(&GetCall()->arg_data);

		cell result = 0;
		if (ExecuteHooks(m_pre) < OO_SUPERCEDE)
		{
			result = Execute(fwd);
			ExecuteHooks(m_post);
		}
		else
		{
			result = GetCall()->result;
		}

		// this should free memory in arg_data 
		PopCall();
		return result;
	}

	cell Forward::Execute(AmxxForward fwd)
	{
		auto arg_data = &GetCall()->arg_data;

		int num_args = (m_arg_list == nullptr) ? 0 : m_arg_list->length();
		int start = num_args - 1;
		int end = 0;

		AssemblyCreate assembly;
		assembly.add<Inst_Enter>();

		for (int i = start; i >= end; i--)
		{
			int p = i + m_start_param;
			auto type = m_arg_list->at(i);
			auto data = &arg_data->at(i);

			switch (type)
			{
			case OO_CELL:
				assembly.add<Inst_Push_VAL>()->set_long(*data->ToCell());
				break;

			case OO_BYREF:
				assembly.add<Inst_Push_VAL>()->set_long((long)(&*data->ToCell()));
				break;

			case OO_STRING:
			case OO_STRING_EX:
				assembly.add<Inst_Push_VAL>()->set_long((long)(data->ToString()));
				break;

			default:
				if (type > OO_CELL)
					assembly.add<Inst_Push_VAL>()->set_long(MF_PrepareCellArrayA(data->ToCell(), type, true));
				
				break;
			}
		}

		assembly.add<Inst_Push_VAL>()->set_long((long)fwd);

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

		for (int i = 0; i < num_args; i++)
		{
			int p = i + m_start_param;
			auto type = m_arg_list->at(i);
			auto data = &arg_data->at(i);

			switch (type)
			{
			case OO_STRING_EX:
				MF_SetAmxString(m_amx, m_params[p], data->ToString(), data->GetStringSize());
				break;
			}
		}

		return ret;
	}
}

