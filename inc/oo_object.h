#ifndef OO_OBJECT_H
#define OO_OBJECT_H

#include <amtl/am-vector.h>
#include <amtl/am-hashmap.h>
#include "oo_policies.h"
#include "amxxmodule.h"

namespace oo
{
    using Var = ke::Vector<cell>;

    struct Class;

    struct Object
    {
        Class* isa;
        ke::HashMap<ke::AString, Var, StringPolicy> vars;
    };
}

#endif