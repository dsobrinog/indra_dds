#pragma once

#include "pattern_base.h"

#include "stress_test/sub_test.hpp"
#include "stress_test/sub_control.hpp"

#include "stress_test/core/executive.h"

class sub_test_1_to_1 : public pattern_base
{
protected:
    SubTest sub;
    SubControl sub_control;

    bool test_active = false;
    int current_test_id = 0;

    int total_lost_samples = 0;

    TestValidation test_validation;
    Distribution latencies;
    Distribution cpu_times;;


    Executive* exec;

    enum ProcessType{ Normal = 1, Loan = 2} process_type = ProcessType::Normal;

    // jitter local
    double get_local_jitter()
    {
        return latencies.localJitter();
    }

public:
    sub_test_1_to_1(cl_dds* dds, int expected_messages, int test) : pattern_base(dds) 
    {
        exec = dds->exec;
        max_test = test;
        sub.expected_messages_cycle = expected_messages;
    }

    virtual void init()
    {
       // Data
        SubscriberQos subQos = SUBSCRIBER_QOS_DEFAULT;
        DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
        rqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
        rqos.history().kind = KEEP_LAST_HISTORY_QOS;
        rqos.history().depth = 100;
        rqos.resource_limits().max_instances = 0;  
        rqos.resource_limits().max_samples = 0; 
        rqos.resource_limits().max_samples_per_instance = 0;
        rqos.resource_limits().allocated_samples = 100000;

        if(sub.init(subQos, rqos))
            std::cout<< "Starting subscriber..." << std::endl;
        else
            std::cout << "Failed start subscriber" << std::endl;

        // Control
        if(sub_control.init())
            std::cout<< "Starting control subscriber..." << std::endl;
        else
            std::cout << "Failed start control subscriber" << std::endl;

        test_command();
        
    }

    virtual void test_command()
    {
       
    }
    
    virtual void read()
    {
        TestConfig test_config;
        if(sub_control.poll_control(test_config))
        {
            current_test_id = test_config.testId;
            sub.expected_messages_cycle = test_config.expectedEntities;
            test_active = test_config.active;
            process_type ? ProcessType::Loan : ProcessType::Normal;

            std::cout << "[CTRL] testId=" << current_test_id
                      << " start=" << (test_active ? "true" : "false")
                      << " expected=" << sub.expected_messages_cycle
                      << " loan active=" << test_config.loan << "\n";
            
            if (test_active)
                return; // Give 1 cycle range to the publisher    
        }

        if (!test_active) return;

        // Begin test
        sub.reset_samples();
        int current_lost_samples = 0;
        auto t0 = std::chrono::steady_clock::now();
        bool result;

        // Process Type
        if (process_type == ProcessType::Normal)
            result = sub.run_without_loan(current_lost_samples);              
        else if(process_type == ProcessType::Loan)
            result = sub.run_with_loan(current_lost_samples);

        // CPU Time
        auto t1 = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(t1 - t0);

        std::cout<< "CPU PROCESS TIME: " << elapsed.count() << " ms" << std::endl;
        cpu_times.addValue(elapsed.count());
        total_lost_samples += current_lost_samples;

        // Finish test
        finish_test(result);
        reset_test();
    };

    virtual void write(){};
    virtual void update(){};

    virtual void reset_test()
    {
        sub.reset_samples();
        test_active = false;
    }
    
    virtual void make_final_report() 
    {
        std::stringstream ss;
        ss << "\n**** FINAL 1:1 REPORT ****\n";
        ss << "NUMBER OF ENTITIES EXPECTED PER CYCLE: " << sub.expected_messages_cycle << "\n";
        ss << "NUMBER OF TESTS RUN: " << current_test << "\n";
        ss << "SUCCESS RATIO: " << test_validation.get_percentage() << "%\n";
        ss << "CPU MEAN TIME: " << cpu_times.mean() << "\n";
        ss << "CPU MAX TIME: " << cpu_times.max() << "\n";
        ss << "TOTAL LOST SAMPLES: " << total_lost_samples << "\n";
        ss << "-----------------------------\n";

        std::cout << ss.str();
    }

    virtual void on_test_finished(bool value)
    {
        test_validation.add_test_result(value ? TestResult::VALID : TestResult::INVALID);
    };

};