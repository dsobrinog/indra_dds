#include <FastDomain.h>
#include <FastParticipant.h>

#include <algorithm>

using namespace ddscomms;

FastDomain::FastDomain(int id) : IDomain(id) {}

FastDomain::~FastDomain() {
    ShutdownAll();
}

IParticipant* FastDomain::CreateParticipant() {
    auto participant = std::make_unique<FastParticipant>();
    IParticipant* ptr = participant.get(); 
    participants_.push_back(std::move(participant));
    return ptr; 
}

void FastDomain::RemoveParticipant(IParticipant* participant) {
   
    // 1 Search for the Participant
    auto it = std::remove_if(participants_.begin(), participants_.end(),
        [participant](const std::unique_ptr<IParticipant>& p) {
            return p.get() == participant;
        });
    
    // 2 If founded, remove
    if (it != participants_.end()) {
        (*it)->Disable(); // Optional cleanup before removal
        participants_.erase(it, participants_.end()); // This calls the destructor
    }
}

void FastDomain::EnableAll() {
    for (auto& p : participants_) {
        p->Enable();
    }
}

void FastDomain::ShutdownAll() {
    for (auto& p : participants_) {
        p->Disable();
    }
    participants_.clear();
}

void FastDomain::Reset() {
    ShutdownAll();
}