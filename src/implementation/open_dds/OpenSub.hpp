#pragma once

#ifdef USE_OPEN_DDS

#include "abstraction/ISub.h"
#include "stress_test/DDSParticipantManager.h"


#include <dds/DCPS/DomainParticipantFactoryImpl.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/SubscriberImpl.h>
#include <dds/DCPS/StaticIncludes.h>

#include "IDLs_open/generated/inse/inseC.h"
#include "IDLs_open/generated/inse/inseTypeSupportImpl.h"

#include <atomic>
#include <vector>
#include <iostream>
#include <set>

#include "open_dds_defs.h"


class OpenSub : public ISub
{
    ::DDS::DomainParticipant_var participant_;
    ::DDS::Subscriber_var subscriber_;
    ::DDS::Topic_var topic_;
    ::DDS::DataReader_var reader_;

    ::AirEntityDataReader_var dr;

    std::set<int> received_ids;
    int expected_messages_cycle = 0;

    std::vector<::AirEntity> pre_allocated_data;    
    std::vector<DDS::SampleInfo> pre_allocated_info;

    
    class SubListener : public virtual OpenDDS::DCPS::LocalObject<DDS::DataReaderListener>
    {
    public:
        SubListener(std::set<int>& ids, int& matched)
            : received_ids_(ids), matched_pub_(matched) {}

        void on_subscription_matched(DDS::DataReader_ptr /*reader*/,
                                     const DDS::SubscriptionMatchedStatus& status) override
        {
            matched_pub_ = status.current_count;
            std::cout << "OpenDDS Subscriber matched publishers: " << matched_pub_ << std::endl;
        }

        void on_data_available(DDS::DataReader_ptr reader) override {}
        void on_liveliness_changed(DDS::DataReader_ptr, const DDS::LivelinessChangedStatus&) override {}
        void on_requested_deadline_missed(DDS::DataReader_ptr, const DDS::RequestedDeadlineMissedStatus&) override {}
        void on_requested_incompatible_qos(DDS::DataReader_ptr, const DDS::RequestedIncompatibleQosStatus&) override {}
        void on_sample_rejected(DDS::DataReader_ptr, const DDS::SampleRejectedStatus&) override {}
        void on_sample_lost(DDS::DataReader_ptr, const DDS::SampleLostStatus&) override {}

    private:
        std::set<int>& received_ids_;
        int& matched_pub_;
    };

    int matched_pub_ = 0;
    SubListener listener_{received_ids, matched_pub_};

public:
    OpenSub(){
        std::cout<< "OpenSub created!" << std::endl;
    }

    void set_participant_manager(ParticipantManager* participant_manager) override
    {
        participant_ = static_cast<::DDS::DomainParticipant*>(participant_manager->getParticipant());
    }


    ~OpenSub() {
        if (!CORBA::is_nil(reader_)) subscriber_->delete_datareader(reader_);
        if (!CORBA::is_nil(subscriber_)) participant_->delete_subscriber(subscriber_);
        if (!CORBA::is_nil(topic_)) participant_->delete_topic(topic_);
        if (!CORBA::is_nil(participant_)) TheParticipantFactory->delete_participant(participant_);
    }

    bool init() override
    {
        if (CORBA::is_nil(participant_)) return false;

      
        
        ::AirEntityTypeSupport_var ts = new ::AirEntityTypeSupportImpl();
        if (ts->register_type(participant_, ts->get_type_name()) != DDS::RETCODE_OK) return false;

        topic_ = participant_->create_topic("AirEntityTopic", ts->get_type_name(),
                                            TOPIC_QOS_DEFAULT, nullptr,
                                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(topic_)) return false;

        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr,
                                                     OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(subscriber_))   {
            std::cout << "[OpenDDS] Subscriber subscriber failed." << std::endl;
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
        TheTransportRegistry->bind_config(config, subscriber_);


        DDS::DataReaderQos dr_qos;
        subscriber_->get_default_datareader_qos(dr_qos);

        dr_qos.reliability.kind = DDS::BEST_EFFORT_RELIABILITY_QOS;
        dr_qos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
        dr_qos.history.depth = 1;

        // Resource limits similar to Fast DDS
        dr_qos.resource_limits.max_instances = 200;
        dr_qos.resource_limits.max_samples_per_instance = 1;
        dr_qos.resource_limits.max_samples = dr_qos.resource_limits.max_instances * dr_qos.resource_limits.max_samples_per_instance;

        reader_ = subscriber_->create_datareader(
            topic_,
            dr_qos,
            &listener_,
            DDS::SUBSCRIPTION_MATCHED_STATUS // Only this one
        );

        if (CORBA::is_nil(reader_))
        {
            std::cout << "[OpenDDS] Subscriber reader failed." << std::endl;
            return false;
        }

        // Narrow once and keep the strongly typed reader
        dr = ::AirEntityDataReader::_narrow(reader_);
        if (CORBA::is_nil(dr)) {
            std::cerr << "[OpenDDS] Failed to narrow DataReader" << std::endl;
            return false;
        }

        pre_allocated_data.reserve(1000);
        pre_allocated_info.reserve(1000);
        std::cout << "[OpenDDS] Subscriber initialized." << std::endl;
        return true;
    }

    bool run_without_loan(int& lost_samples) override
    {
        received_ids.clear();
        int total_samples = 0;

        if (CORBA::is_nil(dr))
        {
            std::cerr << "[OpenDDS] DataReader is null!" << std::endl;
            lost_samples = expected_messages_cycle;
            return false;
        }

        // Preallocated buffers (match Fast DDS pattern)
        pre_allocated_data.resize(expected_messages_cycle);
        pre_allocated_info.resize(expected_messages_cycle);

        for (size_t i = 0; i < static_cast<size_t>(expected_messages_cycle); ++i)
        {
            // Take next sample directly into preallocated slot
            DDS::ReturnCode_t ret = dr->take_next_sample(pre_allocated_data[i], pre_allocated_info[i]);

            // Fast DDS: handle_result() equivalent
            if (ret == DDS::RETCODE_NO_DATA)
                break;
            else if (ret != DDS::RETCODE_OK)
            {
                std::cerr << "[OpenDDS] take_next_sample() failed: "
                        << DDSReturnCodeToString(ret)
                        << " (" << ret << ")" << std::endl;
                break;
            }

            total_samples++;

            if (pre_allocated_info[i].valid_data)
            {
                received_ids.insert(pre_allocated_data[i].id());
            }
            else
            {
                // Metadata sample — skip
                continue;
            }
        }

        // --- Evaluate results ---
        bool all_received = (received_ids.size() == static_cast<size_t>(expected_messages_cycle));

        if (!all_received)
        {
            lost_samples = expected_messages_cycle - static_cast<int>(received_ids.size());
            std::cout << "[OpenDDS] Warning: Received "
                    << received_ids.size() << "/" << expected_messages_cycle
                    << " entities this cycle (total samples: " << total_samples << ")"
                    << std::endl;
        }
        else
        {
            lost_samples = 0;
            std::cout << "[OpenDDS] All " << expected_messages_cycle
                    << " entities received this cycle." << std::endl;
        }

        return all_received;
    }



    bool run_with_loan(int& lost_samples){
        std::__throw_runtime_error("run with loan not implemented (OpenSub)");
        return false;
    }

    // Reset ids and accumulated samples
    void reset_samples(){
        received_ids.clear();
    }
    // Check connection with pub
    bool is_match(){
        return matched_pub_ > 0;
    }

    void set_expected_messages(int expected) {
        expected_messages_cycle = expected;
    }

    const std::set<int>& get_received_ids() const {
        return received_ids;
    }
};

#endif