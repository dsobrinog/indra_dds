#pragma once

#include <Core/Publisher/IPublisher.h>

namespace indradds
{
    class FastPublisher : public IPublisher
    {
        public:
            FastPublisher(){};
            ~FastPublisher() = default;
    };

}