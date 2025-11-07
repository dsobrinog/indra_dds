
#pragma once
#include <memory>

class ParticipantManager {
public:
    virtual ~ParticipantManager() = default;

    // Returns a pointer/handle to the DDS participant (vendor-specific type)
    virtual void* getParticipant() = 0;

    // Optional: cleanup, shutdown participant
    virtual void shutdown() = 0;
};

// OpenDDSParticipantManager

#ifdef USE_OPEN_DDS
#include <dds/DCPS/DomainParticipantFactoryImpl.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/StaticIncludes.h>
#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DCPS/Service_Participant.h>

class OpenDDSParticipantManager : public ParticipantManager {
private:
    ::DDS::DomainParticipant_var participant_;
    bool debug = false;
public:
    OpenDDSParticipantManager() {
       std::vector<const char*> args = {
            "app",
            "-DCPSInfoRepo", "file:///workspaces/testdds/tools/repo.ior"
        };

        if (debug) {
            args.push_back("-DCPSDebugLevel");
            args.push_back("10");
            args.push_back("-ORBVerboseLogging");
            args.push_back("1");
        }

        int argc = static_cast<int>(args.size());
        participant_ = TheParticipantFactoryWithArgs(argc, const_cast<char**>(args.data()))
                       ->create_participant(
                            0,
                            PARTICIPANT_QOS_DEFAULT,
                            nullptr,
                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(participant_)) {
            throw std::runtime_error("[OpenDDS] Failed to create participant");
        }
    }

    void* getParticipant() override {
        return participant_.in(); // return as void*
    }

    void SetDebugMode(bool value){
        debug = value;
    }

    void shutdown() override {
        // Do not delete if shared — optionally implement
    }
};
#endif

#ifdef USE_FAST_DDS

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <memory>
#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;

class FastDDSParticipantManager : public ParticipantManager
{
private:
    DomainParticipant* participant_ = nullptr;
    bool debug_ = false;

public:
    FastDDSParticipantManager() = default;

    bool Init()
    {
        DomainParticipantQos participantQos = PARTICIPANT_QOS_DEFAULT;

        // Name
        participantQos.name("Participant_FastDDS");

        // Discovery
        participantQos.wire_protocol().builtin.discovery_config.leaseDuration = c_TimeInfinite;

        // Transport: use UDPv4 only
        participantQos.transport().use_builtin_transports = false;
        auto udp_descriptor = std::make_shared<UDPv4TransportDescriptor>();
        udp_descriptor->sendBufferSize = 65500;
        udp_descriptor->maxMessageSize = 65500;
        udp_descriptor->non_blocking_send = true;
        participantQos.transport().user_transports.push_back(udp_descriptor);

        // Create participant
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);
        if (!participant_)
        {
            std::cerr << "[FastDDS] Failed to create participant\n";
            return false;
        }

        if (debug_)
        {
            std::cout << "[FastDDS] Participant created successfully with UDP transport.\n";
        }

        return true;
    }

    void* getParticipant()
    {
        return static_cast<void*>(participant_);
    }

    void SetDebugMode(bool value)
    {
        debug_ = value;
    }

    void shutdown()
    {
        if (participant_)
        {
            DomainParticipantFactory::get_instance()->delete_participant(participant_);
            participant_ = nullptr;

            if (debug_)
            {
                std::cout << "[FastDDS] Participant deleted.\n";
            }
        }
    }
};

#endif

class ParticipantManagerFactory {
public:
    static std::unique_ptr<ParticipantManager> create() {
#ifdef USE_FAST_DDS
    return std::make_unique<FastDDSParticipantManager>();

#elif USE_CYCLONE_DDS
    return std::make_unique<CycloneDDSParticipantManager>();

#elif USE_OPEN_DDS
    return std::make_unique<OpenDDSParticipantManager>(); 

#elif USE_RTI_DDS
    return std::make_unique<RTIParticipantManager>();
#else
        #warning "DDS vendor not defined"
#endif
    }
};
