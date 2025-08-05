#pragma once

#include "Core/Subscriber/ISubscriber.h"
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/topic/Topic.hpp>
   
#include <mutex>
#include <queue>
#include <condition_variable>

namespace indradds
{
    template<typename T>
    class FastSubscriber : public ISubscriber, public eprosima::fastdds::dds::DataReaderListener
    {
    public:
        FastSubscriber(eprosima::fastdds::dds::DomainParticipant* participant,
                       std::string& topicName,
                       const eprosima::fastdds::dds::TypeSupport& typeSupport);

        ~FastSubscriber() = default;

        void start() override;
        void stop() override;
        void update() override;

        bool read_next(T& out) override;
        void on_data_available(eprosima::fastdds::dds::DataReader* reader) override;

    private:
        std::string topicName_;
        eprosima::fastdds::dds::DomainParticipant* participant_;
        eprosima::fastdds::dds::TypeSupport typeSupport_;

        eprosima::fastdds::dds::Topic* topic_ = nullptr;
        eprosima::fastdds::dds::Subscriber* subscriber_ = nullptr;
        eprosima::fastdds::dds::DataReader* reader_ = nullptr;

        bool started_ = false;
        std::queue<T> messageQueue_;
        std::mutex mutex_;
        std::condition_variable cond_;
    };
}

#include "FastSubscriber.tpp"


