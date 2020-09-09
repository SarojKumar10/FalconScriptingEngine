#include "Attribute.hpp"

namespace Falcon
{
    namespace FALI
    {
        std::unordered_map<std::string, std::vector<std::string>> AttributeTable::s_Attributes;

        void AttributeTable::New(const std::string & name, std::vector<std::string> attribs)
        {
            s_Attributes[name] = attribs;
        }

        std::vector<std::string> & AttributeTable::GetAttributes(const std::string & name)
        {
            return s_Attributes[name];
        }

        std::string AttributeTable::GetFunctionWithAttribute(const std::string & attrib)
        {
            std::string function;

            for (auto & p : s_Attributes)
            {
                if (std::find(p.second.begin(), p.second.end(), attrib) != p.second.end())
                {
                    function = p.first;
                }
            }

            return std::move(function);
        }
    }
}