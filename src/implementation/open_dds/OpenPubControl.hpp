#pragma once

#ifdef USE_OPEN_DDS
#include "abstraction/IPubControl.h"

class OpenPubControl : public IPubControl
{
public:
    OpenPubControl()
    {
        std::cout<< "OpenPubControl created!" << std::endl;
    }

     ~OpenPubControl(){}

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