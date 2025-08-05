#pragma once

namespace indradds{

enum class DDSLibrary
{
    FAST_DDS,
    // CYCLONE_DDS,
    // RTI_DDS,
};

enum class DDSStatus {
    OK,
    TIMEOUT,
    ERROR,
    INVALID_ARGUMENT,
    NOT_INITIALIZED
};

enum class DDSResult{
    FAILURE = -1,
    SUCCESS = 0
};

}