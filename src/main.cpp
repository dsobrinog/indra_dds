#include <iostream>

#include <dds_definitions.h>

#include <DDSFactory.h>
#include <Core/IDomain.h>
#include <Core/Participant/IParticipant.h>


using namespace indradds;
// ABSTRACTION LAYER ONLY ( Don't call Fast DDS directly )

int main()
{
    std::cout << "Start Simulation" << std::endl;

    // Create domain specifie
    auto domain = DDSFactory::CreateDomain(DDSLibrary::FAST_DDS, 0);
    domain->Init();

    // Create participant
    auto participant = domain->create_participant();
    participant->enable();

    // Create pub/sub/topic
    // IPublisher* pub = participant->create_pub();
    // ISubscriber* sub = participant->create_sub();
    // ITopic* topic = participant->create_topic();

    participant->disable();

    domain->Close();
    
    std::cout << "End Simulation" << std::endl;

    return 0;
}
