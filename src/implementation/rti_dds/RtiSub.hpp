#pragma once

#ifdef USE_RTI_DDS

#include "abstraction/ISub.h"

class RtiSub : public ISub
{
public:
    RtiSub(){
        std::cout<< "RtiSub created!" << std::endl;
    }
    bool init()
    {
        return true;
    }
    bool run_without_loan(int& lost_samples)
    {
        return true;
    }
    bool run_with_loan(int& lost_samples){

        return true;
    }

    // Reset ids and accumulated samples
    void reset_samples(){
        received_ids.clear();
    }
    // Check connection with pub
    bool is_match(){
        return false;
    }
};

#endif