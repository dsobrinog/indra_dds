#pragma once

#ifdef USE_OPEN_DDS

#include "abstraction/ISubControl.h"
#include "stress_test/DDSParticipantManager.h"

#include "IDLs_open/generated/test_control/test_controlC.h"
#include "IDLs_open/generated/test_control/test_controlTypeSupportImpl.h"

#include <dds/DCPS/DomainParticipantFactoryImpl.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/StaticIncludes.h>

#include <atomic>
#include <iostream>
#include <optional>

#include "open_dds_defs.h"

class OpenSubControl : public ISubControl
{
private:
    ::DDS::DomainParticipant_var participant_;
    ::DDS::Subscriber_var subscriber_;
    ::DDS::Topic_var topic_;
    ::DDS::DataReader_var reader_;

    // narrow datareader
    ::TestControlDataReader_var dr;

    TestConfig last_{};
    std::atomic_int matched_{0};

    class SubListener : public virtual OpenDDS::DCPS::LocalObject<DDS::DataReaderListener>
    {
    public:
        std::atomic_int& matched_;

        explicit SubListener(std::atomic_int& m) : matched_(m) {}

        void on_subscription_matched(DDS::DataReader_ptr,
                                     const DDS::SubscriptionMatchedStatus& s) override
        {
            if (s.current_count_change == 1)
                ++matched_;
            else if (s.current_count_change == -1)
                --matched_;

            std::cout << "OpenDDS Subscriber Controller matched: " << matched_ << std::endl;

        }

        // Keep all other callbacks empty
        void on_data_available(DDS::DataReader_ptr) override {}
        void on_requested_deadline_missed(DDS::DataReader_ptr, const DDS::RequestedDeadlineMissedStatus&) override {}
        void on_requested_incompatible_qos(DDS::DataReader_ptr, const DDS::RequestedIncompatibleQosStatus&) override {}
        void on_liveliness_changed(DDS::DataReader_ptr, const DDS::LivelinessChangedStatus&) override {}
        void on_sample_rejected(DDS::DataReader_ptr, const DDS::SampleRejectedStatus&) override {}
        void on_sample_lost(DDS::DataReader_ptr, const DDS::SampleLostStatus&) override {}
    } listener_{matched_};

public:
    OpenSubControl()
    {
        std::cout << "[OpenDDS] OpenSubControl created!" << std::endl;
    }

    ~OpenSubControl() override
    {
        if (!CORBA::is_nil(reader_)) subscriber_->delete_datareader(reader_);
        if (!CORBA::is_nil(subscriber_)) participant_->delete_subscriber(subscriber_);
        if (!CORBA::is_nil(topic_)) participant_->delete_topic(topic_);
        if (!CORBA::is_nil(participant_)) TheParticipantFactory->delete_participant(participant_);
    }

    void set_participant_manager(ParticipantManager* participant_manager) override
    {
        participant_ = static_cast<::DDS::DomainParticipant*>(participant_manager->getParticipant());
    }

    bool init() override
    {
        if (CORBA::is_nil(participant_))
        {
            std::cerr << "OpenSubControl: Participant is null" << std::endl;
            return false;
        }

        ::TestControlTypeSupport_var ts = new ::TestControlTypeSupportImpl();
        if (ts->register_type(participant_, ts->get_type_name()) != DDS::RETCODE_OK)
        {
            std::cerr << "OpenSubControl: Registration type failed!" << std::endl;
            return false;
        }

        topic_ = participant_->create_topic("TestControlTopic",
                                            ts->get_type_name(),
                                            TOPIC_QOS_DEFAULT,
                                            nullptr,
                                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(topic_))
        {
            std::cerr << "OpenSubControl: Topic is null" << std::endl;
            return false;
        }
                   

        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                                     nullptr,
                                                     OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(subscriber_))
        {
            std::cerr << "OpenSubControl: Subscriber is null" << std::endl;
            return false;
        }

        DDS::DataReaderQos rqos;
        subscriber_->get_default_datareader_qos(rqos);

        rqos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
        rqos.durability.kind = DDS::TRANSIENT_LOCAL_DURABILITY_QOS;
        rqos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
        rqos.history.depth = 1001;
        rqos.resource_limits.max_samples = DDS::LENGTH_UNLIMITED;
        rqos.resource_limits.max_instances = DDS::LENGTH_UNLIMITED;
        rqos.resource_limits.max_samples_per_instance = DDS::LENGTH_UNLIMITED;

        reader_ = subscriber_->create_datareader(topic_,
                                                 rqos,
                                                 &listener_,
                                                 OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(reader_))
        {
            std::cerr << "OpenSubControl: DataReader is null" << std::endl;
            return false;
        }

        // Narrow once and keep the strongly typed reader
        dr = ::TestControlDataReader::_narrow(reader_);
        if (CORBA::is_nil(dr)) {
            std::cerr << "[OpenDDS] Failed to narrow DataReader" << std::endl;
            return false;
        }

        std::cout << "[OpenDDS] Control subscriber initialized." << std::endl;
        return true;
    }

    bool has_match() const override
    {
        return matched_ > 0;
    }

    bool poll_control(TestConfig& out) override
    {
        if (CORBA::is_nil(dr)) 
        {
            dr = ::TestControlDataReader::_narrow(reader_);
            if (CORBA::is_nil(dr)) {
                std::cerr << "[OpenDDS] re-narrow failed in poll_control()" << std::endl;
                return false;
            } else {
                std::cout << "[OpenDDS] re-narrow succeeded in poll_control()" << std::endl;
            }
        }

        bool applied = false;
        ::TestControl sample;
        DDS::SampleInfo info;

        while (dr->take_next_sample(sample, info) == DDS::RETCODE_OK)
        {
            if (!info.valid_data)
                continue;

            last_.testId = sample.test_id();
            last_.expectedEntities = static_cast<int>(sample.expected_entities());
            last_.loan = sample.loan();
            last_.active = sample.start();

            out = last_;
            applied = true;
        }

        return applied;
    }

    const TestConfig& current() const
    {
        return last_;
    }
};

#endif
