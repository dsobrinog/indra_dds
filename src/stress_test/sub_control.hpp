// ctrl_sub.hpp
#pragma once
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <IDLs/generated/test_control/test_controlPubSubTypes.hpp>
#include <atomic>
#include <optional>

using namespace eprosima::fastdds::dds;

struct TestConfig
{
    int testId = 0;
    int expectedEntities = 0;
    int cycleMs = 10;
    bool active = false;
};

class SubControl
{
public:
    SubControl()
        : participant_(nullptr), subscriber_(nullptr), topic_(nullptr), reader_(nullptr),
          type_(new TestControlPubSubType()) {}

    ~SubControl()
    {
        if (reader_) subscriber_->delete_datareader(reader_);
        if (subscriber_) participant_->delete_subscriber(subscriber_);
        if (topic_) participant_->delete_topic(topic_);
        if (participant_) DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    bool init(int domain = 0)
    {
        DomainParticipantQos pq;
        pq.name("CtrlSubParticipant");
        pq.wire_protocol().builtin.discovery_config.leaseDuration = c_TimeInfinite;

        participant_ = DomainParticipantFactory::get_instance()->create_participant(domain, pq);
        if (!participant_) return false;

        type_.register_type(participant_);
        topic_ = participant_->create_topic("TestControlTopic", "TestControl", TOPIC_QOS_DEFAULT);
        if (!topic_) return false;

        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
        if (!subscriber_) return false;

        DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
        rqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
        rqos.durability().kind  = TRANSIENT_LOCAL_DURABILITY_QOS;
        rqos.history().kind     = KEEP_LAST_HISTORY_QOS;
        rqos.history().depth = 1001; // Aumentar profundidad

        // Aumentar límites de recursos
        rqos.resource_limits().max_instances = -1;
        rqos.resource_limits().max_samples_per_instance = -1;
        rqos.resource_limits().max_samples = -1;
        rqos.resource_limits().allocated_samples = 1001;

        reader_ = subscriber_->create_datareader(topic_, rqos, &listener_);
        if (!reader_) return false;

        return true;
    }

    bool has_match() const { return listener_.matched_ > 0; }

    bool poll_control(TestConfig& out)
    {
        TestControl ctrl;
        SampleInfo info;

        bool applied = false;
        while (reader_->take_next_sample(&ctrl, &info) == RETCODE_OK)
        {
            if (!info.valid_data) continue;

            last_.testId = ctrl.test_id();
            last_.expectedEntities = static_cast<int>(ctrl.expected_entities());
            last_.cycleMs = static_cast<int>(ctrl.cycle_time_ms());
            last_.active = ctrl.start();
            out = last_;
            applied = true;
        }

        return applied;
    }

    const TestConfig& current() const { return last_; }

private:
    class Listener : public DataReaderListener
    {
    public:
        std::atomic_int matched_{0};
        void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& s) override
        {
            if (s.current_count_change == 1) ++matched_;
            else if (s.current_count_change == -1) --matched_;
        }
    } listener_;

    DomainParticipant* participant_;
    Subscriber* subscriber_;
    Topic* topic_;
    DataReader* reader_;
    TypeSupport type_;
    TestConfig last_{};
};
