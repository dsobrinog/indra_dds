#pragma once

#include <IPublisher.h>

namespace indraDDS
{
    class FastPublisher : public IPublisher
    {
        public:
            FastPublisher(){};
            ~FastPublisher() = default;
    };

}