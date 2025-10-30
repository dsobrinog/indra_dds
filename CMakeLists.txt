cmake_minimum_required(VERSION 3.16)
project(DDS_STRESS_TEST)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# CMake Flags Options
option(USE_FAST_DDS "Use Fast DDS implementation" OFF)
option(USE_CYCLONE_DDS "Use Cyclone DDS implementation" OFF)

# Validate that exactly one DDS implementation is selected
if(USE_FAST_DDS AND USE_CYCLONE_DDS)
    message(FATAL_ERROR "Cannot use both FastDDS and CycloneDDS. Please select only one.")
elseif(NOT USE_FAST_DDS AND NOT USE_CYCLONE_DDS)
    message(FATAL_ERROR "Please select a DDS implementation: USE_FAST_DDS or USE_CYCLONE_DDS")
endif()



# Common
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/json)


#Common sources
file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/stress_test/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/stress_test/*.cxx"
    "${CMAKE_SOURCE_DIR}/src/stress_test/*.cpp"
)
#FastDDS sources
file(GLOB_RECURSE FASTDDS_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/implementation/fast_dds/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/IDL/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/IDL/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/IDL/*.cxx"
    "${CMAKE_SOURCE_DIR}/src/IDLs/**/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/IDLs/**/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/IDLs/**/*.cxx"
)
# CycloneDDS sources
file(GLOB_RECURSE CYCLONEDDS_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/implementation/cyclone_dds/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/IDLs_cyclone/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/IDLs_cyclone/*.hpp"
)

#Create common Library
add_library(common_dds ${COMMON_SOURCES})
target_include_directories(common_dds PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/stress_test
    ${CMAKE_SOURCE_DIR}/src/stress_test/core
    ${CMAKE_SOURCE_DIR}/src/stress_test/core/patterns
)

if(USE_FAST_DDS)
    target_compile_definitions(common_dds PRIVATE USE_FAST_DDS=1)
    target_include_directories(common_dds PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/include
    )
elseif(USE_CYCLONE_DDS)
    target_compile_definitions(common_dds PRIVATE USE_CYCLONE_DDS=1)
    target_include_directories(common_dds PRIVATE
            ${CMAKE_SOURCE_DIR}/src/implementation/cyclone_dds
            /opt/cyclonedds/include
            /opt/cyclonedds/include/ddscxx
        )
endif()
# Create DDS-specific libraries

#FAST DDS
if(USE_FAST_DDS)
    add_library(fastdds_impl ${FASTDDS_SOURCES})
    target_include_directories(fastdds_impl PRIVATE
        ${CMAKE_SOURCE_DIR}/src/implementation/fast_dds
        ${CMAKE_SOURCE_DIR}/src/IDL
        ${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/include
    )
    # Add library search path
    link_directories(${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib)

    # Link with Fast DDS libs
    target_link_libraries(fastdds_impl common_dds fastdds fastcdr)
    
    # Add compile definition
    target_compile_definitions(fastdds_impl PRIVATE USE_FAST_DDS=1)

#CYCLONE DDS
elseif(USE_CYCLONE_DDS)
    add_library(cyclonedds_impl ${CYCLONEDDS_SOURCES})
    target_include_directories(cyclonedds_impl PRIVATE
        ${CMAKE_SOURCE_DIR}/src/implementation/cyclone_dds
        ${CMAKE_SOURCE_DIR}/src/IDLs_cyclone
        /opt/cyclonedds/include
        /opt/cyclonedds/include/ddscxx
    )
    
    find_library(DDSC_LIB ddsc PATHS /opt/cyclonedds/lib REQUIRED)
    find_library(CYCLONEDDSIDL_LIB cycloneddsidl PATHS /opt/cyclonedds/lib REQUIRED)
    find_library(DDSCXX_LIB ddscxx PATHS /opt/cyclonedds-cxx/lib REQUIRED)
    
    target_link_libraries(cyclonedds_impl common_dds 
        ${DDSC_LIB} ${CYCLONEDDSIDL_LIB} ${DDSCXX_LIB} )
    
    # Add compile definition
    target_compile_definitions(cyclonedds_impl PRIVATE USE_CYCLONE_DDS=1)
    
endif()

# Main executable only includes main.cpp
set(MAIN_SOURCES
    ${CMAKE_SOURCE_DIR}/src/stress_test/main.cpp
)

message(STATUS "COMMON_SOURCES=${COMMON_SOURCES}")
message(STATUS "FASTDDS_SOURCES=${FASTDDS_SOURCES}")
message(STATUS "MAIN_SOURCES=${MAIN_SOURCES}")

add_executable(${PROJECT_NAME} ${MAIN_SOURCES})

# Common include directories for main executable
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/stress_test
    ${CMAKE_SOURCE_DIR}/src/stress_test/core
    ${CMAKE_SOURCE_DIR}/src/stress_test/core/patterns
)

# Link based on selected DDS implementation
if(USE_FAST_DDS)
    target_link_libraries(${PROJECT_NAME} common_dds fastdds_impl)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_FAST_DDS=1)
elseif(USE_CYCLONE_DDS)
    target_link_libraries(${PROJECT_NAME} common_dds cyclonedds_impl)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_CYCLONE_DDS=1)
endif()

target_link_libraries(common_dds PRIVATE nlohmann_json::nlohmann_json)

# Common dependencies
target_link_libraries(${PROJECT_NAME}
    pthread
    nlohmann_json::nlohmann_json
)

# Configure RPATH if needed
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(USE_FAST_DDS)
    set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")
    set(CMAKE_BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")
elseif(USE_CYCLONE_DDS)
    set(CMAKE_INSTALL_RPATH "/opt/cyclonedds/install/lib;/opt/cyclonedds/install/cxx/lib")
    set(CMAKE_BUILD_RPATH "/opt/cyclonedds/install/lib;/opt/cyclonedds/install/cxx/lib")
endif()