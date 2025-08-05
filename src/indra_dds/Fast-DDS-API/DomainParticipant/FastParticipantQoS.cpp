#include "FastParticipantQoS.h"

#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>

using namespace indraDDS;


void FastParticipantQoS::set_user_data(std::vector<u_int8_t>& data)
{
    std::vector<eprosima::fastdds::rtps::octet> octet_data(data.begin(), data.end());
    participant_qos->user_data().data_vec(octet_data);
}

void FastParticipantQoS::set_auto_enable_entities(bool value)
{
    participant_qos->entity_factory().autoenable_created_entities = value;
}

void FastParticipantQoS::set_property(const std::string &key, const std::string &value)
{
    participant_qos->properties().properties().emplace_back(key, value);
}


void indraDDS::FastParticipantQoS::set_resource_limits()
{
    auto& allocation = participant_qos->allocation();

    // fill with ParticipantResourcelimitsConfig
}

