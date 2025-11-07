#pragma once
#ifdef USE_OPEN_DDS

#include <dds/DCPS/Definitions.h>
#include <string>

inline const char* DDSReturnCodeToString(DDS::ReturnCode_t code)
{
    switch (code)
    {
        case DDS::RETCODE_OK:                     return "RETCODE_OK";
        case DDS::RETCODE_ERROR:                  return "RETCODE_ERROR";
        case DDS::RETCODE_UNSUPPORTED:            return "RETCODE_UNSUPPORTED";
        case DDS::RETCODE_BAD_PARAMETER:          return "RETCODE_BAD_PARAMETER";
        case DDS::RETCODE_PRECONDITION_NOT_MET:   return "RETCODE_PRECONDITION_NOT_MET";
        case DDS::RETCODE_OUT_OF_RESOURCES:       return "RETCODE_OUT_OF_RESOURCES";
        case DDS::RETCODE_NOT_ENABLED:            return "RETCODE_NOT_ENABLED";
        case DDS::RETCODE_IMMUTABLE_POLICY:       return "RETCODE_IMMUTABLE_POLICY";
        case DDS::RETCODE_INCONSISTENT_POLICY:    return "RETCODE_INCONSISTENT_POLICY";
        case DDS::RETCODE_ALREADY_DELETED:        return "RETCODE_ALREADY_DELETED";
        case DDS::RETCODE_TIMEOUT:                return "RETCODE_TIMEOUT";
        case DDS::RETCODE_NO_DATA:                return "RETCODE_NO_DATA";
        case DDS::RETCODE_ILLEGAL_OPERATION:      return "RETCODE_ILLEGAL_OPERATION";
        default:                                  return "UNKNOWN_RETCODE";
    }
}

#endif