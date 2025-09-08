#pragma once

// Fast DDS includes
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>



// idl
#include <IDLs/generated/insePubSubTypes.hpp>

#include <chrono>

using namespace eprosima::fastdds::dds;

class pub_custom
{
    public:

    // Test related
    int messages_per_cycle = 1;

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

    AirEntity air_entity;

    DomainParticipant* participant_;

    Publisher* publisher_;

    Topic* topic_;

    DataWriter* writer_;

    TypeSupport type_;

    class PubListener : public DataWriterListener
    {
    public:

        PubListener()
            : matched_(0)
        {
        }

        ~PubListener() override
        {
        }

        void on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                matched_ = info.total_count;
                std::cout << "Publisher matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                matched_ = info.total_count;
                std::cout << "Publisher unmatched." << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                        << " is not a valid value for PublicationMatchedStatus current count change." << std::endl;
            }
        }

        /// @brief How many people we have listening to our topic
        std::atomic_int matched_;

    } listener_;

public:

    pub_custom()
        : participant_(nullptr)
        , publisher_(nullptr)
        , topic_(nullptr)
        , writer_(nullptr)
        , type_(new AirEntityPubSubType())
    {
    }

    virtual ~pub_custom()
    {
        if (writer_ != nullptr)
        {
            publisher_->delete_datawriter(writer_);
        }
        if (publisher_ != nullptr)
        {
            participant_->delete_publisher(publisher_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    //!Initialize the publisher
    bool init(PublisherQos& pQoS, DataWriterQos& QoS)
    {
        // Initialize values of air entity

        DomainParticipantQos participantQos;
        participantQos.name("Participant_publisher");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
        {
            return false;
        }

        // Register the Type
        type_.register_type(participant_);

        // Create the publications Topic
        topic_ = participant_->create_topic("AirEntityTopic", "AirEntity", TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            return false;
        }

        // Create the Publisher
        publisher_ = participant_->create_publisher(pQoS, nullptr);

        if (publisher_ == nullptr)
        {
            return false;
        }

        // Create Default DataWriter
        // writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, &listener_);

        // Create Custom DataWriter
        DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
        wqos.history().kind = KEEP_LAST_HISTORY_QOS;
        wqos.history().depth = 200;
        wqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;

        writer_ = publisher_->create_datawriter(topic_, QoS, &listener_);

        if (writer_ == nullptr)
        {
            return false;
        }
        return true;
    }

    int current_id = 0;
    
    bool is_matched()
    {
        return listener_.matched_ > 0;
    };

    //!Send a publication
    virtual bool publish()
    {
        if (listener_.matched_ > 0)
        {
            int message_sent = 0;
            for (size_t i = 0; i < messages_per_cycle; i++)
            { 
                EntityID id = EntityID();
                id.id(current_id++);
                air_entity.entity_id(id);
                // time_stamp
                set_timestamp(air_entity);
                
                if(writer_->write(&air_entity) == 0) // OK
                    message_sent++;
                
            }

            std::cout<< "Messages send this cycle: " << message_sent << std::endl;
            return true;
        }
        return false;
    }
};
