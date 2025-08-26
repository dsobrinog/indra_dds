#include <indra_dds/FastDDS/Participant/FastParticipant.h>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>

#include <indra_dds/FastDDS/Publisher/FastPublisher.h>
#include <indra_dds/FastDDS/Subscriber/FastSubscriber.h>
#include <indra_dds/FastDDS/Topic/FastTopic.h>
#include <indra_dds/FastDDS/TypeSupport/FastTypeSupport.h>

using namespace indradds;
using namespace eprosima::fastdds::dds;


FastParticipant::FastParticipant(): IParticipant()
{
    // Creation of Fast DDS Domain Participant
    on_init();
}

FastParticipant::~FastParticipant()
{
    on_destroy();
}

void FastParticipant::update()
{
    if(!is_enabled()) return;


}

void FastParticipant::on_init()
{

    // Fast dds native QoS
    DomainParticipantQos participant_qos;

    // Initial QoS (Non variable)
    // DomainParticipantQos participant_qos(PARTICIPANT_QOS_DEFAULT);

    // Default (User custom)
    DomainParticipantFactory::get_instance()->get_default_participant_qos(participant_qos);

    qos_ = new FastParticipantQoS(&participant_qos);

    // Apply inmutable Quality of Service configuration
    intitialize_qos();

    // Creation of domain participant
    participant_ = DomainParticipantFactory::get_instance()->create_participant(
        0,                       // domain id
        participant_qos,         // QoS con valores por defecto
        nullptr,                 // listener
        StatusMask::all()        // mask para todos los eventos
    );

    if(participant_ == nullptr)
    {
        std::cerr << "Error creating FastParticipant" << std::endl; 
        DDSStatus::ERROR;
        return;
    }

    std::cout << "FastParticipant created!" << std::endl; 
}

void indradds::FastParticipant::intitialize_qos()
{
    // Apply all Quality of Services configuration before enable the entity
    // Once enable, inmutable qualities can not be edited.
}

void FastParticipant::on_destroy()
{
    // clean qos_helper
    delete qos_;

    // delete participant from domain
    DomainParticipantFactory::get_instance()->delete_participant(participant_);
}



void FastParticipant::enable()
{
    // Logic with QoS on DDS Participant
    enabled = true;
}

void FastParticipant::disable()
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

ISubscriber *FastParticipant::create_sub(ITopic& topic, ITypeSupport& type)
{
    // TO DO: USE GENERIC FACTORY

    // auto& fastType = dynamic_cast<FastTypeSupport&>(type);

    // auto sub = std::make_unique<FastSubscriber>(
    //     participant_,                  // Fast DDS raw pointer
    //     topic.GetName(),               // Nombre del topic
    //     fastType.Get()                 // Tipo concreto para FastDDS
    // );
    
    // ISubscriber* sub_ptr = sub.get();
    // subscribers_.emplace_back(std::move(sub));
    // return sub_ptr;

    return nullptr;
}

ITopic *FastParticipant::create_topic(const std::string& topic_name, const std::string& type_name)
{
    // Create the DDS Topic
    Topic* raw_topic = participant_->create_topic(
        topic_name,
        type_name,
        TOPIC_QOS_DEFAULT
    );

    if (!raw_topic)
    {
        std::cerr << "Failed to create topic: " << topic_name << std::endl;
        return nullptr;
    }

    // Wrap the raw topic in FastTopic abstraction
    auto topic = std::make_unique<FastTopic>(raw_topic);
    FastTopic* topic_ptr = topic.get();

    // Store for lifetime management
    topics_.emplace_back(std::move(topic));

    return topic_ptr;
}

// QoS
void FastParticipant::apply_qos()
{
    if (participant_)
        participant_->set_qos(participant_qos);
}

void indradds::FastParticipant::apply_qos(eprosima::fastdds::dds::DomainParticipantQos &qos)
{
     if (participant_)
        participant_->set_qos(qos);
}

const DomainParticipantQos& FastParticipant::get_qos()
{
    return participant_->get_qos();
}


