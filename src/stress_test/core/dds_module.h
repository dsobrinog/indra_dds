#ifndef DDS_H
#define DDS_H

#include "module.h"
#include "common_defs.h"
#include "stats/Distribution.h"

class pattern_base;

class cl_dds : public cl_module
{
public:
    cl_dds(int moduleId, int mode, Executive* _exec);
    cl_dds(test_config config, Executive* _exec);
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

    // Distributions for multiple subscribers
    Distribution lost_samples;

private:
    test_config config;
};


#endif