#pragma once

#include <Core/IDomain.h>

namespace indradds{
    
    class IParticipant;

    class FastDomain : public IDomain
    {
    public:

        FastDomain(int id);
        ~FastDomain();

        // factory

        IParticipant* create_participant() override;
        void remove_participant(IParticipant* participant) override;


        void EnableAll() override;
        void ShutdownAll() override;
        void Reset() override;
    };
}

