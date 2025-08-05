#include <IQoS/IPropertyQoS.h>
#include <IQoS/IUserDataQoS.h>
#include <IQoS/IAutoEnableFactoryQoS.h>

#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>


namespace indradds
{
    class FastParticipantQoS : IUserDataQoS, IAutoEnableFactoryQoS, IPropertyQoS
    {

    public:

        FastParticipantQoS(eprosima::fastdds::dds::DomainParticipantQos* part_qos):
        participant_qos(part_qos){}


        // Participant Quality of Services:
        // https://fast-dds.docs.eprosima.com/en/stable/fastdds/dds_layer/domain/domainParticipant/domainParticipant.html#
        
        // User Data (Mutable)
        virtual void set_user_data(std::vector<u_int8_t>& data);

        // Auto-Enable Factory (Mutable)
        virtual void set_auto_enable_entities(bool enable);

        // Resource Limit (allocation)
        virtual void set_resource_limits();

        // Properties
        virtual void set_property(const std::string& key, const std::string& value);

        // Wire Protocol

        // Transport Config

        // Flow Controller 

        // Thread Settings

    private:
        eprosima::fastdds::dds::DomainParticipantQos* participant_qos;

    };
}