cmake_minimum_required(VERSION 3.16)
project(DDS_STRESS_TEST)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)


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
########################
### PROJECT SETTINGS ###
########################

# Add JSON dependency
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/json)

########################
### SOURCE GATHERING ###
########################

# Common sources
file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/stress_test/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/stress_test/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/stress_test/*.cxx"
)

# FastDDS sources
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

########################
### UNIFIED LIBRARY  ###
########################

# Merge all sources conditionally
set(ALL_SOURCES ${COMMON_SOURCES})
if(USE_FAST_DDS)
    list(APPEND ALL_SOURCES ${FASTDDS_SOURCES})
elseif(USE_CYCLONE_DDS)
    list(APPEND ALL_SOURCES ${CYCLONEDDS_SOURCES})
endif()

# Create a single library
add_library(dds_impl ${ALL_SOURCES})

# Common include paths
target_include_directories(dds_impl PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/stress_test
    ${CMAKE_SOURCE_DIR}/src/stress_test/core
    ${CMAKE_SOURCE_DIR}/src/stress_test/core/patterns
)

# Link JSON
target_link_libraries(dds_impl PRIVATE nlohmann_json::nlohmann_json)

########################
### FAST DDS SETUP   ###
########################

if(USE_FAST_DDS)
    message(STATUS "Building with Fast DDS support")

    target_compile_definitions(dds_impl PRIVATE USE_FAST_DDS=1)

    target_include_directories(dds_impl PRIVATE
        ${CMAKE_SOURCE_DIR}/src/implementation/fast_dds
        ${CMAKE_SOURCE_DIR}/src/IDL
        ${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/include
    )

    link_directories(${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib)

    target_link_libraries(dds_impl PUBLIC
        fastdds
        fastcdr
        pthread
    )

    set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")
    set(CMAKE_BUILD_RPATH   "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")

########################
### CYCLONE DDS SETUP ###
########################

elseif(USE_CYCLONE_DDS)
    message(STATUS "Building with Cyclone DDS support")

    target_compile_definitions(dds_impl PRIVATE USE_CYCLONE_DDS=1)

    # Define prefix paths
    set(CMAKE_PREFIX_PATH
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install"
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib/cmake/CycloneDDS-CXX"
    )

    # Find CycloneDDS
    find_package(CycloneDDS REQUIRED)
    find_package(CycloneDDS-CXX REQUIRED)

    # Verify includes
    set(CYCLONEDDS_C_INCLUDE "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/include")
    if(EXISTS "${CYCLONEDDS_C_INCLUDE}/dds/dds.h")
        message(STATUS "CycloneDDS C headers found at: ${CYCLONEDDS_C_INCLUDE}")
    else()
        message(FATAL_ERROR "CycloneDDS C headers NOT found!")
    endif()

    # Verify library
    set(CYCLONEDDS_LIB "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/lib/libddsc.so.0.10.5")
    if(NOT EXISTS ${CYCLONEDDS_LIB})
        message(FATAL_ERROR "CycloneDDS library not found at ${CYCLONEDDS_LIB}")
    endif()
    message(STATUS "CycloneDDS library: ${CYCLONEDDS_LIB}")

    # Optional target check
    if(TARGET CycloneDDS-CXX::ddscxx)
        get_target_property(CXX_INCLUDES CycloneDDS-CXX::ddscxx INTERFACE_INCLUDE_DIRECTORIES)
        message(STATUS "CycloneDDS-CXX includes: ${CXX_INCLUDES}")
    endif()

    find_library(CYCLONEDDSIDL_LIB cycloneddsidl
        PATHS /workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/lib
        REQUIRED
    )

    target_include_directories(dds_impl PUBLIC
        ${CYCLONEDDS_C_INCLUDE}
        ${CXX_INCLUDES}
        ${CMAKE_SOURCE_DIR}/src/implementation/cyclone_dds
        ${CMAKE_SOURCE_DIR}/src/IDLs_cyclone
    )

    target_link_libraries(dds_impl PUBLIC
        ${CYCLONEDDS_LIB}
        CycloneDDS-CXX::ddscxx
        ${CYCLONEDDSIDL_LIB}
        pthread
    )

    set(CMAKE_INSTALL_RPATH "/workspaces/fastdds/lib/Cyclone-DDS/install/lib;/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib")
    set(CMAKE_BUILD_RPATH   "/workspaces/fastdds/lib/Cyclone-DDS/install/lib;/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib")
endif()

########################
### MAIN EXECUTABLE  ###
########################

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/stress_test/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/stress_test
    ${CMAKE_SOURCE_DIR}/src/stress_test/core
    ${CMAKE_SOURCE_DIR}/src/stress_test/core/patterns
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    dds_impl
    nlohmann_json::nlohmann_json
    pthread
)

########################
### RPATH SETTINGS   ###
########################

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
