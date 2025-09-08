#pragma once

#include "pattern_base.h"

#include "stress_test/sub_custom.hpp"

class sub_test_1_to_1 : public pattern_base
{
    sub_custom sub;
    TestValidation test_validation;

public:
    sub_test_1_to_1(cl_dds* dds) : pattern_base(dds) {}

    void init()
    {
        SubscriberQos subQos = SUBSCRIBER_QOS_DEFAULT;

        DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
        rqos.history().kind = KEEP_LAST_HISTORY_QOS;
        rqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;

        if(sub.init(subQos, rqos))
            std::cout<< "Starting subscriber..." << std::endl;
        else
            std::cout << "Failed start subscriber" << std::endl;

        test_command();

        number_test = 1;
    }

    void test_command()
    {
        int number = 0; // Default command

        std::cout << "How many Air Entities do you expect to receive per cycle? ";
        std::cin >> number;

        if (std::cin.fail())  // check for invalid input (non-numeric)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Invalid input, using default 0." << std::endl;
            number = 0;
        }

        sub.expected_messages_cycle = number;
    }
    
    void read()
    {
        finish_test(!sub.run());
    };

    void write(){};
    void update(){};

    void reset_test()
    {
        sub.reset_samples();
    }
    
    void make_final_report() 
    {
        std::stringstream ss;
        ss << "\n**** FINAL 1:1 REPORT ****\n";
        ss << "NUMBER OF ENTITIES EXPECTED PER CYCLE: " << sub.expected_messages_cycle << "\n";
        ss << "NUMBER OF TESTS RUN: " << number_test << "\n";
        ss << "SUCCESS RATIO: " << test_validation.get_percentage() << "%\n";
        ss << "-----------------------------\n";

        std::cout << ss.str();
    }

    void on_test_finished(bool value)
    {
        test_validation.add_test_result(value ? TestResult::VALID : TestResult::INVALID);
    };

};