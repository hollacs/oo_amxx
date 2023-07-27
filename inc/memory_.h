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

#ifndef MEMORY__H
#define MEMORY__H

#include <am-vector.h>
#include <MemoryUtils.h>

#if defined(LINUX) || defined(OSX)

	#include <sys/mman.h>
	#include <sys/types.h>
	#include <unistd.h>

	const int PAGE_READONLY          = PROT_READ;
	const int PAGE_EXEC              = PROT_EXEC;
	const int PAGE_WRITE             = PROT_WRITE;
	const int PAGE_EXECUTE_READWRITE = PROT_READ | PROT_WRITE | PROT_EXEC;
	const int PAGE_EXECUTE_READ      = PROT_READ | PROT_EXEC;
	const int PAGE_READWRITE         = PROT_READ | PROT_WRITE;
	const int PageSize               = 4096;

	inline void* Align(void* address)
	{
		return (void*)((long)address & ~(PageSize - 1));
	}

	struct MemoryMap
	{
		long start_address;
		long end_address;
		int memory_protection;

		MemoryMap(long start_address=0, long end_address=0, long memory_protection=0) :
			start_address(start_address),
			end_address(end_address),
			memory_protection(memory_protection)
		{
		}
	};

#endif

class Memory
{
	#if defined(LINUX) || defined(OSX)

		ke::Vector<MemoryMap> memory_maps;

		void fill_memory_maps()
		{
			pid_t pid = getpid();
			char file[255];
			char buffer[2048];
			snprintf(file, sizeof(file) - 1, "/proc/%d/maps", pid);
			FILE *fp = fopen(file, "rt");

			char memory_prot[10];

			if (fp)
			{
				void *start=NULL;
				void *end=NULL;

				while (!feof(fp))
				{
					fgets(buffer, sizeof(buffer) - 1, fp);
					#if defined AMD64
						sscanf(buffer, "%p-%p %s", &start, &end, memory_prot);
					#else
						sscanf(buffer, "%p-%p %s", &start, &end, memory_prot);
					#endif
					int protection = 0;

					for (int i = 0; i < 4; i++)
					{
						char c = memory_prot[i];

						if (c == 'r')
							protection |= PROT_READ;
						if (c == 'w')
							protection |= PROT_WRITE;
						if (c == 'x')
							protection |= PROT_EXEC;
					}

					memory_maps.append(MemoryMap((long)start, (long)end, protection));
				}

				fclose(fp);
			}
		}

	#endif

	public:

		Memory()
		{
			#if defined(LINUX) || defined(OSX)
				fill_memory_maps();
			#endif
		}

		int get_memory_protection(long address)
		{
			#if defined(LINUX) || defined(OSX)
				for (size_t i = 0; i < this->memory_maps.length(); ++i)
				{
					MemoryMap memory_map = memory_maps[i];

					if ((address >= memory_map.start_address) && (address >= memory_map.end_address))
					{
						return memory_map.memory_protection;
					}
				}

				return 0;
			#else
				MEMORY_BASIC_INFORMATION Buffer;

				VirtualQuery(LPCVOID(address), &Buffer, sizeof(MEMORY_BASIC_INFORMATION));

				return Buffer.Protect;
			#endif
		}

		int set_memory_protection(long address, int protection)
		{
			#if defined(LINUX) || defined(OSX)
				return this->set_memory_protection(address, protection, sysconf(_SC_PAGESIZE));
			#else
				MEMORY_BASIC_INFORMATION Buffer;

				VirtualQuery(LPCVOID(address), &Buffer, sizeof(MEMORY_BASIC_INFORMATION));

				SIZE_T size = Buffer.RegionSize;

				return this->set_memory_protection(address, protection, size);
			#endif
		}

		int set_memory_protection(long address, int protection, int size)
		{
			#if defined(LINUX) || defined(OSX)
				void* alignedAddress = Align((void*)address);
				return !mprotect(alignedAddress, size, protection);
			#else
				static DWORD oldProtection;

				return VirtualProtect((LPVOID)address, size, protection, &oldProtection);
			#endif
		}

		void make_writable_executable(long address)
		{
			set_memory_protection(address, PAGE_EXECUTE_READWRITE);
		}

		void make_writable_executable(long address, int size)
		{
			set_memory_protection(address, PAGE_EXECUTE_READWRITE, size);
		}

		void make_writable(long address)
		{
			set_memory_protection(address, PAGE_EXECUTE_READWRITE);
		}

		void make_writable(long address, int size)
		{
			set_memory_protection(address, PAGE_EXECUTE_READWRITE, size);
		}

		void make_executable(long address)
		{
			set_memory_protection(address, PAGE_EXECUTE_READ);
		}

		void make_executable(long address, int size)
		{
			set_memory_protection(address, PAGE_EXECUTE_READ, size);
		}

		int convert_to_sys(int protection)
		{
			if (protection & (2 | 4))
			{
				return PAGE_EXECUTE_READWRITE;
			}
			else if (protection & 4)
			{
				return PAGE_EXECUTE_READ;
			}
			else if (protection & 2)
			{
				return PAGE_READWRITE;
			}
			else
			{
				return PAGE_READONLY;
			}

			return 0;
		}

		int convert_from_sys(int sys_protection)
		{
			switch (sys_protection)
			{
				case PAGE_READONLY:
					return 1;

				case PAGE_READWRITE:
					return 1 | 2;

				case PAGE_EXECUTE_READ:
					return 1 | 4;

				case PAGE_EXECUTE_READWRITE:
					return 1 | 2 | 4;
			}

			return 1;
		}
};

#endif // MEMORY__H
