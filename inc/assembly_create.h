//
// AMX Mod X, based on AMX Mod by Aleksander Naszko ("OLO").
// Copyright (C) The AMX Mod X Development Team.
//
// This software is licensed under the GNU General Public License, version 3 or higher.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     https://alliedmods.net/amxmodx-license

//
// Okapi Module
//

#ifndef ASSEMBLY_CREATE_H
#define ASSEMBLY_CREATE_H

#include "am-vector.h"
#include <memory.h>

class InstructionBytes
{
	protected:

		unsigned char* bytes;

	public:

		InstructionBytes(unsigned char* bytes) : bytes(bytes)
		{
			this->bytes = bytes;
		}

		virtual void set_bytes(unsigned char* bytes)
		{
			this->bytes = bytes;
		}

		virtual ~InstructionBytes(){};
};

template <class T>
class Instruction : public InstructionBytes
{
	public:

		virtual ~Instruction(){}

		Instruction(unsigned char* bytes) : InstructionBytes(bytes)
		{
		}

		static void get_bytes(ke::Vector<unsigned char>& bytes)
		{
			for (size_t i = 0; i < T::size; i++)
			{
				bytes.append(T::data[i]);
			}
		}

		void set_long(long value)
		{
			//assert(sizeof(T::size) >= 5);

			*((long*)(&this->bytes[1])) = value;
		}
};

#define INSTRUCTION(T) class T : public Instruction<T>{ public: static unsigned char data[]; static size_t size; T(unsigned char* bytes) : Instruction(bytes) {};

INSTRUCTION(Inst_Mov_EDX_VAL) };
INSTRUCTION(Inst_Mov_EAX_VAL) };
INSTRUCTION(Inst_Mov_ECX_VAL) };

INSTRUCTION(Inst_Mov_ptrEDX_EAX) };
INSTRUCTION(Inst_Mov_ptrEDX_ECX) };
INSTRUCTION(Inst_Mov_ptrEDXpVAL_EAX)

void set_inc(unsigned char value)
{
	*((unsigned char*)(&this->bytes[2])) = value;
}
};
INSTRUCTION(Inst_Mov_ptrEDXpVAL_ECX)

void set_inc(unsigned char value)
{
	*((unsigned char*)(&this->bytes[2])) = value;
}
};
INSTRUCTION(Inst_Pop_EDX) };
INSTRUCTION(Inst_Push_ECX) };
INSTRUCTION(Inst_Push_ESP) };
INSTRUCTION(Inst_Push_VAL) };
INSTRUCTION(Inst_Push_EDX) };
INSTRUCTION(Inst_Enter) };
INSTRUCTION(Inst_Call)

void set_address(long address)
{
	*((long*)(&this->bytes[1])) = (long)address - ((long)this->bytes + 5);
}
};

INSTRUCTION(Inst_Leave) };
INSTRUCTION(Inst_Add_ESP_Val)

void set_inc(unsigned char value)
{
	*((unsigned char*)(&this->bytes[2])) = value;
}
};

INSTRUCTION(Inst_Mov_EDX_ptrESPpVAL)

void set_inc(unsigned char value)
{
	*((unsigned char*)(&this->bytes[3])) = value;
}
};
INSTRUCTION(Inst_Mov_ptrESP_EDX) };
INSTRUCTION(Inst_Ret) };
INSTRUCTION(Inst_RetN)

void set_count(unsigned char value)
{
	*((unsigned char*)(&this->bytes[1])) = value;
}
};

INSTRUCTION(Inst_Fst)

void set_ptr(long value)
{
	*((long*)(&this->bytes[2])) = value;
}
};

INSTRUCTION(Inst_Fstp)

void set_ptr(long value)
{
	*((long*)(&this->bytes[2])) = value;
}
};

INSTRUCTION(Inst_Fld)

void set_ptr(long value)
{
	*((long*)(&this->bytes[2])) = value;
}
};

class AssemblyCreate
{
	private:

		struct InstructionBytesOffset
		{
			InstructionBytes* instruction_bytes;
			size_t offset;

			InstructionBytesOffset() { }
			InstructionBytesOffset(InstructionBytes* instruction_bytes, size_t offset) : instruction_bytes(instruction_bytes), offset(offset) { }
		};

		ke::Vector<unsigned char> bytes;
		ke::Vector<InstructionBytesOffset> instructionsBytesData;

	public:

		~AssemblyCreate()
		{
			for (size_t i = 0; i < instructionsBytesData.length(); i++)
			{
				InstructionBytesOffset instructionBytesData = instructionsBytesData[i];

				delete instructionBytesData.instruction_bytes;
			}
		}

		template <class T>
		T* add()
		{
			int pos = this->bytes.length();


			ke::Vector < unsigned char > bytes;
			T::get_bytes(bytes);

			for (size_t i = 0; i < bytes.length(); ++i)
			{
				this->bytes.append(bytes.at(i));
			}

			T* object = new T(&this->bytes.buffer()[pos]);

			instructionsBytesData.append(InstructionBytesOffset(object, pos));

			return object;
		}

		unsigned char* create_block()
		{
			unsigned char* block = new unsigned char[bytes.length()];
			memcpy(block, this->bytes.buffer(), bytes.length());

			for (size_t i = 0; i < instructionsBytesData.length(); i++)
			{
				InstructionBytesOffset instructionBytesData = instructionsBytesData[i];
				instructionBytesData.instruction_bytes->set_bytes(&block[instructionBytesData.offset]);
			}

			return block;
		}

		unsigned char* get_block()
		{
			for (size_t i = 0; i < instructionsBytesData.length(); i++)
			{
				InstructionBytesOffset instructionBytesData = instructionsBytesData[i];
				instructionBytesData.instruction_bytes->set_bytes(&this->bytes.buffer()[instructionBytesData.offset]);
			}

			return this->bytes.buffer();
		}

		size_t size()
		{
			return this->bytes.length();
		}
};

#endif // ASSEMBLY_CREATE_H

