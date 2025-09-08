#include <iostream>

// Executive Test
#include "core/executive.h"
#include "core/dds_module.h"


Executive* _exec; 

int main(int argc, char* argv[])
{
    int mode = -1;
    if(argc > 1)
    {
        mode = std::stoi(argv[1]);
    }
    
    // Start Simulation
    std::cout << "EXECUTIVE*** : Start Simulation" << std::endl;
    _exec = new Executive;
    cl_dds* moduleDDS = new cl_dds(0, mode, _exec);
    _exec->add_module(moduleDDS);
    _exec->init();

    // Loop
    std::cout << "EXECUTIVE*** : Begining Execution..." << std::endl;
    while (_exec->in_execution())
    {
        _exec->execute_cycle();
    }

    
    // End
    _exec->end_simulation();
    std::cout << "EXECUTIVE*** : End Simulation..." << std::endl;
    return 0;
}
