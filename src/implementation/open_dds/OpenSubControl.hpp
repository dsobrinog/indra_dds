#pragma once

#ifdef USE_OPEN_DDS

#include "abstraction/ISubControl.h"

class OpenSubControl : public ISubControl
{
public:
    OpenSubControl(){
        std::cout<< "OpenSubControl created!" << std::endl;
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