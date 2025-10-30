#pragma once

#include <string>

enum TestMode
{
    DefaultMode = 0,
    One_to_One_Pub = 1,
    One_to_One_Sub = 2,
};

inline std::string ToString(TestMode mode)
{
    switch (mode)
    {
        case DefaultMode:      return "DefaultMode";
        case One_to_One_Pub:   return "One_to_One_Pub";
        case One_to_One_Sub:   return "One_to_One_Sub";

        default:               return "Unknown";
    }
}

struct test_config{
    std::string name = "default name";
    TestMode test_mode = TestMode::DefaultMode;
    int message_count = 100;
    int test_count = 10;
    std::string logFile = "output.log";
    bool manual_mode = false;
    bool loan_mode = false;

    // PUB
    int listener_number = 0;
};