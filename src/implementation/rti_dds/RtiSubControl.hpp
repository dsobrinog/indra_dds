#pragma once

#ifdef USE_RTI_DDS

#include "abstraction/ISubControl.h"

class RtiSubControl : public ISubControl
{
public:
    RtiSubControl(){
        std::cout<< "RtiSubControl created!" << std::endl;
    }
    bool init(){
        return true;
    }
    bool poll_control(TestConfig& out){
        return false;
    }
    bool has_match() const
    {
        return false;
    }
};

#endif