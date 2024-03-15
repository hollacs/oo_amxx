#include "oo_manager.h"

#include "oo_class.h"
#include "oo_object.h"
#include <string.h>
#include "amxxmodule.h"

namespace oo
{
    Manager::Manager()
    {
        m_classes.init();
        m_objects.init();
    }

    Manager::~Manager()
    {
        Clear();
    }

    Class* Manager::NewClass(const char *class_name, int32_t version, ke::Vector<Class *> *supers)
    {
        auto in = m_classes.findForAdd(class_name);
        m_classes.add(in, ke::AString(class_name), new Class(version, class_name, supers));
        return in->value;
    }

    ObjectHash Manager::NewObject(Class *isa)
    {
        Object *obj = new Object();
        obj->isa = isa;
        obj->vars.init();

        for (size_t i = 0; i < isa->mro.length(); i++)
        {
            auto current = isa->mro[i];

            for (auto iter = current->vars.iter(); !iter.empty(); iter.next())
            {
                ke::AString v_name;
                v_name.format("%s@%s", current->name.chars(), iter->key.chars());
                int8_t v_size = iter->value;

                auto in = obj->vars.findForAdd(v_name.chars());
                if (!in.found())
                {
                    Var var;
                    var.resize(v_size);
                    obj->vars.add(in, v_name, ke::Move(var));
                }
            }
        }

        auto obj_hash = ke::HashPointer(obj);
        auto in = m_objects.findForAdd(obj_hash);
        m_objects.add(in, obj_hash, ke::Move(obj));

        return obj_hash;
    }

    void Manager::DeleteObject(ObjectHash object_hash)
    {
        auto r = m_objects.find(object_hash);
        m_objects.remove(r);
    }

    Class* Manager::ToClass(const char *class_name) const
    {
        auto r = m_classes.find(class_name);
        if (!r.found())
            return nullptr;
        
        return r->value.get();
    }

    Object* Manager::ToObject(ObjectHash object_hash) const
    {
        auto r = m_objects.find(object_hash);
        if (!r.found())
            return nullptr;
        
        return r->value.get();
    }

    const Ctor* Manager::FindCtor(Class *cl, uint8_t num_args) const
    {
        for (size_t i = 0; i < cl->mro.length(); i++)
        {
            auto current = cl->mro[i];

            auto r = current->ctors.find(num_args);
            if (r.found())
                return &r->value;
        }

        return nullptr;
    }

    const Method* Manager::FindMethod(Class *cl, const char *name) const
    {
        for (size_t i = 0; i < cl->mro.length(); i++)
        {
            auto current = cl->mro[i];

            auto r = current->methods.find(name);
            if (r.found())
                return &r->value;
        }

        return nullptr;
    }

    Var* Manager::FindVar(Object *obj, const char *name) const
    {
        auto isa = obj->isa;
        if (strchr(name, '@') == NULL)
        {
            ke::AString _name;
            for (size_t i = 0; i < isa->mro.length(); i++)
            {
                auto current = isa->mro.at(i);
                _name.format("%s@%s", current->name.chars(), name);

                auto r = obj->vars.find(_name.chars());
                if (r.found())
                    return &r->value;
            }
        }
        else
        {
            auto r = obj->vars.find(name);
            if (r.found())
                return &r->value;
        }

        return nullptr;
    }

    ObjectHash Manager::GetThis()
    {
        return m_these.back();
    }

	void Manager::PushThis(ObjectHash next_this)
	{
		m_these.append(next_this);
	}

    void Manager::PopThis()
    {
        m_these.popBack();
    }

    void Manager::Clear()
    {
        m_classes.clear();
        m_objects.clear();

        while (!m_these.empty())
            m_these.popBack();
    }

    Manager *Manager::Instance()
    {
        static Manager manager;
        return &manager;
    }
}