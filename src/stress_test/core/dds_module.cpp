#include "dds_module.h"

#include "patterns/pattern_base.h"
#include "stress_test/core/tests/1_1/pub_test_1_to_1.h"
#include "stress_test/core/tests/1_1/sub_test_1_to_1.h"

#include "stress_test/DDSFactory.h"
#include "stress_test/DDSParticipantManager.h"

// idls fast
#ifdef USE_FAST_DDS
#include "IDLs/generated/inse/inse.hpp"
#endif
// idls cyclone
#ifdef USE_CYCLONE_DDS
#include "IDLs_cyclone/generated/inse/inse.hpp"
#endif

// #include "patterns/ping_pong_pattern.h"

cl_dds::cl_dds(int moduleId, int mode, Executive * _exec) : cl_module(moduleId, _exec)
{
}

cl_dds::cl_dds(test_config _config, Executive* _exec) : cl_module(0, _exec), config(_config)
{
    if(config.manual_mode)
        config.listener_number = 1;
    
}

cl_dds::~cl_dds()
{
    delete participant_factory;
}


void cl_dds::init()
{
    participant_factory = new ParticipantManagerFactory();

    #ifdef USE_FAST_DDS
        current_library = DDS_Library::FastDDS;
    #endif
    #ifdef USE_CYCLONE_DDS
        current_library = DDS_Library::CycloneDDS;
    #endif
    #ifdef USE_OPEN_DDS
        current_library = DDS_Library::OpenDDS;
    #endif
    #ifdef USE_RTI_DDS
        current_library = DDS_Library::RtiDDS;
    #endif

    test_patterns(config.test_mode);
    if(p_pattern)
        p_pattern->init();
}

void cl_dds::read()
{
    p_pattern->read();
}

void cl_dds::write()
{
    p_pattern->write();
}
void cl_dds::update()
{
    p_pattern->update();
}

void cl_dds::end()
{
   
}


void cl_dds::test_patterns(int mode)
{
    int number = 0; // Default command

    if(mode <= 0)
    {
        char input;
        std::cout << "Select test: \n 1) PUB \n 2) SUB \n 3) PUB 1:MANY \n 4) SUB 1:MANY \n";
        std::cin >> input;


        // Check if input is a digit
        if (std::isdigit(input))
        {
            number = input - '0'; // Convert char digit to int
        }
    }
    else
    {
        number = mode;
    }

    auto domain_participant = participant_factory->create();

    switch (number)
    {
        case TestMode::One_to_One_Pub:
        {
            auto pub_control_impl = DDSFactory::createPublisherControl();
            pub_control_impl.get()->set_participant_manager(domain_participant.get());

            // Each nested case must have braces
            switch (current_library)
            {
                case DDS_Library::FastDDS:
                {
                    #ifdef USE_FAST_DDS
                        auto pub_impl = DDSFactory::createPublisher<AirEntity>();
                        p_pattern = new pub_test_1_to_1<AirEntity>(this, config, std::move(pub_impl), std::move(pub_control_impl));
                    #endif
                    break;
                }

                case DDS_Library::CycloneDDS:
                {
                    #ifdef USE_CYCLONE_DDS
                        auto pub_impl = DDSFactory::createPublisher<cyclone_dds::AirEntity>();
                        p_pattern = new pub_test_1_to_1<cyclone_dds::AirEntity>(this, config, std::move(pub_impl), std::move(pub_control_impl));
                    #endif
                    break;
                }

                case DDS_Library::OpenDDS:
                {
                    #ifdef USE_OPEN_DDS
                        auto pub_impl = DDSFactory::createPublisher<::AirEntity>();
                        pub_impl.get()->set_participant_manager(domain_participant.get());
                        p_pattern = new pub_test_1_to_1<::AirEntity>(this, config, std::move(pub_impl), std::move(pub_control_impl));
                    #endif
                    break;
                }
            }

            // Create pattern after nested switch
            break; // break outer case
        }
        case TestMode::One_to_One_Sub:
        {
            // SUB - 
            auto sub_control_impl = DDSFactory::createSubscriberControl();
            sub_control_impl.get()->set_participant_manager(domain_participant.get());

            switch (current_library)
            {
                case DDS_Library::FastDDS:
                {
                    #ifdef USE_FAST_DDS
                        auto sub_impl = DDSFactory::createSubscriber();
                        p_pattern = new sub_test_1_to_1(this, config, std::move(sub_impl), std::move(sub_control_impl));
                    #endif

                    break;
                }

                case DDS_Library::CycloneDDS:
                {
                    #ifdef USE_CYCLONE_DDS
                        auto sub_impl = DDSFactory::createSubscriber();
                        p_pattern = new sub_test_1_to_1(this, config, std::move(sub_impl), std::move(sub_control_impl));
                    #endif

                    break;
                }

                case DDS_Library::OpenDDS:
                {
                    #ifdef USE_OPEN_DDS
                        auto sub_impl = DDSFactory::createSubscriber();
                        sub_impl.get()->set_participant_manager(domain_participant.get());
                        p_pattern = new sub_test_1_to_1(this, config, std::move(sub_impl), std::move(sub_control_impl));
                    #endif

                    break;
                }


            }
            break;
        }
        default:
        test_patterns(number);
            break;
    }
}