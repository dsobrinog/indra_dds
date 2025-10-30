#pragma once

#include "ITopic.h"

template <typename T>
class IPub
{
public:
    virtual ~IPub() = default;

    virtual bool init() = 0;
    virtual bool publish(T& instance) = 0;
    virtual bool publish_loan(size_t count) = 0;

    virtual void set_expected_subs(int expected){
        messages_per_cycle = expected;
    }
    virtual bool is_matched() = 0;
    
    int messages_per_cycle = 0;
};
