#pragma once

#include <string>

// Defines channels where pub/sub make matching. Allows uniformity of data type between the domain.
// Change propagation:
// When an IWriter publish data to , a change is applied in RTPSWrtier's history cache. Then is sent to all IReaders, that updates their history cache with the new change.
class ITopic
{
    public:
        ITopic(const std::string& _topic_name, const std::string& _type_name)
        {
            topic_name = _topic_name;
            type_name = _type_name;
        }

        virtual ~ITopic() = default;
        
        virtual std::string GetName() const = 0;
        virtual std::string GetTypeName() const = 0;

        // Configuration QoS

        // ITypeSupport std::shared_ptr<ITypeSupport>

    protected: 
        std::string topic_name;
        std::string type_name;
};