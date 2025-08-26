#ifndef CORE_H
#define CORE_H

#include <vector>
#include <cstdint>
#include <iostream>

#include "module.h"

#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

constexpr int CYCLE_TIME = 10000;

enum class ExecutiveState
{
    INIT        = 0,
    EXECUTION   = 1,
    END         = 2
};

class Executive
{
    public:
        Executive(){};
        ~Executive(){};

        void init();
        
        const void add_module(cl_module* module);
        void remove_module(int id);
        const cl_module* get_module(int id);
        const cl_module* get_module(std::string name);

        // TIME MANAGEMENT AND EXECUTION
        void execute_cycle();

        std::chrono::milliseconds cycle_min_duration{10};    // 10 miliseconds
        std::chrono::steady_clock::duration total_time{0};

        // total time in milliseconds
        inline auto get_total_time() const {return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(total_time);}
            
        const inline uint64_t       get_current_cycle()     { return current_cycle;     }
        const inline double         get_operation_time()    { return operation_time;    }    
        const inline uint32_t       get_execution_cycle()   { return execution_cycle;   }
        const inline uint32_t       get_simulation_cycle()  { return simulation_cycle;  }

                      // Solicita Operacion

        // STATES
        ExecutiveState currentState = ExecutiveState::INIT;
        void change_state(ExecutiveState newState);
        inline bool in_execution(){ return currentState == ExecutiveState::EXECUTION;}
        void end_simulation();

        // MODES
        void request_simulation_end();              // Solicita fin de Simulacion
        void request_mission_initialization();      // Solicita Reiniciar la Simulacionn
        void request_simulation_reinitialization(); // Solicita Reiniciar la Simulacion
        void request_operation();  

    private:

        std::vector<cl_module*> submodules;
        uint64_t current_cycle = 0;

        uint64_t operation_time     = 0;
        uint64_t total_mission_time = 0;
        uint64_t simulation_cycle   = 0;
        uint64_t execution_cycle    = 0;
};

#endif