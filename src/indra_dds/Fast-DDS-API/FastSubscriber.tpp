#pragma once

#include <FastSubscriber.h>

#include <fastdds/dds/subscriber/InstanceState.hpp>
#include <fastdds/dds/core/ReturnCode.hpp>


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;

namespace indraDDS
{

template<typename T>
FastSubscriber<T>::FastSubscriber(
    eprosima::fastdds::dds::DomainParticipant* participant,
    std::string& topicName,
    const eprosima::fastdds::dds::TypeSupport& typeSupport)
    : topicName_(topicName), participant_(participant), typeSupport_(typeSupport)
{
    using namespace eprosima::fastdds::dds;

    typeSupport_.register_type(participant_);

    topic_ = participant_->create_topic(
        topicName_,
        typeSupport_->get_name(),
        TOPIC_QOS_DEFAULT);

    subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

    DataReaderQos readerQos = DATAREADER_QOS_DEFAULT;

    reader_ = subscriber_->create_datareader(
        topic_,
        readerQos,
        this);
}

template<typename T>
void FastSubscriber<T>::start()
{
    started_ = true;
}

template<typename T>
void FastSubscriber<T>::stop()
{
    started_ = false;
    std::unique_lock<std::mutex> lock(mutex_);
    std::queue<T>().swap(messageQueue_);
}

template<typename T>
bool FastSubscriber<T>::read_next(T& out)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (messageQueue_.empty())
        return false;

    out = messageQueue_.front();
    messageQueue_.pop();
    return true;
}

template<typename T>
void FastSubscriber<T>::on_data_available(eprosima::fastdds::dds::DataReader* reader)
{
    using namespace eprosima::fastdds::dds;

    SampleInfo info;
    T sample;

    while (reader->take_next_sample(&sample, &info) == RETCODE_OK)
    {
        if (info.instance_state == ALIVE_INSTANCE_STATE)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            messageQueue_.push(sample);
            cond_.notify_one();
        }
    }
}

} // namespace indradds
