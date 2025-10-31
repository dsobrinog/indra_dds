#include <dds/dds.hpp>
#include <iostream>

int main() {
    try {
        dds::domain::DomainParticipant participant(0);
        std::cout << "CycloneDDS participant created successfully!" << std::endl;
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "CycloneDDS test failed: " << e.what() << std::endl;
        return 1;
    }
}
