#pragma once
#include <vector>
#include <stdint.h>

namespace indraDDS {

    class IUserDataQoS
    {
    public:
        virtual ~IUserDataQoS() = default;

        virtual void set_user_data(std::vector<uint8_t>& data) = 0;    };

}
       
      
