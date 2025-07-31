#pragma once

#include <ITopic.h>

namespace ddscomms
{
    class FastTopic : public ITopic
    {
        public:
            FastTopic() {} ;
            ~FastTopic() = default;

        std::string GetName() const override { return topic_name; }
        std::string GetTypeName() const override { return type_name;}
    };
}
