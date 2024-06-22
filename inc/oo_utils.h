#ifndef OO_UTILS_H
#define OO_UTILS_H

#include <amtl/am-vector.h>
#include "amxxmodule.h"

namespace oo {
namespace util
{
    using AmxxForward = int32_t;
    using ArgList = ke::Vector<int8_t>;
    using HookChain = ke::Vector<AmxxForward>;
    using ObjectHash = size_t;

    AmxxForward AddMethod(AMX *amx, const char *callback, const ArgList *args=nullptr);
    cell ExecuteMethod(AMX *amx, cell *params, AmxxForward forward_id, const ArgList* args=nullptr, int8_t start_param=0);
    cell ExecuteMethodHookChain(AMX *amx, cell *params, const HookChain *hook_chain, const ArgList *args=nullptr, int8_t start_param=0);

    template<typename T>
    T clamp(T value, T lower, T upper)
    {
        return value < lower ? lower : (value > upper ? upper : value);
    }
}}

#endif // OO_UTILS_H