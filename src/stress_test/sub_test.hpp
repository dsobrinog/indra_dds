#pragma once

#pragma once

// Fast DDS includes
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.hpp>

// idl
#include <IDLs/generated/inse/insePubSubTypes.hpp>

#include <stress_test/core/tests/test_def.h>

#include <chrono>
#include <unordered_set>

using namespace eprosima::fastdds::dds;

class SubTest
{

public:    
    // Test Related
    int expected_messages_cycle = 1;
    std::unordered_set<int> received_ids;
    std::vector<AirEntity> pre_allocated_data;    
    std::vector<SampleInfo> pre_allocated_info;

private:

    DomainParticipant* participant_;

    Subscriber* subscriber_;

    DataReader* reader_;

    Topic* topic_;

    TypeSupport type_;

  

    class SubListener : public DataReaderListener
    {
    public:
        SubListener()
            : samples_(0)
        {
        }

        ~SubListener() override
        {
        }

        void on_subscription_matched(
                DataReader*,
                const SubscriptionMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                std::cout << "Subscriber matched." << std::endl;
                matched =true;
            }
            else if (info.current_count_change == -1)
            {
                std::cout << "Subscriber unmatched." << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                          << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
            }
        }

        void on_data_available(
                DataReader* reader) override
        {
            // SampleInfo info;
            // if (reader->take_next_sample(&air_entity, &info) == eprosima::fastdds::dds::RETCODE_OK)
            // {
            //     if (info.valid_data)
            //     {
            //         samples_++;
            //         std::cout << "Air Entity: with index: " << air_entity.entity_id().id()
            //                   << " RECEIVED." << std::endl;
            //     }
            // }
        }

        AirEntity air_entity;
        bool matched = false;

    public:
        std::atomic_int samples_;
            }
    listener_;
    
public:

    SubTest()
        : participant_(nullptr)
        , subscriber_(nullptr)
        , topic_(nullptr)
        , reader_(nullptr)
        , type_(new AirEntityPubSubType())
    {
    }

    virtual ~SubTest()
    {
        if (reader_ != nullptr)
        {
            subscriber_->delete_datareader(reader_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        if (subscriber_ != nullptr)
        {
            participant_->delete_subscriber(subscriber_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

     //!Initialize the subscriber
    bool init(SubscriberQos& sQoS, DataReaderQos& rQoS)
    {
        // PARTICIPANT CONFIG (QoS)
        DomainParticipantQos participantQos;
        participantQos.wire_protocol().builtin.discovery_config.leaseDuration = c_TimeInfinite; // DEBUG PURPUSE
        participantQos.name("Participant_subscriber");
  
        // UDP COMMS
        participantQos.transport().use_builtin_transports = false;
        auto udp_descriptor = std::make_shared<UDPv4TransportDescriptor>();
        udp_descriptor->sendBufferSize = 65500;
        udp_descriptor->maxMessageSize = 65500;
        participantQos.transport().user_transports.push_back(udp_descriptor);

        // DDS PARTICIPANT
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);
        if (participant_ == nullptr)
        {
            return false;
        }

        // REGISTER TYPE
        type_.register_type(participant_);

        // DDS TOPIC
        topic_ = participant_->create_topic("AirEntityTopic", "AirEntity", TOPIC_QOS_DEFAULT);
        if (topic_ == nullptr)
        {
            return false;
        }

        // DDS SUBSCRIBER
        subscriber_ = participant_->create_subscriber(sQoS, nullptr);
        if (subscriber_ == nullptr)
        {
            return false;
        }

        // CUSTOM DATA READER
        reader_ = subscriber_->create_datareader(topic_, rQoS, &listener_);
        if (reader_ == nullptr)
        {
            return false;
        }

        // PREALLOCATION
        pre_allocated_data.reserve(1000);
        pre_allocated_info.reserve(1000);

        // SUCCESS
        return true;
    }

    virtual bool is_match()
    {
        return listener_.matched > 0;
    }
    virtual int get_samples()
    { 
        return listener_.samples_; 
    }

    virtual void reset_samples()
    {
        listener_.samples_ = 0;
        received_ids.clear();
    }

    inline bool handle_result(ReturnCode_t result)
    {
        if (result != RETCODE_OK && result != RETCODE_NO_DATA)
        {
            std::cerr << "Error code: " << print_return_code(result) << " trying to take sample from subscriber" << std::endl;
        }
        
        return result == 0;
    }

    
  
    // SYNC CONSUME PATTERN
    // Main thread drains samples enqeueud in DDS

    virtual bool run_without_loan()
    {
        received_ids.clear();
        int total_samples = 0;

        for (size_t i = 0; i < expected_messages_cycle; i++)
        {
            // Drain next sample until expected message or empty
            ReturnCode_t ret = reader_->take_next_sample(&pre_allocated_data[i], &pre_allocated_info[i]);

            if(!handle_result(ret))
            {
                break;
            }
            
            total_samples++;
            
            if(pre_allocated_info[i].valid_data)
            {
                received_ids.insert(pre_allocated_data[i].id());
            }
            else
            {
                continue;
            }
        }
        
        // Check if we got all expected entities
        bool all_received = (received_ids.size() == expected_messages_cycle);

        if (!all_received)
        {
            std::cout << "Warning! Received: "
                    << received_ids.size()
                    << "/" << expected_messages_cycle
                    << " entities this cycle (total samples: " << total_samples << ")\n";
        }
        else
        {
            std::cout << "All " << expected_messages_cycle
                    << " entities received this cycle\n";
        }

        return all_received;
    }

    virtual bool run_with_loan()
    {
        received_ids.clear();
        int total_samples = 0;
        
        // Continue taking samples until no more are available
        while (true)
        {
            LoanableSequence<AirEntity> data;
            SampleInfoSeq info;
            
            // Take available samples (not unlimited to avoid potential issues)
            ReturnCode_t ret = reader_->take(data, info, 100); // Take up to 100 at a time
            
            if (ret != RETCODE_OK || data.length() == 0)
            {
                break; // No more samples available
            }
            
            total_samples += data.length();
            
            for (int i = 0; i < data.length(); i++)
            {
                if (info[i].valid_data)
                {
                    received_ids.insert(data[i].id());
                }
            }
            
            reader_->return_loan(data, info);
        }
        
        // Check if we got all expected entities
        bool all_received = (received_ids.size() == expected_messages_cycle);

        if (!all_received)
        {
            std::cout << "Warning! Received: " 
                    << received_ids.size() 
                    << "/" << expected_messages_cycle 
                    << " entities this cycle (total samples: " << total_samples << ")\n";
        }
        else
        {
            std::cout << "All " << expected_messages_cycle 
                    << " entities received this cycle\n";
        }

        return all_received;
    }


};