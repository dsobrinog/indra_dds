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
// idl
#include <IDLs/generated/insePubSubTypes.hpp>

#include <chrono>

using namespace eprosima::fastdds::dds;

class sub_custom
{
    // Test Related
    public:
    int expected_messages_cycle = 1;

    void set_timestamp(AirEntity& entity)
    {
        using namespace std::chrono;

        // Tiempo actual en milisegundos desde el arranque del programa
        auto now = steady_clock::now().time_since_epoch();
        uint32_t ms = static_cast<uint32_t>(
            duration_cast<milliseconds>(now).count()
        );

        entity.entity_id().delayedActivationTime() = ms;
    }

    uint32_t get_timestamp(AirEntity& entity)
    {
        return entity.entity_id().delayedActivationTime();
    }


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
            SampleInfo info;
            if (reader->take_next_sample(&air_entity, &info) == eprosima::fastdds::dds::RETCODE_OK)
            {
                if (info.valid_data)
                {
                    samples_++;
                    std::cout << "Air Entity: with index: " << air_entity.entity_id().id()
                              << " RECEIVED." << std::endl;
                }
            }

            // SampleInfo info;
            // while (reader->take_next_sample(&air_entity, &info) == eprosima::fastdds::dds::RETCODE_OK)
            // {
            //     if (info.valid_data)
            //     {
            //         samples_++;
            //         std::cout << "Air Entity: with index: " << air_entity.entity_id().id()
            //                 << " RECEIVED." << std::endl;
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

    sub_custom()
        : participant_(nullptr)
        , subscriber_(nullptr)
        , topic_(nullptr)
        , reader_(nullptr)
        , type_(new AirEntityPubSubType())
    {
    }

    virtual ~sub_custom()
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
        DomainParticipantQos participantQos;
        participantQos.name("Participant_subscriber");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
        {
            return false;
        }

        // Register the Type
        type_.register_type(participant_);

        // Create the subscriptions Topic
        topic_ = participant_->create_topic("AirEntityTopic", "AirEntity", TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            return false;
        }

        // Create the Subscriber
        subscriber_ = participant_->create_subscriber(sQoS, nullptr);

        if (subscriber_ == nullptr)
        {
            return false;
        }

        // Create Default DataReader
        // reader_ = subscriber_->create_datareader(topic_, DATAREADER_QOS_DEFAULT, &listener_);

        // Create custom DataReader
       

        reader_ = subscriber_->create_datareader(topic_, rQoS, &listener_);

        if (reader_ == nullptr)
        {
            return false;
        }

        return true;
    }

    virtual bool is_match()
    {
        return listener_.matched > 0;
    }

    /// @brief Analyze data received
    /// @return true (all msg received), false (less than expected or none message at all)
    virtual bool run()
    {
        // Less than expected: Make report
        if (listener_.samples_ != 0 && listener_.samples_ < expected_messages_cycle)
        {
            std::cout << "Warning! Received: " 
            << listener_.samples_ 
            << "/" << expected_messages_cycle 
            << std::endl;
            listener_.samples_ = 0;
            return false;
        }

        // Message received as expected
        if(listener_.samples_ > 0)
        {
            std::cout << "Messages received this cycle: " << listener_.samples_ << std::endl;
            return true;
        }

        // No message at all received when matched
        return false;
    }

    virtual void reset_samples()
    {
        listener_.samples_ = 0;
    }



};