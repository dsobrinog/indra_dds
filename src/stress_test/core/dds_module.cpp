#include "dds_module.h"

#include "patterns/pattern_base.h"
#include "stress_test/core/tests/1_1/pub_test_1_to_1.h"
#include "stress_test/core/tests/1_1/sub_test_1_to_1.h"
#include "stress_test/core/tests/1_many/sub_test_1_to_many.hpp"

// #include "patterns/ping_pong_pattern.h"

cl_dds::cl_dds(int moduleId, int mode, Executive * _exec) : cl_module(moduleId, _exec)
{
}

cl_dds::cl_dds(test_config _config, Executive* _exec) : cl_module(0, _exec), config(_config)
{
    if(config.manual_mode)
        config.listener_number = 1;
}

cl_dds::~cl_dds()
{
}


void cl_dds::init()
{
    test_patterns(config.test_mode);
    if(p_pattern)
        p_pattern->init();
}

void cl_dds::read()
{
    p_pattern->read();
}

void cl_dds::write()
{
    p_pattern->write();
}
void cl_dds::update()
{
    p_pattern->update();
}

void cl_dds::end()
{
   
}


void cl_dds::test_patterns(int mode)
{
    int number = 0; // Default command

    if(mode <= 0)
    {
        char input;
        std::cout << "Select test: \n 1) PUB \n 2) SUB \n 3) PUB 1:MANY \n 4) SUB 1:MANY \n";
        std::cin >> input;


        // Check if input is a digit
        if (std::isdigit(input))
        {
            number = input - '0'; // Convert char digit to int
        }
    }
    else
    {
        number = mode;
    }
   

    switch (number)
    {
        case TestMode::One_to_One_Pub: 
        {
            // PUB - 
            p_pattern = new pub_test_1_to_1(this, config);
            break;
        }
        case TestMode::One_to_One_Sub:
        {
            // SUB - 
            p_pattern = new sub_test_1_to_1(this, config);
            break;
        }
        case TestMode::One_to_Many_Pub:
        {
            //1:Many Pub
            p_pattern = new pub_test_1_to_1(this, config);
            break;
        }
        case TestMode::One_to_Many_Sub:
        {
            //1:Many Sub
            break;
        }

        default:
        test_patterns(number);
            break;
    }
}