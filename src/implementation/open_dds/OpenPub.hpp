#pragma once

#ifdef USE_OPEN_DDS

#include <iostream>       
#include "abstraction/IPub.h"


#include "IDLs_open/generated/inse/inseTypeSupportImpl.h"

#include <dds/DCPS/DomainParticipantFactoryImpl.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/StaticIncludes.h>


class OpenPub : public IPub<::AirEntity>
{
private:
    ::DDS::DomainParticipant_var participant_;
    ::DDS::Publisher_var publisher_;
    ::DDS::Topic_var topic_;
    ::DDS::DataWriter_var writer_;

    int current_id;
    int min_subs;
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
        // Cyclone DDS participant / topic / writer creation
        std::cout << "Open DDS C++ Publisher initialized" << std::endl;
        
        // Participant Factory
        int argc = 0;
        char** argv = nullptr;
        DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);

        // Create participant
         participant_ = dpf->create_participant(
            0, // domain ID
            PARTICIPANT_QOS_DEFAULT,
            nullptr,
            OpenDDS::DCPS::DEFAULT_STATUS_MASK
        );
        if (CORBA::is_nil(participant_)) {
            std::cerr << "Failed to create participant." << std::endl;
            return false;
        }

        // Register Type (Topic)
        ::AirEntityTypeSupport_var ts = new ::AirEntityTypeSupportImpl();
        if (ts->register_type(participant_, "") != DDS::RETCODE_OK) {
            std::cerr << "Failed to register AirEntity type." << std::endl;
            return false;
        }

        return true;
    }

    bool publish(::AirEntity& instance) override
    {
        return true;
        // instance.id() = current_id++;
        // return writer_->write(instance, DDS::HANDLE_NIL) == DDS::RETCODE_OK;
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