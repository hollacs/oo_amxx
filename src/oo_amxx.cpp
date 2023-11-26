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

#include <unordered_set>
#include <queue>

#include "amxxmodule.h"
#include "oo_class.h"
#include "oo_manager.h"
#include "oo_natives.h"

/** AMXX attach
 * Do native functions init here (MF_AddNatives)
 */
void OnAmxxAttach(void)
{
	static const AMX_NATIVE_INFO oo_natives[] =
	{
		{ "oo_class",	oo::natives::native_decl_class },
		{ "oo_ctor",	oo::natives::native_decl_ctor },
		{ "oo_dtor",	oo::natives::native_decl_dtor },
		{ "oo_mthd",	oo::natives::native_decl_msg },
		{ "oo_smthd",	oo::natives::native_decl_static_msg },
		{ "oo_var",		oo::natives::native_decl_ivar },

		{ "oo_isa",			oo::natives::native_isa },
		{ "oo_subclass_of",	oo::natives::native_subclass_of	},

		{ "oo_new",			oo::natives::native_new },
		{ "oo_delete",		oo::natives::native_delete },
		{ "oo_call",		oo::natives::native_send },
		{ "oo_get",			oo::natives::native_read },
		{ "oo_set",			oo::natives::native_write },
		//{ "oo_get_str",		oo::natives::native_read_str },
		//{ "oo_set_str",		oo::natives::native_write_str },

		{ "oo_this",		oo::natives::native_this },

		{ "oo_this_ctor",	oo::natives::native_this_ctor },
		{ "oo_super_ctor",	oo::natives::native_super_ctor },

		{ "oo_class_exists",	oo::natives::native_class_exists },
		{ "oo_object_exists",	oo::natives::native_object_exists },
		{ "oo_get_classname",	oo::natives::native_get_class_name },

		{ nullptr, nullptr }
	};
	MF_AddNatives(oo_natives);
}


/** All plugins loaded
 * Do forward functions init here (MF_RegisterForward)
 */
void OnPluginsLoaded(void)
{
	//::MessageBoxA(nullptr, "0", "", MB_OK);

	MF_ExecuteForward(
		MF_RegisterForward(
			"oo_init",
			ForwardExecType::ET_IGNORE,
			ForwardParam::FP_DONE)
	);

	//::MessageBoxA(nullptr, "1", "", MB_OK);

	MF_PrintSrvConsole("[%s] Classes and their methods:\n", MODULE_LOGTAG);
	for (auto &&cl : oo::Manager::Instance()->GetClasses())
	{
		//::MessageBoxA(nullptr, "a", cl.first.c_str(), MB_OK);
		MF_PrintSrvConsole("    %s (#%p)\n", cl.first.c_str(), static_cast<void *>(cl.second.get()));
		{
			int extra_tabs = 3;

			if (cl.second->super_classes.size() > 0)
				MF_PrintSrvConsole("%s-> ", std::string(extra_tabs * 4, ' ').c_str());
			
			for (size_t i = 0; i < cl.second->super_classes.size(); i++)
			{
				auto derived = cl.second->super_classes[i].lock();
				if (i > 0)
					MF_PrintSrvConsole(", ");

				MF_PrintSrvConsole("%s (#%p)", derived->name.c_str(), static_cast<void*>(derived.get()));
			}

			MF_PrintSrvConsole("\n");

			/*
			std::shared_ptr<oo::Class> pderived = cl.second->super_class.lock();
			while (pderived != nullptr)
			{
				MF_PrintSrvConsole("%s-> (#%p)\n", std::string(extra_tabs++ * 4, ' ').c_str(), static_cast<void*>(pderived.get()));
				pderived = pderived->super_class.lock();
			}*/
		}

		//::MessageBoxA(nullptr, "b", cl.first.c_str(), MB_OK);

		MF_PrintSrvConsole("         <Ctors>\n");
		for (auto &&ct : cl.second->ctors)
		{
			MF_PrintSrvConsole("              %s@%d(", cl.first.c_str(), ct.first);
			int count = 0;
			for (auto && arg : ct.second.args)
			{
				if (count++ > 0) MF_PrintSrvConsole(", ");
				MF_PrintSrvConsole(std::to_string(arg).c_str());
			}
			MF_PrintSrvConsole(")\n");
		}

		MF_PrintSrvConsole("         <Methods>\n");
		for (auto &&m : cl.second->methods)
		{
			MF_PrintSrvConsole("              %s@%s(", cl.first.c_str(), m.first.c_str());
			int count = 0;
			for (auto && arg : m.second.args)
			{
				if (count++ > 0) MF_PrintSrvConsole(", ");
				MF_PrintSrvConsole(std::to_string(arg).c_str());
			}
			MF_PrintSrvConsole(")\n");
		}

		//::MessageBoxA(nullptr, "c", cl.first.c_str(), MB_OK);

		MF_PrintSrvConsole("         <IVars>\n");
		for (auto &&iv : cl.second->ivars)
			MF_PrintSrvConsole("              %s@%s[%d]\n", cl.first.c_str(), iv.first.c_str(), iv.second);

		//::MessageBoxA(nullptr, "d", cl.first.c_str(), MB_OK);
	}
}

void OnPluginsUnloaded()
{
	oo::Manager::Instance()->Clear();
	//MF_PrintSrvConsole("OO Release memory\n");
}

void OnAmxxDetach()
{
}