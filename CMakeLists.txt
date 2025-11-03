cmake_minimum_required(VERSION 3.16)
project(DDS_STRESS_TEST)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(USE_FAST_DDS "Use Fast DDS implementation" OFF)
option(USE_CYCLONE_DDS "Use Cyclone DDS implementation" OFF)
option(SIMPLE_CYCLONE_TEST "Make simple Cyclone DDS test" OFF)

# Validation
if(USE_FAST_DDS AND USE_CYCLONE_DDS)
    message(FATAL_ERROR "Cannot enable both FastDDS and CycloneDDS.")
elseif(NOT USE_FAST_DDS AND NOT USE_CYCLONE_DDS)
    message(FATAL_ERROR "You must enable one DDS implementation.")
endif()

###################
### SIMPLE TEST ###
###################
if(SIMPLE_CYCLONE_TEST AND USE_CYCLONE_DDS)
    message(STATUS "Configuring SIMPLE_CYCLONE_TEST executable")

    set(SIMPLE_TEST_SRC "${CMAKE_SOURCE_DIR}/src/test.cpp")

    set(CMAKE_PREFIX_PATH
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/lib/cmake/CycloneDDS"
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib/cmake/CycloneDDS-CXX"
    )

    find_package(CycloneDDS REQUIRED)
    find_package(CycloneDDS-CXX REQUIRED)

    add_executable(simple_cyclone_test ${SIMPLE_TEST_SRC})
    target_include_directories(simple_cyclone_test PUBLIC
        /workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/include
    )
    target_link_libraries(simple_cyclone_test PUBLIC
        CycloneDDS-CXX::ddscxx
        pthread
    )
    target_compile_definitions(simple_cyclone_test PRIVATE USE_CYCLONE_DDS=1)
    return()
endif()

########################
### PROJECT SETTINGS ###
########################
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/json)

# Common sources
file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/stress_test/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/stress_test/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/stress_test/*.cxx"
)

# Main executable sources
set(MAIN_SOURCE "${CMAKE_SOURCE_DIR}/src/stress_test/main.cpp")

################################
### FAST DDS IMPLEMENTATION  ###
################################
if(USE_FAST_DDS)
    message(STATUS "Building with Fast DDS")

    file(GLOB_RECURSE FASTDDS_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/implementation/fast_dds/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/IDL/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/IDL/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/IDLs/**/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/IDLs/**/*.hpp"
    )

    add_executable(${PROJECT_NAME}
        ${MAIN_SOURCE}
        ${COMMON_SOURCES}
        ${FASTDDS_SOURCES}
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/stress_test
        ${CMAKE_SOURCE_DIR}/src/stress_test/core
        ${CMAKE_SOURCE_DIR}/src/stress_test/core/patterns
        ${CMAKE_SOURCE_DIR}/src/implementation/fast_dds
        ${CMAKE_SOURCE_DIR}/src/IDL
        ${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/include
    )

    link_directories(${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        nlohmann_json::nlohmann_json
        fastdds
        fastcdr
        pthread
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_FAST_DDS=1)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib"
        INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib"
    )
endif()

##################################
### CYCLONE DDS IMPLEMENTATION ###
##################################
if(USE_CYCLONE_DDS)
    message(STATUS "Building with Cyclone DDS")

    file(GLOB_RECURSE CYCLONEDDS_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/implementation/cyclone_dds/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/IDLs_cyclone/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/IDLs_cyclone/*.hpp"
    )

    set(CMAKE_PREFIX_PATH
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/lib/cmake/CycloneDDS"
        "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib/cmake/CycloneDDS-CXX"
    )

    find_package(CycloneDDS REQUIRED)
    find_package(CycloneDDS-CXX REQUIRED)

    add_executable(${PROJECT_NAME}
        ${MAIN_SOURCE}
        ${COMMON_SOURCES}
        ${CYCLONEDDS_SOURCES}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/stress_test
        ${CMAKE_SOURCE_DIR}/src/stress_test/core
        ${CMAKE_SOURCE_DIR}/src/stress_test/core/patterns
        ${CMAKE_SOURCE_DIR}/src/implementation/cyclone_dds
        ${CMAKE_SOURCE_DIR}/src/IDLs_cyclone
        /workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/include
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        CycloneDDS::ddsc
        CycloneDDS-CXX::ddscxx
        pthread
    )

    target_compile_definitions(${PROJECT_NAME} PUBLIC USE_CYCLONE_DDS=1)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/lib;/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib"
        INSTALL_RPATH "/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-install/lib;/workspaces/fastdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib"
    )
endif()
