#pragma once

#include <string>


namespace indradds{

// Responsable for creating and configuring IReaders.
class ISubscriber
{
public:
    virtual ~ISubscriber() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void update() = 0;

    virtual const std::string& get_topic_name() const = 0;

protected:

    // Lista de IReaders

};

}