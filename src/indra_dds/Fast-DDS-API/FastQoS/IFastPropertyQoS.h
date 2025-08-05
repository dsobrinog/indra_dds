#pragma once
#include <string>

#include <IQoS/IPropertyQoS.h>

namespace indraDDS {

    class IFastPropertyQoS : IPropertyQoS
    {
    public:
        virtual ~IFastPropertyQoS() = default;

        virtual void set_property(const std::string& key, const std::string& value) = 0;
    };

}
