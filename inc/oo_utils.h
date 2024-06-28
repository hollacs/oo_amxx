#ifndef OO_UTILS_H
#define OO_UTILS_H

#include <amtl/am-vector.h>
#include <amtl/am-autoptr.h>
#include "amxxmodule.h"

namespace oo {
namespace util
{
    int clamp(int value, int lower, int upper);
}}

#endif // OO_UTILS_H