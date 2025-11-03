#pragma once

#ifdef USE_CYCLONE_DDS

#include "abstraction/IPubControl.h"

class CyclonePubControl : public IPubControl
{
public:
    CyclonePubControl()
    {
        std::cout<< "CyclonePubControl created!" << std::endl;
    }

     ~CyclonePubControl(){}

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