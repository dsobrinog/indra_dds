#include <dds_definitions.h>

// Abstract Layer
#include <IDomain.h>

// Concrete Layer
#include <Fast-DDS-API/FastDomain.h>

#include <memory>
#include <stdexcept>

class DDSFactory{
    public:
        
        static std::unique_ptr<IDomain> CreateDomain(DDSLibrary library, int domain_id)
        {
            switch (library) {
                case DDSLibrary::FAST_DDS:
                    return std::make_unique<indraDDS::FastDomain>(domain_id);
            
                default:
                    throw std::runtime_error("Unsupported DDS library");
            }
        }
};