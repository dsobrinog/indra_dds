#pragma once

#ifdef USE_OPEN_DDS

#include <iostream>       
#include "abstraction/IPub.h"
#include "stress_test/DDSParticipantManager.h"


#include "IDLs_open/generated/inse/inseC.h"
#include "IDLs_open/generated/inse/inseTypeSupportImpl.h"

#include <dds/DCPS/DomainParticipantFactoryImpl.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/StaticIncludes.h>

#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/transport/framework/TransportRegistry.h>
#include <dds/DCPS/transport/framework/TransportConfig.h>
#include <dds/DCPS/transport/framework/TransportInst.h>
#include <dds/DCPS/transport/udp/UdpInst.h>
#include <dds/DCPS/transport/udp/UdpTransport.h>

#include "open_dds_defs.h"


class OpenPub : public IPub<::AirEntity>
{
private:
    ::DDS::DomainParticipant_var participant_;
    ::DDS::Publisher_var publisher_;
    ::DDS::Topic_var topic_;
    ::DDS::DataWriter_var writer_;

    ::AirEntityDataWriter_var air_writer_;

    int current_id = 0;
    int min_subs = 0;
    std::atomic_int matched_subs = 0;

    
    class PubListener : public virtual OpenDDS::DCPS::LocalObject<DDS::DataWriterListener>
    {
    public:
        std::atomic_int& matched_;
        PubListener(std::atomic_int& m) : matched_(m) {}

        void on_publication_matched(
            DDS::DataWriter_ptr /*writer*/,
            const DDS::PublicationMatchedStatus& status) override
        {
            matched_ = status.total_count;
            std::cout << "OpenDDS Publisher matched subscribers: " << matched_ << std::endl;
        }

        void on_offered_deadline_missed(
            DDS::DataWriter_ptr, const DDS::OfferedDeadlineMissedStatus&) override {}
        void on_liveliness_lost(
            DDS::DataWriter_ptr, const DDS::LivelinessLostStatus&) override {}
        void on_offered_incompatible_qos(
            DDS::DataWriter_ptr, const DDS::OfferedIncompatibleQosStatus&) override {}
    } listener_{matched_subs};
    
public:
    OpenPub()
    {
        std::cout<< " Open DDS Publisher created!" << std::endl;
    }

    void set_participant_manager(ParticipantManager* participant_manager) override
    {
        participant_ = static_cast<::DDS::DomainParticipant*>(participant_manager->getParticipant());
    }

    ~OpenPub() override
    {
        if (!CORBA::is_nil(writer_))
            publisher_->delete_datawriter(writer_);
        if (!CORBA::is_nil(publisher_))
            participant_->delete_publisher(publisher_);
        if (!CORBA::is_nil(topic_))
            participant_->delete_topic(topic_);
    }

    bool init() override
    {
        if (CORBA::is_nil(participant_)) {
            std::cerr << "Failed to create participant." << std::endl;
            return false;
        }

    
        // Register Type 
        ::AirEntityTypeSupport_var ts = new ::AirEntityTypeSupportImpl();
        if (ts->register_type(participant_, ts->get_type_name()) != DDS::RETCODE_OK) {
            std::cerr << "Failed to register AirEntity type." << std::endl;
            return false;
        }

        // Create Topic
        topic_ = participant_->create_topic("AirEntityTopic", ts->get_type_name(),
                                        TOPIC_QOS_DEFAULT, nullptr,
                                        OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(topic_)) {
            std::cerr << "Failed to create topic." << std::endl;
            return false;
        }

        // PUBLISHER
        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr,
                                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(publisher_)) {
            std::cerr << "Failed to create publisher." << std::endl;
            return false;
        }

           // --- Transport Configuration ---
        OpenDDS::DCPS::TransportConfig_rch config =
            TheTransportRegistry->create_config("udp_config");

        OpenDDS::DCPS::TransportInst_rch inst =
            TheTransportRegistry->create_inst("udp1", "udp");

        OpenDDS::DCPS::UdpInst* udp = dynamic_cast<OpenDDS::DCPS::UdpInst*>(inst.in());
        if (udp) {
            udp->send_buffer_size_ = 65500;
            udp->rcv_buffer_size_  = 65500;
            udp->local_address(ACE_INET_Addr((u_short)0, ACE_LOCALHOST)); // binds to localhost
        }

        config->instances_.push_back(inst);
        TheTransportRegistry->bind_config(config, publisher_);

        // Customize DataWriter QoS
        DDS::DataWriterQos wqos;
        publisher_->get_default_datawriter_qos(wqos);

        wqos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
        wqos.history.depth = 1;
        wqos.reliability.kind = DDS::BEST_EFFORT_RELIABILITY_QOS;
        wqos.durability.kind = DDS::VOLATILE_DURABILITY_QOS;
        wqos.resource_limits.max_instances = 200;
        wqos.resource_limits.max_samples_per_instance = 1;
        wqos.resource_limits.max_samples =
            wqos.resource_limits.max_instances * wqos.resource_limits.max_samples_per_instance;

        // DATA WRITER
        writer_ = publisher_->create_datawriter(topic_, wqos,
                                                &listener_,
                                                OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(writer_)) {
            std::cerr << "Failed to create datawriter." << std::endl;
            return false;
        }

      

        air_writer_ = ::AirEntityDataWriter::_narrow(writer_);
        if (CORBA::is_nil(air_writer_)) {
            std::cerr << "Failed to narrow AirEntityDataWriter" << std::endl;
            return false;
        }

       
        std::cout << "Open DDS C++ Publisher initialized" << std::endl;
        return true;
    }

    bool publish(::AirEntity& instance) override
    {
        DDS::ReturnCode_t ret = air_writer_->write(instance, DDS::HANDLE_NIL);
        if (ret != DDS::RETCODE_OK)
        {
            std::cerr << "OpenDDS Publisher error in publish: "
                    << DDSReturnCodeToString(ret)
                    << " (" << ret << ")" << std::endl;
        }
        return ret == DDS::RETCODE_OK;
    }

    bool publish_loan(size_t count) override
    {
        return true;
    }

    void set_expected_subs(int expected) override
    {
        min_subs = expected;
    }

    bool is_matched() override
    {
        return matched_subs >= min_subs;
    }
};


#endif