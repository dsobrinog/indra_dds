#pragma once

#include "pattern_base.h"

#include "stress_test/sub_test.hpp"
#include "stress_test/sub_control.hpp"

#include "stress_test/core/executive.h"

class sub_test_1_to_many : public sub_test_1_to_1
{
    Distribution lost_samples;
    
public:
    sub_test_1_to_many(cl_dds* dds, int expected_msg, int test, Distribution& acc_lost_samples) : sub_test_1_to_1(dds, expected_msg, test)
    {
        lost_samples = acc_lost_samples;
        max_test = test;
    };

    void on_test_finished(bool value)
    {
        test_validation.add_test_result(value ? TestResult::VALID : TestResult::INVALID);

        if(current_test == max_test)
        {
            std::cout << "Last Test" << std::endl;
            lost_samples.addValue(total_lost_samples);
        }
    }
};