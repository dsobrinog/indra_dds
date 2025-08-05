#pragma once

#include <dds_definitions.h>

#include <memory>
#include <vector>


namespace indradds{

class IParticipant;

// A domain is a common context for all participants so that they can communicate and discover.
class IDomain
{
public:
    IDomain(int id):domain_id(id){}

    void Init() {};
    void Close() { ShutdownAll(); };

    // Add IParticipant
    virtual IParticipant* create_participant() = 0;

    // Delete IParticipant
    virtual void remove_participant(IParticipant* participant) = 0;

    virtual void EnableAll() = 0;
    virtual void ShutdownAll() = 0;

    // Reset domain (delete all participants)
    virtual void Reset() = 0;

protected:
    // Lista de Participants
    std::vector<std::unique_ptr<IParticipant>> participants_;

    // Domain ID
    int domain_id = -1;
  
};

}