#pragma once

namespace indraDDS {

    class IAutoEnableFactoryQoS
    {
    public:
        virtual ~IAutoEnableFactoryQoS() = default;

        virtual void set_auto_enable_entities(bool enable) = 0;
    };

}
