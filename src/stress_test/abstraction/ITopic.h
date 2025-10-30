#pragma once

// Topic DDS Abstraction
class ITopic
{
    virtual void set_id(int new_id) = 0;
    virtual int id() = 0;
};