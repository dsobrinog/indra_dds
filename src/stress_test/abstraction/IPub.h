#pragma once

#include "ITopic.h"

class ParticipantManager;

template <typename T>
class IPub
{
public:
    virtual ~IPub() = default;

    virtual void set_participant_manager(ParticipantManager* participant_manager){}
    virtual bool init() = 0;
    virtual bool publish(T& instance) = 0;
    virtual bool publish_loan(size_t count) = 0;

    virtual void set_expected_subs(int expected){
        messages_per_cycle = expected;
    }
    virtual bool is_matched() = 0;
    
    int messages_per_cycle = 0;
};
