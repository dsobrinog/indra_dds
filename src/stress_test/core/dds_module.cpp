#include "dds_module.h"

cl_dds::cl_dds(int moduleId, Executive * _exec) : cl_module(moduleId, _exec)
{
}

cl_dds::~cl_dds()
{
}


void cl_dds::init()
{
    test_patterns();
}

void cl_dds::read()
{}

void cl_dds::write()
{}

void cl_dds::end()
{}


void cl_dds::test_patterns()
{
    char input;
    std::cout << "Select test: \n 1) PUB \n 2) SUB \n ";
    std::cin >> input;

    int number = 0; // Default command

    // Check if input is a digit
    if (std::isdigit(input))
    {
        number = input - '0'; // Convert char digit to int
    }

    switch (number)
    {
        case 1: 
        {
            // PUB
            break;
        }
        case 2:
        {
            // SUB
        }
        default:
        test_patterns();
            break;
    }
}