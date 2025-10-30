#pragma once

struct TestConfig
{
    int testId = 0;
    int expectedEntities = 0;
    bool loan = false;
    bool active = false;
};

class ISubControl
{
public:
    virtual ~ISubControl() = default;

    virtual bool init() = 0;
    virtual bool poll_control(TestConfig& out) = 0;
    virtual bool has_match() const = 0;
};
