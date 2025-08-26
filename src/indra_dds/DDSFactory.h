#include <dds_definitions.h>

// Abstract Layer
#include <Core/IDomain.h>

// Concrete Layer
#include <FastDDS/FastDomain.h>

#include <memory>
#include <stdexcept>

namespace indradds{

class DDSFactory{
    public:
        
        static std::unique_ptr<IDomain> CreateDomain(DDSLibrary library, int domain_id)
        {
            switch (library) {
                case DDSLibrary::FAST_DDS:
                    return std::make_unique<indradds::FastDomain>(domain_id);
            
                default:
                    throw std::runtime_error("Unsupported DDS library");
            }
        }
};
}