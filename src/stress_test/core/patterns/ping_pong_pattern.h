
class ping_pong_pattern : public pattern_base
{
public:
    ping_pong_pattern(cl_dds* dds) : pattern_base(dds), _module(dds) {}

private:
    cl_dds* _module = nullptr;

    int totalSize = 0;
    int entities = 0;

    enum ROLE { STARTER, NON_STARTER } _role;
    enum STATUS { PING, PONG } _status;

    pub_custom pub;
    sub_custom sub;

    int cycles_offset = 5;            
    int max_entities = 10000;    
    const int missed_threadhold = 1;        
    int missed_messages = 0;          
    int handshake_count = 0;
    const int handshake_threshold = 10;
    bool handshake_done = false;

    std::string pub_partition = "";
    std::string sub_partition = "";

    // TEST DATA

    int num_test = 50;
    int current_test = 0;

    Distribution total_msg_distribution;

public:

void init()
    {
        test_command();

        if (_role == ROLE::STARTER)
        {
            pub_partition = "A_to_B";
            sub_partition = "B_to_A";
        }
        else
        {
            pub_partition = "B_to_A";
            sub_partition = "A_to_B";
        }

        // Publisher setup
        PublisherQos pubQos = PUBLISHER_QOS_DEFAULT;
        pubQos.partition().push_back(pub_partition.c_str());
        
        DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
        wqos.history().kind = KEEP_LAST_HISTORY_QOS;
        wqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;


        if(pub.init(pubQos, wqos)) std::cout << "Starting publisher..." << std::endl;
        else std::cout << "Failed start publisher" << std::endl;

        // Subscriber setup
        SubscriberQos subQos = SUBSCRIBER_QOS_DEFAULT;
        subQos.partition().push_back(sub_partition.c_str());
        
        DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
        rqos.history().kind = KEEP_LAST_HISTORY_QOS;
        rqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;


        if(sub.init(subQos, rqos)) std::cout << "Starting subscriber..." << std::endl;
        else std::cout << "Failed start subscriber" << std::endl;
    }
    
    void test_command()
    {
        int number = 0;
        std::cout << "Press 1 (Starter) or 2 (Non-Starter): ";
        std::cin >> number;

        if(std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            number = 1;
        }

        if(number == 1) { _role = ROLE::STARTER; _status = STATUS::PING; }
        else { _role = ROLE::NON_STARTER; _status = STATUS::PONG; }

        pub.messages_per_cycle = 1;
        sub.expected_messages_cycle = 1;
    }

    void read()
    {
        if(!sub.is_match()) return;
        if (_status != STATUS::PONG) return;

        if(sub.run())
        {
            if(!handshake_done)
            {
                handshake_count++;
                if(handshake_count >= handshake_threshold)
                {
                    on_hand_shake();
                }
            }
            else
            {
                entities++;  // Only count real test messages
            }

            pub.messages_per_cycle++;
            sub.expected_messages_cycle++;
            _status = STATUS::PING;
            sub.reset_samples();
        }
        else if(sub.is_match())
        {
            missed_messages++;
        }
    }

    void write()
    {
        if(_status != STATUS::PING) return;
        if(!pub.is_matched()) return;

        if(pub.publish())
        {
            _status = STATUS::PONG;
        }
    }

    void update()
    {
        // Perform handshake until done
        if(!handshake_done)
        {
            // std::cout << "[INFO] Handshake phase: " << handshake_count 
            //           << "/" << handshake_threshold << std::endl;

            missed_messages = 0;
        }

        // Stop condition
        if((handshake_done && (entities >= max_entities || missed_messages >= missed_threadhold)) || 
           (handshake_done && (!pub.is_matched() || !sub.is_match())))
        {
            final_report();
            finish_test();
        }
    }

    void on_hand_shake()
    {
        handshake_done = true;
        missed_messages = 0;
        entities = 0;
        // pub.current_id = 0;
        // pub.messages_per_cycle = 1;
        // sub.expected_messages_cycle = 1;
        std::cout << "Hand-shake done, starting test..." << std::endl;
    }

    void connectivity_info()
    {
        std::cout << "[INFO] Publisher matched: " << pub.is_matched() 
                  << ", Subscriber matched: " << sub.is_match() 
                  << ", Handshake: " << handshake_count << "/" << handshake_threshold
                  << ", Messages received: " << entities
                  << ", Missed: " << missed_messages << std::endl;
    }

    void final_report()
    {
        totalSize = entities * sizeof(AirEntity);
        std::stringstream ss;
        ss << "\n **** FINAL PING PONG REPORT *** \n";
        ss << "TOTAL ENTITIES: " << entities << "\n";
        ss << "TOTAL SIZE: " << totalSize << "\n";
        ss << "MISSED MESSAGES: " << missed_messages << "\n";
        ss << "-----------------------------\n";
        std::cout << ss.str();
    }

    void finish_test() 
    {
        current_test++;

        // Add test result to Distribution

        if (current_test > num_test)
        {
           make_final_report();
        }
        else
        {
            total_msg_distribution.addValue(entities);
            // run test again
            reset_test();
        }
    }

    void reset_test()
    {
        pub.current_id = 0;
        pub.messages_per_cycle = 1;
        sub.expected_messages_cycle = 1;
        entities = 0;
        missed_messages = 0;          
        handshake_count = 0;
        handshake_done = false;
    }

        
    void make_final_report()
    {
        // Make Distribution calculation of mean, max, min, mode...
        // Make Final report
        std::cout << " \n ********* FINAL PING PONG REPORT: ********* \n TOTAL MESSAGE BEFORE MISSING " << missed_threadhold << "ENTITY " << std::endl;
        total_msg_distribution.printReport();
        _module->exec->end_simulation();
    };

    virtual void on_test_finished(bool value)
    {
        
    }
};
