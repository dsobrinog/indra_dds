#pragma once

#include <unordered_set>

// Subscriber DDS Abstraction
class ISub
{
public: 
    virtual bool init() = 0;

    virtual bool run_without_loan(int& lost_samples) = 0;
    virtual bool run_with_loan(int& lost_samples) = 0;

    // Reset ids and accumulated samples
    virtual void reset_samples() = 0;
    // Check connection with pub
    virtual bool is_match() = 0;

    int expected_messages_cycle = -1;
    std::unordered_set<int> received_ids;
};