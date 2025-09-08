#pragma once

#include "stress_test/core/tests/test_def.h"

#include <vector>
#include <iostream>

class TestValidation
{
    std::vector<TestResult> tests;

public:

    void add_test_result(TestResult r){
        tests.push_back(r);
    }

    float get_percentage() const
    {
        if (tests.empty()) return 0.0f;

        size_t number_success = std::count(tests.begin(), tests.end(), TestResult::VALID);
        return (static_cast<float>(number_success) / tests.size()) * 100.f;
    }


    void print_report()
    {
        std::cout << "\n**** TEST VALIDATION REPORT ****\n";
        std::cout << "Percentage: " << get_percentage() << " success \n";
    }
};