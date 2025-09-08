#include "dds_module.h"

#include "patterns/pattern_base.h"
#include "stress_test/core/tests/1_1/pub_test_1_to_1.h"
#include "stress_test/core/tests/1_1/sub_test_1_to_1.h"

#include "patterns/ping_pong_pattern.h"

cl_dds::cl_dds(int moduleId, int mode, Executive * _exec) : cl_module(moduleId, _exec)
{
    _mode = mode;
}

cl_dds::~cl_dds()
{
}


void cl_dds::init()
{

    test_patterns(_mode);
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

    if(_mode <= 0)
    {
        char input;
        std::cout << "Select test: \n 1) PUB \n 2) SUB \n 3) PING-PONG \n";
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
        case 1: 
        {
            // PUB
            p_pattern = new pub_test_1_to_1(this);
            break;
        }
        case 2:
        {
            // SUB
            p_pattern = new sub_test_1_to_1(this);
            break;
        }
        case 3:
        {
            // PING-PONG
            p_pattern = new ping_pong_pattern(this);
            break;
        }
        default:
        test_patterns(number);
            break;
    }
}