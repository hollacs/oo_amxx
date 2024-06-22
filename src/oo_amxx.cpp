#include "amxxmodule.h"
#include "oo_class.h"
#include "oo_manager.h"
#include "oo_natives.h"

void OnAmxxAttach(void)
{
	static const AMX_NATIVE_INFO oo_natives[] =
	{
		{ "oo_class",	oo::native::native_class },
		{ "oo_ctor",	oo::native::native_ctor },
		{ "oo_dtor",	oo::native::native_dtor },
		{ "oo_mthd",	oo::native::native_mthd },
		{ "oo_smthd",	oo::native::native_smthd },
		{ "oo_var",		oo::native::native_var },

		{ "oo_hook_mthd", oo::native::native_hook_mthd },
		{ "oo_hook_set_return", oo::native::native_hook_set_return },

		{ "oo_isa",			oo::native::native_isa },
		{ "oo_subclass_of",	oo::native::native_subclass_of	},

		{ "oo_new",			oo::native::native_new },
		{ "oo_delete",		oo::native::native_delete },
		{ "oo_call",		oo::native::native_call },
		{ "oo_get",			oo::native::native_get },
		{ "oo_set",			oo::native::native_set },

		{ "oo_this",		oo::native::native_this },

		{ "oo_super_ctor",	oo::native::native_super_ctor },

		{ "oo_class_exists",	oo::native::native_class_exists },
		{ "oo_object_exists",	oo::native::native_object_exists },
		{ "oo_get_classname",	oo::native::native_get_classname },

		{ nullptr, nullptr }
	};
	MF_AddNatives(oo_natives);
}

void OnPluginsLoaded(void)
{
	//::MessageBoxA(nullptr, "0", "", MB_OK);

	MF_ExecuteForward(
		MF_RegisterForward(
			"oo_init",
			ForwardExecType::ET_IGNORE,
			ForwardParam::FP_DONE)
	);
	/*
	MF_PrintSrvConsole("[%s] Classes and their methods and variables:\n", MODULE_LOGTAG);
	for (auto iter = oo::Manager::Instance()->GetClasses().iter(); !iter.empty(); iter.next())
	{
        auto cl = iter;
		MF_PrintSrvConsole("    %s (#%p)\n", cl->key.chars(), cl->value.get());
		
		if (cl->value->super_classes.length() > 0)
			MF_PrintSrvConsole("            -> ");

		for (size_t i = 0; i < cl->value->super_classes.length(); i++)
		{
			auto derived = cl->value->super_classes[i];
			if (i > 0)
				MF_PrintSrvConsole(", ");

			MF_PrintSrvConsole("%s (#%p)", derived->name.chars(), derived);
		}

		MF_PrintSrvConsole("\n");
	
		MF_PrintSrvConsole("         <Ctors>\n");
		for (auto iter = cl->value->ctors.iter(); !iter.empty(); iter.next())
		{
            auto ct = iter;
			MF_PrintSrvConsole("              %s@%d(", cl->key.chars(), ct->key);
			int count = 0;
			for (auto && arg : ct->value.args)
			{
				if (count++ > 0) MF_PrintSrvConsole(", ");
				MF_PrintSrvConsole("%d", arg);
			}
			MF_PrintSrvConsole(")\n");
		}

		MF_PrintSrvConsole("         <Methods>\n");
		for (auto iter = cl->value->methods.iter(); !iter.empty(); iter.next())
		{
			auto m = iter;
			MF_PrintSrvConsole("              %s@%s(", cl->key.chars(), m->key.chars());
			int count = 0;
			for (auto && arg : m->value.args)
			{
				if (count++ > 0)
					MF_PrintSrvConsole(", ");

				MF_PrintSrvConsole("%d", arg);
			}
			MF_PrintSrvConsole(")\n");
		}

		MF_PrintSrvConsole("         <Vars>\n");
		for (auto iter = cl->value->vars.iter(); !iter.empty(); iter.next())
		{
			auto v = iter;
			MF_PrintSrvConsole("              %s@%s[%d]\n", cl->key.chars(), v->key.chars(), v->value);
		}
	}*/
}

void OnPluginsUnloaded()
{
	oo::Manager::Instance()->Clear();
}

void OnAmxxDetach()
{
}