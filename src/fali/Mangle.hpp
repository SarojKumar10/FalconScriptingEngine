#ifndef FALCON_FALI_MANGLE_HPP
#define FALCON_FALI_MANGLE_HPP

#include <string>
#include <vector>

namespace Falcon
{
    namespace FALI
    {
        std::string MangleFunction(const std::string & name, std::vector<std::string> params);
        std::string DemangleFunction(const std::string & name);

        std::string MangleIdentifier(const std::string & name);
        std::string DemangleIdentifier(const std::string & name);
    }
}

#endif