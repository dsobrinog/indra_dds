#ifndef pattern_base_H
#define pattern_base_H

#include "stress_test/core/executive.h"
#include "stress_test/core/dds_module.h"

#include "stress_test/pub_test.hpp"
#include "stress_test/sub_test.hpp"

#include "stress_test/stats/Distribution.h"
#include "stress_test/stats/TestValidation.h"

#include "stress_test/core/tests/test_def.h"

#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#include <sstream>

class pattern_base
{
    public:
        pattern_base(cl_dds* dds) : dds_module (dds){}

        virtual void init() = 0;

        // Tick update
        // 1. Read
        virtual void read() = 0; 
        // 2. Write       
        virtual void write() = 0;
        // 3. Update
        virtual void update() = 0;
        
        // Configurable by input
        virtual void test_command() = 0;

        // Reset test variables
        virtual void reset_test() = 0;
        
        // Make final report
        virtual void make_final_report() = 0;

        // Logic when test is finished
        virtual void on_test_finished(bool value) = 0;
        
        // Finish test
        void finish_test(bool value)
        {
            on_test_finished(value);
            current_test++;
            if(current_test > max_test)
            {
                make_final_report();
                // dds_module->exec->end_simulation();
                return;
            }
        }


    protected:
        cl_dds* dds_module = nullptr;

        // Number of test that will be performed to create stats
        int max_test = 100;
        int current_test = 0;
};

#endif