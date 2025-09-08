#ifndef DDS_H
#define DDS_H

#include "module.h"

class pattern_base;

class cl_dds : public cl_module
{
public:
    cl_dds(int moduleId, int mode, Executive* _exec);
    ~cl_dds();

    pattern_base* p_pattern = nullptr;

    void init();
    void read();
    void write();
    void update();
    void end();

    // test purpuse
    void main_menu();
    void test_patterns(int mode);
   
private:
    // Mode: -1 (Not), 0 (PUB), 1 (SUB)
    int _mode = -1;
};


#endif