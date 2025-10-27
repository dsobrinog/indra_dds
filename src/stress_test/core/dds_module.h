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

    void set_expected_messages(int messages){
        num_messages = messages;
    }
    void set_tests(int tests){
        num_tests = tests;
    }

    void set_mode(bool mode){
        manual_mode = mode;
    }

    void set_loan(bool loan){
        loan_mode = loan;
    }

    // Distributions for multiple subscribers
    Distribution lost_samples;

private:
    int _mode = -1;
    int num_messages = 100;
    int num_tests = 25;
    bool manual_mode = false;
    bool loan_mode = false;
};


#endif