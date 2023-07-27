#include "oo_manager.h"

#include "oo_class.h"
#include "oo_object.h"

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

    Class* Manager::NewClass(const char *class_name, int32_t version, ke::AString name, Class *super)
    {
        auto in = m_classes.findForAdd(class_name);
        m_classes.add(in, ke::AString(class_name), ke::AutoPtr<Class>(new Class(version, name, super)));
        return in->value;
    }

    ObjectHash Manager::NewObject(Class *isa)
    {
        Object *obj = new Object();
        obj->isa = isa;
        obj->vars.init();
        Class *current = isa;

        while (current != nullptr)
        {
            for (auto iter = current->vars.iter(); !iter.empty(); iter.next())
            {
                auto &&v_name = iter->key;
                auto &&v_size = iter->value;

                auto in = obj->vars.findForAdd(v_name.chars());
                if (!in.found())
                {
                    Var var;
                    var.resize(v_size);
                    obj->vars.add(in, v_name, ke::Move(var));
                }
            }

            current = current->super_class;
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
        Class* current = cl;

        while (current != nullptr)
        {
            auto r = current->ctors.find(num_args);
            if (r.found())
                return &r->value;

            current = current->super_class;
        }

        return nullptr;
    }

    const Method* Manager::FindMethod(Class *cl, const char *name) const
    {
        Class* current = cl;

        while (current != nullptr)
        {
            auto r = current->methods.find(name);
            if (r.found())
                return &r->value;

            current = current->super_class;
        }

        return nullptr;
    }

    ObjectHash Manager::GetThis()
    {
        return m_these.peek();
    }

	void Manager::PushThis(ObjectHash next_this)
	{
		m_these.push(next_this);
	}

    ObjectHash Manager::PopThis()
    {
        return m_these.pop();
    }

    void Manager::Clear()
    {
        m_classes.clear();
        m_objects.clear();
        m_these.clear();
    }

    Manager *Manager::Instance()
    {
        static Manager manager;
        return &manager;
    }
}