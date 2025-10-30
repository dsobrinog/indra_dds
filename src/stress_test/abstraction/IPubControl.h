#pragma once

class IPubControl
{
public:
    virtual ~IPubControl() = default;

    virtual bool init() = 0;
    virtual bool publish_start(int testId, int expected, bool loan) = 0;
    virtual bool publish_stop(int testId) = 0;
    virtual bool is_matched() const = 0;
};
