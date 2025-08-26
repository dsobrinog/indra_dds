#include "module.h"
#include "executive.h"

cl_module::cl_module(int _moduleId, Executive *_exec)
{
    moduleId = _moduleId;
    exec = _exec;
}

void cl_module::run()
{
    if(first_execution())
        read();
    else
        write();

    prev_cycle = exec->get_current_cycle();
}

bool cl_module::first_execution()
{
    return exec->get_current_cycle() != prev_cycle;
}
