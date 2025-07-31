#pragma once

#include <IPublisher.h>

namespace ddscomms
{
    class FastPublisher : public IPublisher
    {
        public:
            FastPublisher(){};
            ~FastPublisher() = default;
    };

}