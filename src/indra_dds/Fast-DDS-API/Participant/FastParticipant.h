#pragma once

#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>

#include <Core/Participant/IParticipant.h>
#include "FastParticipantQoS.h"
#include <Configuration/DomainParticipantQosConfig.h>


namespace eprosima { namespace fastdds { namespace dds { 
    class DomainParticipant; 
    class DomainParticipantQos; 
}}}

namespace indradds{

    class IPublisher;
    class ISubscriber;
    class ITopic;


    class FastParticipant : public IParticipant
    {
    public:

        FastParticipant();
        ~FastParticipant();

        void update() override;

        void enable() override;
        void disable() override;
        inline bool is_enabled() const override { return enabled; }

        // Extend with <T> Factories
        IPublisher*     create_pub()    override;
        ISubscriber*    create_sub(ITopic& topic, ITypeSupport& type)    override;
        ITopic*         create_topic(const std::string& topic_name, const std::string& type_name)  override;

        
        /// @brief Accessor to Participant Quality of Service editor
        inline FastParticipantQoS* qos() { return qos_;}

        virtual void intitialize_qos();

        /// @brief It is needed to apply QoS manually after changing properties
        virtual void apply_qos() override;
        virtual void apply_qos(eprosima::fastdds::dds::DomainParticipantQos& qos);

        const  eprosima::fastdds::dds::DomainParticipantQos& get_qos();

    private:
        virtual void on_init() override;
        virtual void on_destroy() override;

        eprosima::fastdds::dds::DomainParticipant* participant_ = nullptr;
        eprosima::fastdds::dds::DomainParticipantQos participant_qos;
        FastParticipantQoS* qos_;

        ParticipantResourcelimitsConfig resource_limit_config;

    };
}

