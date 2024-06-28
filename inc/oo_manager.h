#ifndef OO_MANAGER_H
#define OO_MANAGER_H

#include <amtl/am-autoptr.h>
#include <amtl/am-vector.h>
#include <amtl/am-hashmap.h>
#include <amtl/am-deque.h>

#include "oo_class.h"
#include "oo_object.h"

namespace oo
{
    using ObjectHash = size_t;
    KE_CONSTEXPR ObjectHash OBJ_NULL = 0u;

    struct Object;

    class Manager
    {
    public:
        Manager();
        ~Manager();

        Class*      NewClass(const char* class_name, int32_t version, ke::Vector<Class *> *supers);
        ObjectHash  NewObject(Class* isa);
        void        DeleteObject(ObjectHash object_hash);

        Class*      ToClass(const char* class_name)     const;
        Object*     ToObject(ObjectHash object_hash)    const;

        const Ctor*     FindCtor(Class* cl, int num_args)   const;
        const Method*   FindMethod(Class* cl, const char* name) const;
        Var*            FindVar(Object *obj, const char *name)  const;

        ObjectHash  GetThis();
        void        PushThis(ObjectHash next_this);
        void  PopThis();

        ke::HashMap<ke::AString, ke::AutoPtr<Class>, StringPolicy> &GetClasses() 
        {
            return m_classes;
        }

        ke::HashMap<uint32_t, ke::AutoPtr<Object>, IntegerPolicy> &GetObjects() 
        {
            return m_objects;
        }

        void Clear();

        static Manager *Instance();

    private:
        ke::HashMap<ke::AString, ke::AutoPtr<Class>, StringPolicy>  m_classes;
        ke::HashMap<uint32_t, ke::AutoPtr<Object>, IntegerPolicy>   m_objects;
        ke::Deque<uint32_t> m_these;
    };
}

#endif