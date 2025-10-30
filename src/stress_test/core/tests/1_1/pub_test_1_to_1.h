#pragma once

#include "pattern_base.h"

#include "abstraction/IPub.h"
#include "abstraction/IPubControl.h"

#include "stress_test/core/executive.h"


template <typename T> 
class pub_test_1_to_1 : public pattern_base
{
public:
    pub_test_1_to_1(cl_dds* dds, test_config config,   std::unique_ptr<IPub<T>> ipub, std::unique_ptr<IPubControl> ipubcontrol) : pattern_base(dds), exec(dds->exec), _config(config),
        pub(std::move(ipub)), pub_control(std::move(ipubcontrol))
    {
        config.manual_mode ? command_type = CommandType::Manual : command_type = CommandType::Auto;
        max_test = config.test_count;
        set_num_messages(config.message_count);
        pub->set_expected_subs(config.listener_number);
        process_type = (ProcessType)config.loan_mode;
    };

    Executive* exec;

    std::unique_ptr<IPub<T>> pub;
    std::unique_ptr<IPubControl> pub_control;

    Distribution cpu_times;
    test_config _config;

    int message_to_send = 100;
    
    std::vector<T> pre_allocated_entities;

    enum CommandType{ Manual = 0, Auto = 1} command_type = CommandType::Manual;
    enum ProcessType{ Normal = 0, Loan = 1} process_type = ProcessType::Loan;

    void SetCommandType(CommandType type){command_type = type; };
    void SetProcessType(ProcessType type){process_type = type; };

    bool reset;
    int current_cyles_reset = 0;
    int offset_cyles_reset = 5;

    bool send = false;

    enum class State { WaitingMatch, Announce, Streaming } state = State::WaitingMatch;

    // entity id
    int current_entity_id = 0;

    void init()
    {
        if(pub->init())
            std::cout<< "Starting publisher..." << std::endl;
        else
            std::cout << "Failed start publisher" << std::endl;

        // Control publisher
        if (pub_control->init())
            std::cout<< "Starting publisher control..." << std::endl;
        else
            std::cout << "Failed start publisher control" << std::endl;


        // if(command_type == CommandType::Manual)
        //     test_command();
        
        pre_allocated_entities.resize(message_to_send);
        current_test = 0;
        cpu_times.set_distribution_name("CPU TIME");
        start_send();
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

        set_num_messages(number);
    }

    void set_num_messages(int num_messages){
        message_to_send = num_messages;
        pub->messages_per_cycle = num_messages;
    }

    void start_send(){
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

    int offset_cycle_start = 50;
    int current_cycle = 0;
    void write()
    {
        if(!send) return;
        if(!pub->is_matched() || !pub_control->is_matched()) return;

        if (state == State::Announce)
        {
            if(current_cycle++ < offset_cycle_start){
                return;
            }

            pub_control->publish_start(current_test + 1, pub->messages_per_cycle, process_type == ProcessType::Loan);
            state = State::Streaming;
            return;
        }

        if(state == State::Streaming)
        {
            auto t0 = std::chrono::steady_clock::now();

            // OPTION 1: NORMAL
            if(process_type == ProcessType::Normal)
            {
                for (size_t i = 0; i < pub->messages_per_cycle; i++)
                {
                    pre_allocated_entities[i].id() = current_entity_id++;
                    pub->publish(pre_allocated_entities[i]);
                }
            }
            // OPTION 2: LOANS
            else if(process_type == ProcessType::Loan)
            {
                pub->publish_loan(pub->messages_per_cycle);
            }
            
            auto t1 = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(t1 - t0);
            std::cout << "Messages sent this cyle: " << pub->messages_per_cycle << std::endl;
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
            pub_control->publish_stop(current_test + 1);
            current_entity_id = 0;
            state = State::Announce;
            reset = true;
        }
       
        finish_test(true);
    };

    bool export_result = true;
    void make_final_report()
    {
        if(export_result)
        {
            export_result = !export_result;
            cpu_times.printReport();
            cpu_times.exportReport(_config.logFile);
        }

        exec->request_simulation_end();
    };
   
};