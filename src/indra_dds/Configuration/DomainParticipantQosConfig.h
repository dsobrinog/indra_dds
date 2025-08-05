
#pragma once

#include <vector>
#include <unordered_set>
#include <string>
#include <stdint.h>

struct ParticipantResourcelimitsConfig
{

};

enum DomainParticipantQoS
{
    USERDATA = 0,
    AUTO_ENABLE_ENTITIES = 1,
};

class DomainParticipantQosConfig
{   

public:
    std::unordered_set<DomainParticipantQoS> modifiedFields;

    bool is_modified(DomainParticipantQoS field) const
    {
        return modifiedFields.count(field) > 0;
    }

    /////////////////////////
    //  UserDataQosPolicy  //
    /////////////////////////

    
};


// class UserDataQoSPolicy
// {
//     // Collection
//     std::vector<uint8_t> user_data;

// public:

//     void set_user_data(const std::vector<uint8_t>& data)
//     {
//         user_data = data;
//         modifiedFields.insert(DomainParticipantQoS::USERDATA);
//     }

//     const std::vector<uint8_t>& get_user_data() const
//     {
//         return user_data;
//     }
// };