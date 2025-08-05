#pragma once

#include <memory>

#include <dds_definitions.h>
#include <IEntity.h>

#include <IPublisher.h>
#include <ISubscriber.h>
#include <ITopic.h>
#include <ITypeSupport.h>

#include <vector>
#include <memory>

// A participant contains pub/sub and manage topics.
class IParticipant : public IEntity
{
public:
    IParticipant(){};


    virtual void update() = 0;

    virtual void enable() = 0;
    virtual void disable() = 0;
    inline virtual bool is_enabled() const = 0;


    // Create Publisher
    virtual IPublisher* create_pub() = 0;

    // Create Subscriber
    virtual ISubscriber* create_sub(ITopic& topic, ITypeSupport& type) = 0;

    // Create Topic
    virtual ITopic* create_topic(const std::string& topic_name, const std::string& type_name) = 0;

    // Delete pub/sub/topic?


  
protected:
    virtual void on_init() = 0;
    virtual void on_destroy() = 0;

    virtual void apply_qos() = 0;

    std::vector<std::unique_ptr<IPublisher>> publishers_;
    std::vector<std::unique_ptr<ISubscriber>> subscribers_;
    std::vector<std::unique_ptr<ITopic>> topics_;

    bool enabled = false;
};