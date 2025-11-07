#pragma once

#ifdef USE_OPEN_DDS
#include "abstraction/IPubControl.h"
#include "stress_test/DDSParticipantManager.h"


#include "IDLs_open/generated/test_control/test_controlC.h"
#include "IDLs_open/generated/test_control/test_controlTypeSupportImpl.h"

#include <dds/DCPS/DomainParticipantFactoryImpl.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/StaticIncludes.h>

#include "open_dds_defs.h"

#include <atomic>
#include <iostream>

class OpenPubControl : public IPubControl
{
private:
    ::DDS::DomainParticipant_var participant_;
    ::DDS::Publisher_var publisher_;
    ::DDS::Topic_var topic_;
    ::DDS::DataWriter_var writer_;

    TestControlDataWriter_var dr;

    std::atomic_int matched_subs{0};
    int min_subs = 0;

    class PubListener : public virtual OpenDDS::DCPS::LocalObject<DDS::DataWriterListener>
    {
    public:
        std::atomic_int& matched_;
        PubListener(std::atomic_int& m) : matched_(m) {}

        void on_publication_matched(DDS::DataWriter_ptr,
                                    const DDS::PublicationMatchedStatus& status) override
        {
            matched_ = status.total_count;
            std::cout << "OpenPubControl matched subscribers: " << matched_ << std::endl;
        }

        void on_offered_deadline_missed(DDS::DataWriter_ptr, const DDS::OfferedDeadlineMissedStatus&) override {}
        void on_liveliness_lost(DDS::DataWriter_ptr, const DDS::LivelinessLostStatus&) override {}
        void on_offered_incompatible_qos(DDS::DataWriter_ptr, const DDS::OfferedIncompatibleQosStatus&) override {}
    } listener_{matched_subs};

public:
    OpenPubControl()
    {
        std::cout<< "OpenPubControl created!" << std::endl;
    }

    void set_participant_manager(ParticipantManager* participant_manager) override
    {
        participant_ = static_cast<::DDS::DomainParticipant*>(participant_manager->getParticipant());
    }

    ~OpenPubControl() override
    {
        if (!CORBA::is_nil(writer_)) publisher_->delete_datawriter(writer_);
        if (!CORBA::is_nil(publisher_)) participant_->delete_publisher(publisher_);
        if (!CORBA::is_nil(topic_)) participant_->delete_topic(topic_);
    }

    bool init() override
    {
        if (CORBA::is_nil(participant_)) return false;

        ::TestControlTypeSupport_var ts = new ::TestControlTypeSupportImpl();
        if (ts->register_type(participant_, ts->get_type_name()) != DDS::RETCODE_OK){
            std::cerr << "OpenPubControl: Registration type failed!" << std::endl;
            return false;
        }

        topic_ = participant_->create_topic("TestControlTopic", ts->get_type_name(),
                                            TOPIC_QOS_DEFAULT, nullptr,
                                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(topic_)){
            std::cerr << "OpenPubControl: Topic is null!" << std::endl;
            return false;
        }   

        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr,
                                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(publisher_)){
            std::cerr << "OpenPubControl: Publisher is null!" << std::endl;
            return false;
        }   

        // --- Configure writer QoS (equivalent to FastDDS setup) ---
        DDS::DataWriterQos wqos;
        publisher_->get_default_datawriter_qos(wqos);

        wqos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
        wqos.reliability.max_blocking_time.sec = 5;
        wqos.reliability.max_blocking_time.nanosec = 0;

        wqos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
        wqos.history.depth = 1000; // Only need the latest control messages

        wqos.durability.kind = DDS::TRANSIENT_LOCAL_DURABILITY_QOS;

        // Resource limits - generous and pre-allocated
        wqos.resource_limits.max_instances = DDS::LENGTH_UNLIMITED;
        wqos.resource_limits.max_samples_per_instance = DDS::LENGTH_UNLIMITED;
        wqos.resource_limits.max_samples = DDS::LENGTH_UNLIMITED;

        writer_ = publisher_->create_datawriter(topic_, wqos, &listener_,
                                                OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(writer_)){
            std::cerr << "OpenPubControl: Writer is null!" << std::endl;
            return false;
        }   

        dr = ::TestControlDataWriter::_narrow(writer_);
        if (CORBA::is_nil(dr)) {
            std::cerr << "Failed to narrow TestControlDataWriter" << std::endl;
            return false;
        }

        return true;
    }


    bool publish_start(int testId, int expected, bool loan) override
    {
        ::TestControl instance;
        instance.test_id() = testId;
        instance.start() = true;
        instance.expected_entities() = expected;
        instance.loan() = loan;

        DDS::ReturnCode_t ret = dr->write(instance, DDS::HANDLE_NIL);
        if (ret != DDS::RETCODE_OK)
        {
            std::cerr << "OpenDDS Publisher Control error in publishing start: "
                    << DDSReturnCodeToString(ret)
                    << " (" << ret << ")" << std::endl;
        }
        return ret == DDS::RETCODE_OK;
    }
    
    bool publish_stop(int testId)
    {
        ::TestControl instance;
        instance.test_id() = testId;
        instance.start() = false;

        DDS::ReturnCode_t ret = ::TestControlDataWriter::_narrow(writer_)->write(instance, DDS::HANDLE_NIL);
        if (ret != DDS::RETCODE_OK)
        {
            std::cerr << "OpenDDS Publisher Control error in publishing stop: "
                    << DDSReturnCodeToString(ret)
                    << " (" << ret << ")" << std::endl;
        }
        return ret == DDS::RETCODE_OK;
    }

    bool is_matched() const
    {
        return matched_subs >= min_subs;
    }

private:

};

#endif