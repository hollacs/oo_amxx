#include "oo_utils.h"
#include <amtl/am-uniqueptr.h>
#include "assembly_create.h"
#include "memory_.h"
#include "oo_defs.h"

namespace oo { 
namespace util
{
	int clamp(int value, int lower, int upper)
    {
        return value < lower ? lower : (value > upper ? upper : value);
    }
}}