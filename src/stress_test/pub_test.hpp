#pragma once

// Fast DDS includes
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.hpp>

#include <fastdds/dds/core/LoanableSequence.hpp>

// idl
#include <IDLs/generated/inse/insePubSubTypes.hpp>
#include <stress_test/core/tests/test_def.h>

#include <chrono>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;


class PubTest
{
    public:

    // Test related
    int messages_per_cycle = 1;

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

    PubTest()
        : participant_(nullptr)
        , publisher_(nullptr)
        , topic_(nullptr)
        , writer_(nullptr)
        , type_(new AirEntityPubSubType())
    {
    }

    virtual ~PubTest()
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
        // DDS PARTICIPANT CONFIGURATION
        DomainParticipantQos participantQos;
        participantQos.name("Participant_publisher");
        participantQos.wire_protocol().builtin.discovery_config.leaseDuration = c_TimeInfinite;

        
        // UDP COMMS
        participantQos.transport().use_builtin_transports = false;
        auto udp_descriptor = std::make_shared<UDPv4TransportDescriptor>();
        udp_descriptor->sendBufferSize = 65500;
        udp_descriptor->maxMessageSize = 65500;
        participantQos.transport().user_transports.push_back(udp_descriptor);

        // DDS PARTICIPANT CREATION
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);
        if (participant_ == nullptr)
        {
            return false;
        }

        // REGISTER TYPE
        type_.register_type(participant_);
        std::cout << "AirEntity type is_plain(): " << std::boolalpha << type_->is_plain(XCDR2_DATA_REPRESENTATION) << std::endl;
        std::cout << "AirEntity type is_bounded() = " << type_.is_bounded() << std::endl;

        // DDS TOPIC
        topic_ = participant_->create_topic("AirEntityTopic", "AirEntity", TOPIC_QOS_DEFAULT);
        if (topic_ == nullptr)
        {
            return false;
        }

        // DDS PUBLISHER
        publisher_ = participant_->create_publisher(pQoS, nullptr);
        if (publisher_ == nullptr)
        {
            return false;
        }

        // CUSTOM DATA WRITER
        DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
        wqos.history().kind = KEEP_LAST_HISTORY_QOS;
        wqos.history().depth = 200;
        wqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
        writer_ = publisher_->create_datawriter(topic_, QoS, &listener_);
        if (writer_ == nullptr)
        {
            return false;
        }

        // SUCCESS
        return true;
    }

    int current_id = 0;
    
    bool is_matched()
    {
        return listener_.matched_ > 0;
    };

    virtual bool publish(AirEntity& entity)
    {
        return writer_->write(&entity) == RETCODE_OK;
    }

    virtual bool publish_loan(size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            void* buffer_ptr = nullptr;
            ReturnCode_t ret = writer_->loan_sample(buffer_ptr);

            if (ret != RETCODE_OK)
            {
                std::cerr << "loan_sample error: " << (int)ret << std::endl;
                return false;
            }

            // Fill the data directly in the loaned buffer (no extra copy)
            AirEntity* entity_sample = static_cast<AirEntity*>(buffer_ptr);
            entity_sample->id() = current_id++;
            // ... fill the rest of the fields ...

            // Publish
            ret = writer_->write(entity_sample);
            if (ret != RETCODE_OK)
            {
                std::cerr << "write after loan error: " << (int)ret << std::endl;

                ret = writer_->discard_loan(buffer_ptr);
                if(ret != RETCODE_OK)
                {
                   std::cerr << "discard loan error: " << (int)ret << std::endl;
                }
                
                return false;
            }
        }
        return true;
    }
};
