#pragma once

#include "pattern_base.h"

#include "stress_test/pub_test.hpp"
#include "stress_test/pub_control.hpp"

#include "stress_test/core/executive.h"

class pub_test_1_to_1 : public pattern_base
{
public:
    pub_test_1_to_1(cl_dds* dds) : pattern_base(dds), exec(dds->exec){};

    Executive* exec;

    PubTest pub;
    PubControl pub_control;

    Distribution cpu_times;


    std::vector<AirEntity> pre_allocated_entities;
    LoanableSequence<AirEntity> pre_allocated_seq;

    enum ProcessType{ Normal = 1, Loan = 2} process_type = ProcessType::Loan;

    bool reset;
    int current_cyles_reset = 0;
    int offset_cyles_reset = 5;

    bool send = false;
    int  cycleMs = 10;

    enum class State { WaitingMatch, Announce, Streaming } state = State::WaitingMatch;

    // entity id
    int current_entity_id = 0;

    void init()
    {
        // Data publisher
        PublisherQos pubQos = PUBLISHER_QOS_DEFAULT;
        DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;

        wqos.history().kind = KEEP_LAST_HISTORY_QOS;
        wqos.history().depth = 100;
        wqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
        wqos.durability().kind = VOLATILE_DURABILITY_QOS;
        wqos.resource_limits().max_instances = 100000;
        wqos.resource_limits().max_samples_per_instance = 1;
        wqos.resource_limits().max_samples = 100000;
        wqos.resource_limits().allocated_samples = 100000;

        if(pub.init(pubQos, wqos))
            std::cout<< "Starting publisher..." << std::endl;
        else
            std::cout << "Failed start publisher" << std::endl;

        // Control publisher
        if (pub_control.init())
            std::cout<< "Starting publisher control..." << std::endl;
        else
            std::cout << "Failed start publisher control" << std::endl;

        test_command();
        pre_allocated_entities.resize(pub.messages_per_cycle);
        
        current_test = 1;

        cpu_times.set_distribution_name("CPU TIME");
    }

    void test_command()
    {
        int number = 0; // Default command

        // PROCESS DATA SELECTION
        std::cout << "Process data type: (1) Normal, (2) Loan Mode";
        std::cin >> number;

        switch (number)
        {
        case ProcessType::Normal:
            process_type = ProcessType::Normal;
            std::cout << "Process type: Normal selected" << std::endl;
            break;
        case ProcessType::Loan:
            process_type = ProcessType::Loan;
            std::cout << "Process type: Loan selected" << std::endl;
            break;
        default:
            std::cout << "invalid command" << std::endl; test_command();
            break;
        }

        // Q ENTITY SELECTION
        std::cout << "How many Air Entities to send each cycle? ";
        std::cin >> number;

        if (std::cin.fail())  // check for invalid input (non-numeric)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Invalid input, using default 0." << std::endl;
            number = 0;
        }

        pub.messages_per_cycle = number;
        send = true;
        state = State::Announce;
    }
    
    // 1... Announce: Send Cntl Test
    // 2... Streaming: Send Data
    // 3... Finish Test
    // 4... Reset 5 cycles
    // 5... 
    void read()
    {
        if(reset)
        {
            current_cyles_reset++;
            if (current_cyles_reset > offset_cyles_reset)
            {
                current_cyles_reset = 0;
                reset=false;

                if (state == State::Announce)
                {
                    send = true;
                }

                if (state == State::Streaming)
                {
                    reset_test();
                }
            }
        }
    };

    void write()
    {
        if(!send) return;
        if(!pub.is_matched() || !pub_control.is_matched()) return;

        if (state == State::Announce)
        {
            pub_control.publish_start(current_test, pub.messages_per_cycle, cycleMs);
            state = State::Streaming;
            return;
        }

        if(state == State::Streaming)
        {
            auto t0 = std::chrono::steady_clock::now();

            // OPTION 1: NORMAL
            if(process_type == ProcessType::Normal)
            {
                for (size_t i = 0; i < pub.messages_per_cycle; i++)
                {
                    pre_allocated_entities[i].id() = current_entity_id++;
                    pub.publish(pre_allocated_entities[i]);
                }
            }
            // OPTION 2: LOANS
            else if(process_type == ProcessType::Loan)
            {
                pub.publish_loan(pub.messages_per_cycle);
            }
            
            auto t1 = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(t1 - t0);
            std::cout << "Messages sent this cyle: " << pub.messages_per_cycle << std::endl;
            std::cout<< "CPU PROCESS TIME: " << elapsed.count() << std::endl;

            // CPU Distribution
            cpu_times.addValue(elapsed.count());

            reset = true;
            send = false;

        }
    }


    void update(){};

    void on_test_finished(bool value)
    {

    };

    void reset_test()
    {
        if(current_test <= max_test)
        {
            pub_control.publish_stop(current_test);
            current_entity_id = 0;
            state = State::Announce;
            reset = true;
        }
       
        finish_test(true);
    };
    void make_final_report()
    {
        cpu_times.printReport();
    };
   
};