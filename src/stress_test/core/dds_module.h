#pragma once

#include "module.h"
#include "common_defs.h"
#include "stats/Distribution.h"

#include "abstraction/IPub.h"
#include "abstraction/ISub.h"
#include "abstraction/IPubControl.h"
#include "abstraction/ISubControl.h"


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

    enum class DDS_Library
    {
        FastDDS = 0,
        CycloneDDS = 1,
        OpenDDS = 2,
        RtiDDS = 3
    } current_library = DDS_Library::FastDDS;

private:
    test_config config;
};