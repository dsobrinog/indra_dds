#ifndef DDS_H
#define DDS_H

#include "module.h"

class pattern_base;

class cl_dds : public cl_module
{
public:
    cl_dds(int moduleId, Executive* _exec);
    ~cl_dds();


    // IFC ZMQ
    void init();
    void read();
    void write();
    void end();

    // test purpuse
    void main_menu();
    void test_patterns();

private:


};


#endif