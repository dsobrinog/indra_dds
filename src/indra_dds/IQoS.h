#pragma once

// DOC: https://fast-dds.docs.eprosima.com/en/stable/fastdds/dds_layer/core/policy/standardQosPolicies.html#durabilityserviceqospolicy
enum Standard_QoS_Policy
{
    DEADLINE = 0,
    DESTINATION_ORDER = 1,
    DURABILITY = 2,
    DURABILITY_SERVICE = 3, // Not implemented
    ENITTY_FACTORY = 4,
    GROUP_DATA = 5,
    HISTORY = 6,
    LATENCY_BUDGET = 7,
    LIFESPAN = 8,
    LIFELINESS = 9,
    OWNERSHIP = 10,
    OWNERSHIP_STRENGHT = 11,
    PARTITION = 12,
    PRESENTATION = 13, // to implement
    READER_DATA_LIFECYCLE = 14, // to implement
    RELIABILITY = 15,
    RESOURCE_LIMITS = 16,
    TIME_BASED_FILTER = 17, // to implement
    TOPIC_DATA =18,
    TRANSPORT_PRIORITY = 19, // to implement,
    USER_DATA = 20,
    WRITER_DATA_LIFECYCLE = 21 // to implement
    
};


class IQoS
{

    // Set reliability

    // Set durability

    // Set History

    // Destructor


    virtual void set_qos_id(int id);
    inline int get_qos_id() { return qos_id;}


protected:
    int qos_id = -1;

    // QoS only be specified either at the entity creation or before callingthe enable option
    bool immutable = false;
};