#pragma once

#include <ISubscriber.h>

namespace ddscomms
{

    class FastSubscriber : public ISubscriber
    {
        public:
            FastSubscriber(){};
            ~FastSubscriber() = default;
    };

}