#include "FastDomain.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <FastDDS/Participant/FastParticipant.h>
#include <FastDDS/Participant/FastParticipantQoS.h>

#include <algorithm>

using namespace indradds;
using namespace eprosima::fastdds::dds;

FastDomain::FastDomain(int id) : IDomain(id) {}

FastDomain::~FastDomain() {
    shutdown_all();
}

void FastDomain::init() {

    // 0. Load default configuration
    DomainParticipantQos initial_qos = DomainParticipantFactory::get_instance()->get_default_participant_qos();

    // 1. Load Set QoS Configurationç
    auto participant_qos = DomainParticipantQos(PARTICIPANT_QOS_DEFAULT);
    auto default_qos = FastParticipantQoS(&participant_qos);

    // 2. Apply QoS Configuration
    default_qos.set_user_data("indra_participant");
    std::cout<< "Load Participant QoS configuration..." << std::endl;

    // 3. Set Config as Default Participant QoS 
    change_default_participant_qos(default_qos);
    std::cout<< "Set default Participant QoS in DomainParticipantFactory..." << std::endl;
}
void FastDomain::close() {
    shutdown_all();
}

IParticipant* FastDomain::create_participant() {
    auto participant = std::make_unique<FastParticipant>();
    IParticipant* ptr = participant.get(); 
    participants_.push_back(std::move(participant));
    return ptr; 
}

void FastDomain::remove_participant(IParticipant* participant) {
   
    // 1 Search for the Participant
    auto it = std::remove_if(participants_.begin(), participants_.end(),
        [participant](const std::unique_ptr<IParticipant>& p) {
            return p.get() == participant;
        });
    
    // 2 If founded, remove
    if (it != participants_.end()) {
        (*it)->disable(); // Optional cleanup before removal
        participants_.erase(it, participants_.end()); // This calls the destructor
    }
}

void indradds::FastDomain::change_initial_participant_qos()
{
    // Resetting the default DomainParticipantQos to the original default constructed values
    if (DomainParticipantFactory::get_instance()->set_default_participant_qos(PARTICIPANT_QOS_DEFAULT)
            != RETCODE_OK)
    {
        // Error
        return;
    }
}

void indradds::FastDomain::change_default_participant_qos(IQoS &default_qos)
{
   try {

    auto& part_qos = static_cast<FastParticipantQoS&>(default_qos);
    auto result = 
        DomainParticipantFactory::get_instance()->set_default_participant_qos(*part_qos.get_native_qos());
    
    if(result != RETCODE_OK )
    {
        std::cerr << "Error trying to change default participant quality of service." << std::endl;
    }

    } catch (const std::bad_cast& e) {
        std::cerr << "Invalid QoS type passed to FastDomain: " << e.what() << std::endl;
    }
}

void FastDomain::enable_all() {
    for (auto& p : participants_) {
        p->enable();
    }
}

void FastDomain::shutdown_all() {
    for (auto& p : participants_) {
        p->disable();
    }
    participants_.clear();
}

void FastDomain::reset() {
    change_initial_participant_qos();
    shutdown_all();
}