// ctrl_pub.hpp
#pragma once
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>


#include <IDLs/generated/test_control/test_controlPubSubTypes.hpp>

using namespace eprosima::fastdds::dds;

class PubControl
{
public:
    PubControl()
        : participant_(nullptr), publisher_(nullptr), topic_(nullptr), writer_(nullptr),
          type_(new TestControlPubSubType()) {}

    ~PubControl()
    {
        if (writer_) publisher_->delete_datawriter(writer_);
        if (publisher_) participant_->delete_publisher(publisher_);
        if (topic_) participant_->delete_topic(topic_);
        if (participant_) DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    bool init(int domain = 0)
    {
        DomainParticipantQos pq;
        pq.name("CtrlPubParticipant");
        pq.wire_protocol().builtin.discovery_config.leaseDuration = c_TimeInfinite;

        participant_ = DomainParticipantFactory::get_instance()->create_participant(domain, pq);
        if (!participant_) return false;

        type_.register_type(participant_);
        topic_ = participant_->create_topic("TestControlTopic", "TestControl", TOPIC_QOS_DEFAULT);
        if (!topic_) return false;

        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
        auto pqos = publisher_->get_qos();

        publisher_->set_qos(pqos);

        if (!publisher_) return false;

        DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
        wqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
        wqos.reliability().max_blocking_time = {5, 0}; // small block
        wqos.history().kind = KEEP_LAST_HISTORY_QOS;
        wqos.history().depth = 1000; // only need latest control messages
        wqos.durability().kind = TRANSIENT_LOCAL_DURABILITY_QOS;

         // More generous resource limits
        wqos.resource_limits().max_instances = -1;
        wqos.resource_limits().max_samples_per_instance = -1;
        wqos.resource_limits().max_samples = -1;
        wqos.resource_limits().allocated_samples = 1001; // Pre-allocate

        writer_ = publisher_->create_datawriter(topic_, wqos, &listener_);
        if (!writer_) return false;
        return true;
    }

    bool is_matched() const { return listener_.matched_ > 0; }

    bool publish_start(int testId, int expected, int cycleMs)
    {
        TestControl msg;
        msg.test_id() = testId;
        msg.start() = true;
        msg.expected_entities() = expected;
        msg.cycle_time_ms() = cycleMs;

        // Debug output
        std::cout << "[DEBUG] Publishing START control message:\n"
                << "  test_id = " << msg.test_id() << "\n"
                << "  start = " << (msg.start() ? "true" : "false") << "\n"
                << "  expected_entities = " << msg.expected_entities() << "\n";

        auto ret = writer_->write(&msg);
        if(ret != RETCODE_OK)
            std::cerr << "[ERROR] Failed to write START control message. RETCODE=" << ret << std::endl;
        return ret == RETCODE_OK;
    }

    bool publish_stop(int testId)
    {
        TestControl msg{};
        msg.test_id() = testId;
        msg.start() = false;

        // Debug output
        std::cout << "[DEBUG] Publishing STOP control message:\n"
                << "  test_id = " << msg.test_id() << "\n"
                << "  start = " << (msg.start() ? "true" : "false") << "\n";

        auto ret = writer_->write(&msg);
        if(ret != RETCODE_OK)
            std::cerr << "[ERROR] Failed to write STOP control message. RETCODE=" << ret << std::endl;
        return ret == RETCODE_OK;
    }

private:
    class Listener : public DataWriterListener
    {
    public:
        Listener(): matched_(0){}

        std::atomic_int matched_{0};
        void on_publication_matched(DataWriter*, const PublicationMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                std::cout << "PublisherControl matched." << std::endl;
                ++matched_;

            }
            else if (info.current_count_change == -1)
            {
                std::cout << "PublisherControl unmatched." << std::endl;
                --matched_;
            }
        }

    } listener_;

    DomainParticipant* participant_;
    Publisher* publisher_;
    Topic* topic_;
    DataWriter* writer_;
    TypeSupport type_;
};
