#pragma once
#include <vector>
#include <stdint.h>
#include <string>

namespace indradds {

    class IUserDataQoS
    {
    public:
        virtual ~IUserDataQoS() = default;

        virtual void set_user_data(std::vector<uint8_t>& data) = 0;    
        virtual void set_user_data(std::string text) = 0;
    };
        

}
       
      
