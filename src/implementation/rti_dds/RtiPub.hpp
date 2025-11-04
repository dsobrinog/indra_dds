#pragma once

#ifdef USE_RTI_DDS

#include <iostream>       
#include "abstraction/IPub.h"

// TO ADD IDL TEST CLASS GENERATED
class RtiPub : public IPub<void>
{
private:
    // PARTICIPANT
    // PUB
    // TOPIC
    // DATA WRITER
public:
    RtiPub()
    {
        std::cout<< " RtiPub Publisher created!" << std::endl;
    }

    ~RtiPub()
    {

    }

    bool init() override
    {
        // Cyclone DDS participant / topic / writer creation
        std::cout << "RTI C++ Publisher initialized" << std::endl;

        return true;
    }

    bool publish(cyclone_dds::AirEntity& instance) override
    {
        return true;
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