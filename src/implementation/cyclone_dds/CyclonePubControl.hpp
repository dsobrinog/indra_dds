#pragma once

#ifdef USE_CYCLONE_DDS

#include "abstraction/IPubControl.h"

#include <dds/dds.hpp>  // Modern C++ API

#include "IDLs_cyclone/generated/test_control/test_control.hpp" 

class CyclonePubControl : public IPubControl
{
private:
    dds_entity_t participant_;
    dds_entity_t topic_;
    dds_entity_t writer_;
public:
    CyclonePubControl()
        : participant_(DDS_ENTITY_NIL),
          topic_(DDS_ENTITY_NIL),
          writer_(DDS_ENTITY_NIL)
    {
        std::cout << "CyclonePubControl created!" << std::endl;
    }

    ~CyclonePubControl()
    {
        if (writer_ > 0) dds_delete(writer_);
        if (topic_ > 0) dds_delete(topic_);
        if (participant_ > 0) dds_delete(participant_);
    }

   
   
    bool init()
    {
        participant_ = dds_create_participant(DDS_DOMAIN_DEFAULT, nullptr, nullptr);
        if (participant_ < 0)
        {
            std::cerr << "[ERROR] Failed to create participant: " << dds_strretcode(-participant_) << std::endl;
            return false;
        }

        auto topic = dds::topic::Topic<TestControl>(participant_, "TestControlTopic");

        if (topic_ < 0)
        {
            std::cerr << "[ERROR] Failed to create topic: " << dds_strretcode(-topic_) << std::endl;
            return false;
        }

        writer_ = dds_create_writer(participant_, topic_, nullptr, nullptr);
        if (writer_ < 0)
        {
            std::cerr << "[ERROR] Failed to create writer: " << dds_strretcode(-writer_) << std::endl;
            return false;
        }

        return true;
    }

    bool publish_start(int testId, int expected, bool loan)
    {
        TestControl msg{};
        msg.test_id() = testId;
        msg.start() = true;
        msg.expected_entities() = expected;
        msg.loan() = loan;

        std::cout << "[DEBUG] Publishing START control message:\n"
                  << "  test_id = " << msg.test_id() << "\n"
                  << "  start = " << (msg.start() ? "true" : "false") << "\n"
                  << "  expected_entities = " << msg.expected_entities() << "\n";

        dds_return_t rc = dds_write(writer_, &msg);
        if (rc != DDS_RETCODE_OK)
        {
            std::cerr << "[ERROR] Failed to write START message: " << dds_strretcode(rc) << std::endl;
            return false;
        }

        return true;
    }

    bool publish_stop(int testId)
    {
        TestControl msg{};
        msg.test_id() = testId;
        msg.start() = false;

        std::cout << "[DEBUG] Publishing STOP control message:\n"
                  << "  test_id = " << msg.test_id() << "\n";

        dds_return_t rc = dds_write(writer_, &msg);
        if (rc != DDS_RETCODE_OK)
        {
            std::cerr << "[ERROR] Failed to write STOP message: " << dds_strretcode(rc) << std::endl;
            return false;
        }

        return true;
    }

    bool is_matched() const
    {
        dds_publication_matched_status_t status;
        dds_return_t rc = dds_get_publication_matched_status(writer_, &status);
        if (rc != DDS_RETCODE_OK)
        {
            std::cerr << "[WARN] Failed to get matched status: " << dds_strretcode(rc) << std::endl;
            return false;
        }

        if (status.current_count_change > 0)
        {
            std::cout << "PublisherControl matched." << std::endl;
        }
        
        return status.current_count > 0;
    }


};

#endif