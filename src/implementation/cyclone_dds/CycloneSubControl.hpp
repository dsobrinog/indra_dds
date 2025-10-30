#pragma once

#ifdef USE_CYCLONE_DDS

#include "abstraction/ISubControl.h"

class CycloneSubControl : public ISubControl
{
    bool init(){
        return true;
    }
    bool poll_control(TestConfig& out){
        return true;
    }
    bool has_match() const
    {
        return false;
    }
};

#endif