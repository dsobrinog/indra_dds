#pragma once

#include <ITopic.h>

#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>

namespace indraDDS
{
    class FastTopic : public ITopic
    {
        public:
            FastTopic(eprosima::fastdds::dds::Topic* topic) : ITopic(topic->get_name(), topic->get_type_name()){};
            ~FastTopic() = default;

            inline std::string GetName() const override { return topic_name; }
            inline std::string GetTypeName() const override { return type_name;}
            inline eprosima::fastdds::dds::Topic* get_raw_topic(){ return dds_topic_;}

        protected:

            eprosima::fastdds::dds::Topic* dds_topic_;

    };
}
