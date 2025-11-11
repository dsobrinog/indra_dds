#pragma once

#ifdef USE_CYCLONE_DDS

#include <dds/dds.hpp>  // Modern C++ API
#include <iostream>       

#include "abstraction/IPub.h"

#include "IDLs_cyclone/generated/inse/inse.hpp"
#include <atomic>

using namespace dds::domain;
using namespace dds::pub;
using namespace dds::topic;
using namespace cyclone_dds;

class CyclonePub : public IPub<cyclone_dds::AirEntity>
{
public:
    // Test-related
    int messages_per_cycle = 1;
private:
    // DDS entities
    DomainParticipant participant_{dds::core::null};
    Publisher publisher_{dds::core::null};
    Topic<AirEntity> topic_{dds::core::null};
    DataWriter<AirEntity> writer_{dds::core::null};

     // State and tracking
    int current_id = 0;
    int min_subs = 0;
    int prev_matched = -1;
    std::atomic_int matched_subs_{0};

    // Listener to handle publication match events
    class PubListener : public dds::pub::NoOpDataWriterListener<AirEntity>
    {
    public:
        std::atomic_int matched_{0};

        void on_publication_matched(
            dds::pub::DataWriter<AirEntity>&,
            const dds::core::status::PublicationMatchedStatus& status) override
        {
            matched_ = status.current_count();
            if (status.current_count_change() > 0)
                std::cout << "[CycloneDDS] Publisher matched (" << matched_.load() << " subscribers)\n";
            else
                std::cout << "[CycloneDDS] Publisher unmatched (" << matched_.load() << " subscribers)\n";
        }
    } listener_;

public:
    CyclonePub()
    {
        std::cout<< " Cyclone DDS Publisher created!" << std::endl;
    }

    ~CyclonePub()
    {
        try
        {
            if (writer_ != dds::core::null)
                writer_.close();
            if (publisher_ != dds::core::null)
                publisher_.close();
            if (topic_ != dds::core::null)
                topic_.close();
            if (participant_ != dds::core::null)
                participant_.close();

            std::cout << "[CycloneDDS] Publisher cleaned up.\n";
        }
        catch (const dds::core::Exception& e)
        {
            std::cerr << "[CycloneDDS] Exception during cleanup: " << e.what() << std::endl;
        }
    }

    bool init() override
    {
        using namespace dds::core::policy;

        // --- DOMAIN PARTICIPANT CONFIGURATION ---
        dds::domain::qos::DomainParticipantQos participantQos;
        participantQos.policy( UserData(std::vector<uint8_t>{'P','u','b','l','i','s','h','e','r'}) );

        participant_ = dds::domain::DomainParticipant(0, participantQos);

        // --- PUBLISHER CONFIGURATION ---
        dds::pub::qos::PublisherQos pubQos = participant_.default_publisher_qos();
        publisher_ = dds::pub::Publisher(participant_, pubQos);

        // --- TOPIC CONFIGURATION ---
        topic_ = dds::topic::Topic<cyclone_dds::AirEntity>(
            participant_, "AirEntityTopic");

        // --- DATAWRITER CONFIGURATION ---
        dds::pub::qos::DataWriterQos writerQos = publisher_.default_datawriter_qos();

        // Customize QoS similar to FastDDS example
        writerQos << Reliability::BestEffort()
                  << Durability::Volatile()
                  << History::KeepLast(1);

        writer_ = dds::pub::DataWriter<cyclone_dds::AirEntity>(
            publisher_, topic_, writerQos);

        std::cout << "Cyclone DDS C++ Publisher initialized." << std::endl;
        return true;
    }

    bool publish(cyclone_dds::AirEntity& instance) override
    {
        writer_.write(instance);
        return true;
    }

    bool publish_loan(size_t count) override
    {
        for (size_t i = 0; i < count; ++i)
        {
            cyclone_dds::AirEntity instance;
            instance.id(current_id++);
            writer_.write(instance);
        }
        return true;
    }

    void set_expected_subs(int expected) override
    {
        min_subs = expected;
    }

    bool is_matched() override
    {
        return matched_subs_.load() >= min_subs;
    }
};


#endif