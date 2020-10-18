#ifndef FALCON_FALI_TYPE_FLAGS_HPP
#define FALCON_FALI_TYPE_FLAGS_HPP

#include "common/Common.hpp"

namespace Falcon
{
    namespace FALI
    {
        using TypeFlag = uint8_t;

        namespace TypeFlags
        {
            const TypeFlag CONST = 0b10000000;
            const TypeFlag TEMPARORY = 0b01000000;
            const TypeFlag REFERENCE = 0b00100000;
        }
    }
}

#endif