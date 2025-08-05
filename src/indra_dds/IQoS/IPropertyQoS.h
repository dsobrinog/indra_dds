#pragma once
#include <string>

namespace indradds {

    class IPropertyQoS
    {
    public:
        virtual ~IPropertyQoS() = default;

        virtual void set_property(const std::string& key, const std::string& value) = 0;
    };

}
