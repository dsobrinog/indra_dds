#pragma once

#include <Core/IDomain.h>


namespace indradds{
    
    class IParticipant;

    class FastDomain : public IDomain
    {
    public:

        FastDomain(int id);
        ~FastDomain();

        void init();
        void close();

        // factory
        IParticipant* create_participant() override;
        void remove_participant(IParticipant* participant) override;

        virtual void change_initial_participant_qos();
        virtual void change_default_participant_qos(IQoS& default_qos);

        void enable_all() override;
        void shutdown_all() override;
        void reset() override;
    };
}

