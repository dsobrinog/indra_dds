#include "executive.h"
#include <chrono>



void Executive::init()
{
    // Pin the current thread (the one running init())
    pinThreadToCore(2);   // choose a core that is free / isolated if possible
    setRealtimePriority();

    for (auto& module : submodules)
    {
        module->init();
    }
    
    change_state(ExecutiveState::EXECUTION);
}

const void Executive::add_module(cl_module * module)
{
    for (size_t i = 0; i < submodules.size(); i++)
    {
        if(submodules[i]->get_module_id() == module->get_module_id())
        {
            std::cout << "Trying to add a module that alread exist." << std::endl;
            return;
        }
    }
    
    submodules.push_back(module);
}

void Executive::remove_module(int id)
{
    
}

void Executive::change_state(ExecutiveState newState)
{
    currentState = newState;
}

void Executive::end_simulation()
{
   change_state(ExecutiveState::END);

   // Remove all modules

   // Close all communications
}

void Executive::request_simulation_end()
{
    std::cout << "End Simulation requested..." << std::endl;
    currentState = ExecutiveState::END;
}
void Executive::execute_cycle()
{
    using namespace std::chrono;
    auto start = steady_clock::now();

    // Execute modules for read/write
    for (const auto module : submodules) module->read();
    for (const auto module : submodules) module->write();
    for (const auto module : submodules) module->update();


    auto end = steady_clock::now();
    auto processing_time = end - start;

    

    // Determine if we need to wait to meet the minimum cycle duration
    auto min_duration = duration_cast<steady_clock::duration>(cycle_min_duration);

    if (processing_time > min_duration)
    {
        std::cout << "Warning! Cycle time exceeded: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(processing_time).count()
                  << " ms" << std::endl;
    }

    if (processing_time < min_duration) {
            auto wait_time = min_duration - processing_time;
            // Log sleep time
            // using double_millis = duration<double, std::milli>;
            // auto wait_ms = duration_cast<double_millis>(wait_time);
            // std::cout << "Cycle " << current_cycle << " sleep time: " << wait_ms.count() << "ms\n";
            std::this_thread::sleep_for(wait_time); 
        }

    // Measure total cycle time (processing + sleep)
    auto cycle_end = steady_clock::now();
    auto total_cycle_time = cycle_end - start;

    // Display execution time in milliseconds with high precision
    using double_millis = duration<double, std::milli>;
    auto elapsed_ms = duration_cast<double_millis>(total_cycle_time);
    // std::cout << "Cycle " << current_cycle << " execution time: " << elapsed_ms.count() << "ms\n";
    // std::cout << "----- CYCLE ----- " << current_cycle << std::endl;
 
    // Update cycle count and total time
    current_cycle++;
    total_time += std::max(total_cycle_time, min_duration);
}
