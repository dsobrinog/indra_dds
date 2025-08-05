#pragma once

#include <IQoS.h>
#include <dds_definitions.h>

class IEntity
{
public: 
    // void set_active(bool value);

    // virtual void on_enable() = 0;
    // virtual void on_disable() = 0;

    // // Quality of Service
    // virtual void set_qos(IQoS _qos) = 0;
    // virtual void reset_qos() = 0;

    // // Listeners (Async nostification)
    // //      init we add the custom listeners to the current entity
    // //      or later with this function
    // virtual void set_listener() = 0;

protected:

    DDSStatus entity_status = DDSStatus::NOT_INITIALIZED;


    // Set of Status Objects = communication status on comms events
    
    // StatusCondition
};