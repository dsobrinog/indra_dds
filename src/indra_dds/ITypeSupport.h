#pragma once

#include <string>
#include <vector>
#include <cstdint>

class ITypeSupport {
public:
    virtual std::string GetTypeName() const = 0;
    virtual bool Serialize(void* data, std::vector<uint8_t>& buffer) = 0;
    virtual bool Deserialize(const std::vector<uint8_t>& buffer, void* data_out) = 0;
};