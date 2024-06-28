#ifndef OO_FORWARD_H
#define OO_FORWARD_H

#include <amtl/am-vector.h>
#include <amtl/am-deque.h>
#include <amtl/am-autoptr.h>
#include "amxxmodule.h"

namespace oo
{
	using AmxxForward = int32_t;
	using ArgTypeList = ke::Vector<int>;
	using HookChain = ke::Vector<AmxxForward>;
	using ObjectHash = size_t;

	class ArgData
	{
	public:
		enum Type
		{
			TypeNull = 0,
			TypeCell,
			TypeString,
			TypeArray,
			TypePtr
		};

		struct StringData
		{
			ke::UniquePtr<char[]> data;
			size_t size;
		};

		struct ArrayData
		{
			ke::UniquePtr<cell[]> data;
			size_t size;
		};

		ArgData(cell c)
		{
			data = new cell(c);
			type = TypeCell;
		}

		ArgData(char* s, size_t len)
		{
			auto chars = ke::MakeUnique<char[]>(len + 1);
			memcpy(chars.get(), s, len);
			chars[len] = '\0';
			data = new StringData{ke::Move(chars), len};
			type = TypeString;
		}

		ArgData(cell* a, size_t size)
		{
			auto cells = ke::MakeUnique<cell[]>(size);
			memcpy(cells.get(), a, size * sizeof(cell));
			data = new ArrayData{ke::Move(cells), size};
			type = TypeArray;
		}

		ArgData(cell *ptr)
		{
			data = ptr;
			type = TypePtr;
		}

		ArgData()
		{
			data = nullptr;
			type = TypeNull;
		}

		void FreeData()
		{
			switch (type)
			{
			case TypeCell:
				delete static_cast<cell*>(data);
				break;

			case TypeString:
				delete static_cast<StringData*>(data);
				break;

			case TypeArray:
				delete static_cast<ArrayData*>(data);
				break;
			}

			data = nullptr;
			type = TypeNull;
		}

		cell *ToCell()
		{
			return static_cast<cell*>(data);
		}

		char* ToString()
		{
			return static_cast<StringData*>(data)->data.get();
		}

		size_t GetStringSize()
		{
			return static_cast<StringData*>(data)->size;
		}

		StringData *ToStringData()
		{
			return static_cast<StringData*>(data);
		}

		cell* ToArray()
		{
			return static_cast<ArrayData*>(data)->data.get();
		}

		size_t GetArraySize()
		{
			return static_cast<ArrayData*>(data)->size;
		}

		ArrayData *ToArrayData()
		{
			return static_cast<ArrayData*>(data);
		}

		void *data;
		Type type;
	};

	using ArgDataList = ke::Vector<ArgData>;

	struct CallStack
	{
		cell result;
		cell state;
		ObjectHash _this;
		const ArgTypeList *arg_list;
		ArgDataList arg_data;
	};

	class Forward
	{
	public:
		Forward(AMX *amx, cell *params, const HookChain *pre, const HookChain *post, ObjectHash _this, const ArgTypeList* args, int start_param) 
			: m_amx(amx), m_params(params), m_start_param(start_param), m_arg_list(args), m_pre(pre), m_post(post), m_this(_this)
		{}

		static AmxxForward Create(AMX *amx, const char *callback, const ArgTypeList *args=nullptr);
		static ke::Deque<CallStack> *GetCallStack();
		static void ClearCallStack();
		static void PushCall(ObjectHash hash, const ArgTypeList *args);
		static void PopCall();
		static CallStack *GetCall();

		void CollectArgDataList(ArgDataList *data);
		cell Execute(AmxxForward fwd);
		cell ExecuteHooks(const HookChain *hooks);
		cell Call(AmxxForward fwd);
		
	private:
		AMX *m_amx;
		cell *m_params;
		int m_start_param;
		const ArgTypeList *m_arg_list;
		const HookChain *m_pre;
		const HookChain *m_post;
		ObjectHash m_this;
	};
}

#endif