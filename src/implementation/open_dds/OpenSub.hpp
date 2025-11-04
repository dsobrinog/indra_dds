#pragma once

#ifdef USE_OPEN_DDS

#include "abstraction/ISub.h"

class OpenSub : public ISub
{
public:
    OpenSub(){
        std::cout<< "OpenSub created!" << std::endl;
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