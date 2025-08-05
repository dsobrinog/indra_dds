#pragma once

#include <IDomain.h>

class IParticipant;

namespace indraDDS{
    
    class FastDomain : public IDomain
    {
    public:

        FastDomain(int id);
        ~FastDomain();

        IParticipant* CreateParticipant() override;
        void RemoveParticipant(IParticipant* participant) override;

        void EnableAll() override;
        void ShutdownAll() override;
        void Reset() override;
    };
}

