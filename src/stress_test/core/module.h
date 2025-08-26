#ifndef MODULE_H
#define MODULE_H

#include <cstdint>
#include <iostream>

class Executive;

class cl_module
{
public:
    cl_module(int moduleId, Executive* _exec);

    virtual void init() = 0;
    
    virtual void run();
    virtual void read() = 0;
    virtual void write() = 0;

    inline int get_module_id(){return moduleId;}

    bool first_execution();
    
protected:
    Executive* exec;

    int moduleId = 0;
    uint64_t prev_cycle = -1;
};

#endif