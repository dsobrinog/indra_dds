#pragma once

#include <dds_definitions.h>

#include <Core/IQoS.h>

#include <memory>
#include <vector>


namespace indradds{

class IParticipant;

// A domain is a common context for all participants so that they can communicate and discover.
class IDomain
{
public:
    IDomain(int id):domain_id(id){}

    virtual void init() = 0;
    virtual void close() = 0;

    // Add IParticipant
    virtual IParticipant* create_participant() = 0;

    virtual void change_default_participant_qos(IQoS& default_qos) = 0;

    // Delete IParticipant
    virtual void remove_participant(IParticipant* participant) = 0;

    virtual void enable_all() = 0;
    virtual void shutdown_all() = 0;

    // Reset domain (delete all participants)
    virtual void reset() = 0;

protected:
    // Lista de Participants
    std::vector<std::unique_ptr<IParticipant>> participants_;

    // Domain ID
    int domain_id = -1;
  
};

}