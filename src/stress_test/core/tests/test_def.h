#pragma once
#include <chrono>

enum class TestResult
{
    INVALID,
    VALID
};

using namespace eprosima::fastdds::dds;

const std::string print_return_code(ReturnCode_t code) 
{
    switch (code) {
        case RETCODE_OK:
            return "RETCODE_OK";
        case RETCODE_ERROR:
            return "RETCODE_ERROR";
        case RETCODE_UNSUPPORTED:
            return "RETCODE_UNSUPPORTED";
        case RETCODE_BAD_PARAMETER:
                "RETCODE_BAD_PARAMETER";
        case RETCODE_PRECONDITION_NOT_MET:
            return "RETCODE_PRECONDITION_NOT_MET";
        case RETCODE_OUT_OF_RESOURCES:
            return "RETCODE_OUT_OF_RESOURCES";
        case RETCODE_NOT_ENABLED:
            return "RETCODE_NOT_ENABLED";
        case RETCODE_IMMUTABLE_POLICY:
            return "RETCODE_IMMUTABLE_POLICY";
        case RETCODE_INCONSISTENT_POLICY:
            return "RETCODE_INCONSISTENT_POLICY";
        case RETCODE_ALREADY_DELETED:
            return "RETCODE_ALREADY_DELETED";
        case RETCODE_TIMEOUT:
            return "RETCODE_TIMEOUT";
        case RETCODE_NO_DATA:
            return "RETCODE_NO_DATA";
        case RETCODE_ILLEGAL_OPERATION:
            return "RETCODE_ILLEGAL_OPERATION";
        default:
            return "Unknown ReturnCode: ";
    }
}

