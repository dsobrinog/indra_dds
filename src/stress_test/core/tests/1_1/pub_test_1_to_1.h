#pragma once

#include "pattern_base.h"

#include "stress_test/pub_custom.hpp"

class pub_test_1_to_1 : public pattern_base
{
public:
    pub_test_1_to_1(cl_dds* dds) : pattern_base(dds) {}

    pub_custom pub;
    bool send = false;

    void init()
    {
        PublisherQos pubQos = PUBLISHER_QOS_DEFAULT;

        DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
        wqos.history().kind = KEEP_LAST_HISTORY_QOS;
        wqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
        wqos.durability().kind = TRANSIENT_LOCAL_DURABILITY_QOS;

        if(pub.init(pubQos, wqos))
            std::cout<< "Starting publisher..." << std::endl;
        else
            std::cout << "Failed start publisher" << std::endl;

        test_command();

        number_test = 1;
    }

    void read(){};

    void write()
    {
        if (send)
        {
            send=false;
            pub.publish();
        }
    }

    void test_command()
    {
        int number = 0; // Default command

        std::cout << "How many Air Entities to send each cycle? ";
        std::cin >> number;

        if (std::cin.fail())  // check for invalid input (non-numeric)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Invalid input, using default 0." << std::endl;
            number = 0;
        }

        pub.messages_per_cycle = number;
        send = true;
    }

    void update(){};

    void reset_test()
    {
        send = true;
    };
    void make_final_report(){};
    void on_test_finished(bool value)
    {
    };

};