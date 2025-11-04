#pragma once

#ifdef USE_RTI_DDS
#include "abstraction/IPubControl.h"

class RtiPubControl : public IPubControl
{
public:
    RtiPubControl()
    {
        std::cout<< "RtiPubControl created!" << std::endl;
    }

     ~RtiPubControl(){}

    bool init()
    {
        return true;
    }
    bool publish_start(int testId, int expected, bool loan)
    {
        return true;
    }
    bool publish_stop(int testId)
    {
        return true;
    }

    bool is_matched() const
    {
        return false;
    }
};

#endif