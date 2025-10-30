#pragma once

#ifdef USE_CYCLONE_DDS

// #include <dds/dds.h>  // C API, defines dds_instance_handle_t
// #include <dds/dds.hpp>  // Modern C++ API
#include <iostream>       

#include "abstraction/IPub.h"
#include "IDLs_cyclone/generated/inse/inse.h"

    
class CyclonePub : public IPub<cyclone_dds_AirEntity>
{
private:
    dds::domain::DomainParticipant participant_;
    dds::pub::Publisher publisher_;
    dds::topic::Topic<cyclone_dds_AirEntity> topic_;
    dds::pub::DataWriter<cyclone_dds_AirEntity> writer_;

    int current_id;
    int min_subs;
    std::atomic_int matched_subs = 0;

public:
     CyclonePub()
        : participant_(dds::domain::DomainParticipant(0)),
          publisher_(dds::pub::Publisher(participant_)),
          topic_(dds::topic::Topic<cyclone_dds_AirEntity>(participant_, "AirEntityTopic")),
          writer_(dds::pub::DataWriter<cyclone_dds_AirEntity>(publisher_, topic_))
    {
    }

    ~CyclonePub()
    {

    }

    bool init() override
    {
        // Cyclone DDS participant / topic / writer creation
        std::cout << "Cyclone DDS C++ Publisher initialized" << std::endl;

        return true;
    }

    bool publish(cyclone_dds_AirEntity& instance) override
    {
        // Cyclone DDS write
        writer_.write(instance);
        return true;
    }

    bool publish_loan(size_t count) override
    {
        // Cyclone DDS loaned samples
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