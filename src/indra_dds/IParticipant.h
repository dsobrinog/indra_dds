#pragma once

#include <memory>

#include <dds_definitions.h>
#include <IDDSEntity.h>

#include <IPublisher.h>
#include <ISubscriber.h>
#include <ITopic.h>

#include <vector>
#include <memory>

// A participant contains pub/sub and manage topics.
class IParticipant : public IDDSEntity
{
public:
    IParticipant(){};

    virtual void Enable() = 0;
    virtual void Disable() = 0;
    inline virtual bool IsEnabled() const = 0;

    // Create Publisher
    virtual IPublisher* create_pub() = 0;

    // Create Subscriber
    virtual ISubscriber* create_sub() = 0;

    // Create Topic
    virtual ITopic* create_topic() = 0;

    // Delete pub/sub/topic?

protected:
    virtual void OnInit() = 0;
    virtual void OnDestroy() = 0;

    std::vector<std::unique_ptr<IPublisher>> publishers_;
    std::vector<std::unique_ptr<ISubscriber>> subscribers_;
    std::vector<std::unique_ptr<ITopic>> topics_;

    bool enabled = false;
};