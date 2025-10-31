cmake_minimum_required(VERSION 3.16)
project(DDS_STRESS_TEST)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)


# CMake Flags Options
option(USE_FAST_DDS "Use Fast DDS implementation" OFF)
option(USE_CYCLONE_DDS "Use Cyclone DDS implementation" OFF)
option(SIMPLE_CYCLONE_TEST "Make simple Cyclone DDS test" OFF)




# Validate that exactly one DDS implementation is selected
if(USE_FAST_DDS AND USE_CYCLONE_DDS)
    message(FATAL_ERROR "Cannot use both FastDDS and CycloneDDS. Please select only one.")
elseif(NOT USE_FAST_DDS AND NOT USE_CYCLONE_DDS)
    message(FATAL_ERROR "Please select a DDS implementation: USE_FAST_DDS or USE_CYCLONE_DDS")
endif()

###################
### SIMPLE TEST ###
###################
if(SIMPLE_CYCLONE_TEST AND USE_CYCLONE_DDS)
    message(STATUS "Configuring SIMPLE_CYCLONE_TEST executable")

    # Minimal test source
    set(SIMPLE_TEST_SRC "${CMAKE_SOURCE_DIR}/src/test.cpp")

     # Add custom prefix path for CycloneDDS
    set(CMAKE_PREFIX_PATH
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install"
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib/cmake/CycloneDDS-CXX"
    )

    # Find CycloneDDS packages automatically
    find_package(CycloneDDS REQUIRED)
    find_package(CycloneDDS-CXX REQUIRED)

    set(CYCLONEDDS_C_INCLUDE /workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/include)
    message(STATUS "CycloneDDS C headers found at: ${CYCLONEDDS_C_INCLUDE}")

    set(CYCLONEDDS_LIB /workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/lib/libddsc.so.0.10.5)
    message(STATUS "CycloneDDS library: ${CYCLONEDDS_LIB}")
    get_target_property(CXX_INCLUDES CycloneDDS-CXX::ddscxx INTERFACE_INCLUDE_DIRECTORIES)

    add_executable(simple_cyclone_test ${SIMPLE_TEST_SRC})

    # Include CycloneDDS headers
    target_include_directories(simple_cyclone_test PUBLIC
        ${CYCLONEDDS_C_INCLUDE}
        ${CXX_INCLUDES}
    )

    # Link with CycloneDDS libraries
    target_link_libraries(simple_cyclone_test PUBLIC
        ${CYCLONEDDS_LIB}
        CycloneDDS-CXX::ddscxx
    )

    target_compile_definitions(simple_cyclone_test PRIVATE USE_CYCLONE_DDS=1)
    return()
endif()

###################
### PROJECT     ###
###################

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
    # Add custom prefix path for CycloneDDS
    set(CMAKE_PREFIX_PATH
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install"
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib/cmake/CycloneDDS-CXX"
    )

    # Find CycloneDDS packages automatically
    find_package(CycloneDDS REQUIRED)
    find_package(CycloneDDS-CXX REQUIRED)

    set(CYCLONEDDS_C_INCLUDE /workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/include)
    if(EXISTS "${CYCLONEDDS_C_INCLUDE}/dds/dds.h")
        message(STATUS "CycloneDDS C headers found at: ${CYCLONEDDS_C_INCLUDE}")
    else()
        message(FATAL_ERROR "CycloneDDS C headers NOT found!")
    endif()

    set(CYCLONEDDS_LIB /workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/lib/libddsc.so.0.10.5)
    if(NOT EXISTS ${CYCLONEDDS_LIB})
        message(FATAL_ERROR "CycloneDDS library not found at ${CYCLONEDDS_LIB}")
    endif()
    message(STATUS "CycloneDDS library: ${CYCLONEDDS_LIB}")


    if(TARGET CycloneDDS-CXX::ddscxx)
        message(STATUS "CycloneDDS-CXX::ddscxx target found")
        get_target_property(CXX_INCLUDES CycloneDDS-CXX::ddscxx INTERFACE_INCLUDE_DIRECTORIES)
        message(STATUS "CycloneDDS-CXX includes: ${CXX_INCLUDES}")
    endif()

    target_link_libraries(common_dds PUBLIC
        ${CYCLONEDDS_LIB}
        CycloneDDS-CXX::ddscxx
    )

    target_include_directories(common_dds PUBLIC
        ${CYCLONEDDS_C_INCLUDE}
        ${CXX_INCLUDES}
        ${CMAKE_SOURCE_DIR}/src/implementation/cyclone_dds
        ${CMAKE_SOURCE_DIR}/src/IDLs_cyclone
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
    find_library(CYCLONEDDSIDL_LIB cycloneddsidl
        PATHS /workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/lib
        REQUIRED
    )
    target_link_libraries(cyclonedds_impl
        common_dds
        ${CYCLONEDDS_LIB}
        CycloneDDS-CXX::ddscxx
        ${CYCLONEDDSIDL_LIB} 
    )

    # Add compile definition
    target_compile_definitions(cyclonedds_impl PRIVATE USE_CYCLONE_DDS=1)
    
endif()

# Main executable only includes main.cpp
set(MAIN_SOURCES
    ${CMAKE_SOURCE_DIR}/src/stress_test/main.cpp
)

# message(STATUS "COMMON_SOURCES=${COMMON_SOURCES}")
# message(STATUS "FASTDDS_SOURCES=${FASTDDS_SOURCES}")
# message(STATUS "CYCLONEDDS_SOURCES=${CYCLONEDDS_SOURCES}")
# message(STATUS "MAIN_SOURCES=${MAIN_SOURCES}")

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

# Configure RPATH 
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(USE_FAST_DDS)
    set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")
    set(CMAKE_BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")
elseif(USE_CYCLONE_DDS)
    set(CMAKE_INSTALL_RPATH "/workspaces/fastdds/lib/Cyclone-DDS/install/lib;/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib")
    set(CMAKE_BUILD_RPATH "/workspaces/fastdds/lib/Cyclone-DDS/install/lib;/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib")
endif()