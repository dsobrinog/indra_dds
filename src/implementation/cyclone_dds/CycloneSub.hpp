#pragma once

#ifdef USE_CYCLONE_DDS

#include "abstraction/ISub.h"

class CycloneSub : public ISub
{
public:
    CycloneSub(){
        std::cout<< "CycloneSub created!" << std::endl;
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