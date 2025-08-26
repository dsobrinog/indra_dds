#include <iostream>

// Executive Test

#include "core/executive.h"
#include "core/dds_module.h"

// Fast DDS includes
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

// IDL include
#include <IDLs/generated/inse.hpp>

#include "pub_test.h"
#include "sub_test.h"


using namespace eprosima::fastdds::dds;

Executive* _exec; 

int main()
{
    // Select pub or sub
    std::cout << "Select mode: 1) Pub 2) Sub" << std::endl;


    // Start Simulation
    std::cout << "Start Simulation" << std::endl;
    _exec = new Executive;
    cl_dds* moduleDDS = new cl_dds(0, _exec);
    _exec->add_module(moduleDDS);
    _exec->init();

    // Loop
    std::cout << "Begining Execution..." << std::endl;
    while (_exec->in_execution())
    {
        _exec->execute_cycle();
    }

    
    // End
    _exec->end_simulation();
    std::cout << "End Simulation..." << std::endl;
    return 0;
}
