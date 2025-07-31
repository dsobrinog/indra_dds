#pragma once


// Responsable for the reception and event consumer of new data in the topic channel
class IReader
{
public:
    // read
    virtual bool read(void* data_out) = 0;
    
    // Destructor
};