#pragma once


namespace indradds{

// Send a buffer to a topic channel with a QoS
class IDataWriter
{
public:
    // Constructor (topic, QoS)

    // write
    virtual bool write(const void* data) = 0;
    
    // Destructor
};

}