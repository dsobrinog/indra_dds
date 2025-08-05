#include <FastParticipant.h>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>

#include <indra_dds/Fast-DDS-API/FastPublisher.h>
#include <indra_dds/Fast-DDS-API/FastSubscriber.h>
#include <indra_dds/Fast-DDS-API/FastTopic.h>
#include <indra_dds/Fast-DDS-API/FastTypeSupport.h>

using namespace indraDDS;
using namespace eprosima::fastdds::dds;



FastParticipant::FastParticipant(): IParticipant()
{
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
    participant_qos = DomainParticipantQos();
    qos_ = new FastParticipantQoS(&participant_qos);

    // Apply inmutable Quality of Service configuration
    intitialize_qos();

    participant_ = DomainParticipantFactory::get_instance()->create_participant(
        0,                       // domain id
        participant_qos,         // QoS con valores por defecto
        nullptr,                 // listener
        StatusMask::all()        // mask para todos los eventos
    );

    if(participant_ == nullptr)
    {
        std::cerr << "Error initializing FastParticipant" << std::endl; 
        DDSStatus::ERROR;
        return;
    }

    std::cout << "FastParticipant created!" << std::endl; 
}

void indraDDS::FastParticipant::intitialize_qos()
{
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

