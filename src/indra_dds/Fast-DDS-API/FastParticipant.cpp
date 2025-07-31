#pragma once

#include <FastParticipant.h>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>

#include <indra_dds/Fast-DDS-API/FastPublisher.h>
#include <indra_dds/Fast-DDS-API/FastSubscriber.h>
#include <indra_dds/Fast-DDS-API/FastTopic.h>

using namespace ddscomms;
using namespace eprosima::fastdds::dds;



FastParticipant::FastParticipant(): IParticipant()
{
   OnInit();
}

FastParticipant::~FastParticipant()
{
   OnDestroy();
}

void FastParticipant::OnInit()
{
    DomainParticipantQos participant_qos;
    participant = DomainParticipantFactory::get_instance()->create_participant(
        0,                       // domain id
        participant_qos,         // QoS con valores por defecto
        nullptr,                 // listener
        StatusMask::all()        // mask para todos los eventos
    );

    if(participant == nullptr)
    {
        std::cerr << "Error initializing FastParticipant" << std::endl; 
        DDSStatus::ERROR;
        return;
    }

    std::cout << "FastParticipant created!" << std::endl; 
}

void FastParticipant::OnDestroy()
{
    DomainParticipantFactory::get_instance()->delete_participant(participant);
}

void FastParticipant::Enable()
{
    // Logic with QoS on DDS Participant
    enabled = true;
}

void FastParticipant::Disable()
{
    if(!enabled) return;

    // Logic with QoS on DDS Participant
    enabled = false;
}

IPublisher *FastParticipant::create_pub()
{
    auto pub = std::make_unique<FastPublisher>();
    IPublisher* pub_ptr = pub.get();
    publishers_.emplace_back(std::move(pub));
    return pub_ptr;
}

ISubscriber *FastParticipant::create_sub()
{
    auto sub = std::make_unique<FastSubscriber>();
    ISubscriber* sub_ptr = sub.get();
    subscribers_.emplace_back(std::move(sub));
    return sub_ptr;
}

ITopic *FastParticipant::create_topic()
{
    auto topic = std::make_unique<FastTopic>();
    FastTopic* topic_ptr = topic.get();
    topics_.emplace_back(std::move(topic));
    return topic_ptr;
}

