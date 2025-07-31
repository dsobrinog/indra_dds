#include <iostream>

#include <dds_definitions.h>

#include <DDSFactory.h>
#include <IDomain.h>
#include <IParticipant.h>


// ABSTRACTION LAYER ONLY ( Don't try to call Fast DDS directly )

int main()
{
    std::cout << "Start Simulation" << std::endl;

    auto domain = DDSFactory::CreateDomain(DDSLibrary::FAST_DDS, 0);
    domain->Init();

    auto participant = domain->CreateParticipant();
    participant->Enable();

    IPublisher* pub = participant->create_pub();
    ISubscriber* sub = participant->create_sub();
    ITopic* topic = participant->create_topic();

    participant->Disable();

    domain->Close();
    
    std::cout << "End Simulation" << std::endl;

    return 0;
}
