#pragma once

#ifdef USE_CYCLONE_DDS

#include "abstraction/ISubControl.h"

class CycloneSubControl : public ISubControl
{
public:
    CycloneSubControl(){
        std::cout<< "CycloneSubControl created!" << std::endl;
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