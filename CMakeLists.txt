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
option(USE_OPEN_DDS "Use OpenDDS implementation" OFF)
option(USE_RTI_DDS "Use RtiDDS implementation" OFF)

# Create a list of enabled DDS implementations
set(ENABLED_DDS "")
if(USE_FAST_DDS)
    list(APPEND ENABLED_DDS "FAST")
endif()
if(USE_CYCLONE_DDS)
    list(APPEND ENABLED_DDS "CYCLONE")
endif()
if(USE_OPEN_DDS)
    list(APPEND ENABLED_DDS "OPEN")
endif()
if(USE_RTI_DDS)
    list(APPEND ENABLED_DDS "RTI")
endif()

# Count enabled DDS implementations
list(LENGTH ENABLED_DDS DDS_COUNT)

if(DDS_COUNT GREATER 1)
    message(FATAL_ERROR "Only one DDS implementation can be enabled at a time: ${ENABLED_DDS}")
elseif(DDS_COUNT EQUAL 0)
    message(FATAL_ERROR "You must enable exactly one DDS implementation.")
else()
    message(STATUS "Using DDS implementation: ${ENABLED_DDS}")
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
        "${CMAKE_SOURCE_DIR}/src/IDLs/generated/**/*.cxx"
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
        ${CMAKE_SOURCE_DIR}/lib/Fast-DDS/fastdds-install/include
    )

    # set(fastdds_DIR "/workspaces/testdds/lib/Fast-DDS/fastdds-install/share/fastdds/cmake")
    # set(fastcdr_DIR "/workspaces/testdds/lib/Fast-DDS/fastdds-install/lib/cmake/fastcdr")
    # find_package(fastdds REQUIRED)
    # find_package(fastcdr REQUIRED)

    link_directories(${CMAKE_SOURCE_DIR}/lib/Fast-DDS/fastdds-install/lib)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        nlohmann_json::nlohmann_json
        /workspaces/testdds/lib/Fast-DDS/fastdds-install/lib/libfastdds.so
        /workspaces/testdds/lib/Fast-DDS/fastdds-install/lib/libfastcdr.so
        pthread
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_FAST_DDS=1)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/fastdds-install/lib"
        INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/fastdds-install/lib"
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
        "/workspaces/testdds/lib/Cyclone-DDS/cyclonedds-install/lib/cmake/CycloneDDS"
        "/workspaces/testdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib/cmake/CycloneDDS-CXX"
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
        /workspaces/testdds/lib/Cyclone-DDS/cyclonedds-install/include
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        CycloneDDS::ddsc
        CycloneDDS-CXX::ddscxx
        pthread
    )

    target_compile_definitions(${PROJECT_NAME} PUBLIC USE_CYCLONE_DDS=1)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "/workspaces/testdds/lib/Cyclone-DDS/cyclonedds-install/lib;/workspaces/testdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib"
        INSTALL_RPATH "/workspaces/testdds/lib/Cyclone-DDS/cyclonedds-install/lib;/workspaces/testdds/lib/Cyclone-DDS/cyclonedds-cxx-install/lib"
    )
endif()


##################################
### OPEN DDS IMPLEMENTATION ######
##################################
option(USE_OPEN_DDS "Use OpenDDS implementation" OFF)
if(USE_OPEN_DDS)
    message(STATUS "Building with OpenDDS")

    set(OPENDDS_ROOT /workspaces/testdds/lib/OpenDDS/OpenDDS-install)
    set(OPENDDS_INCLUDE ${OPENDDS_ROOT}/include)
    set(OPENDDS_SHARE ${OPENDDS_ROOT}/share/dds)
    set(OPENDDS_LIB ${OPENDDS_ROOT}/lib)

    # Paths for IDL input and generated output
    set(GENERATED_DIR ${CMAKE_SOURCE_DIR}/src/IDLs_open/generated)
    set(IDL_FILES
        ${CMAKE_SOURCE_DIR}/src/IDLs_open/test_control.idl
        ${CMAKE_SOURCE_DIR}/src/IDLs_open/inse.idl
    )

    ###############################################
    ### PHASE 1: GENERATE DDS CODE FROM IDL FILES #
    ###############################################
    add_custom_target(generate_dds_code ALL COMMENT "Generating OpenDDS IDL bindings")

    foreach(idl ${IDL_FILES})
        get_filename_component(base ${idl} NAME_WE)
        set(gen_dir ${GENERATED_DIR}/${base})
        file(MAKE_DIRECTORY ${gen_dir})

        add_custom_command(
            TARGET generate_dds_code
            COMMAND ${OPENDDS_ROOT}/bin/opendds_idl -Lc++11 -o ${gen_dir} ${idl}
            COMMAND ${OPENDDS_ROOT}/bin/tao_idl
                --idl-version 4
                -I${OPENDDS_SHARE}
                -I${OPENDDS_SHARE}/DCPS
                -I${OPENDDS_SHARE}/DCPS/RTPS
                -I${CMAKE_SOURCE_DIR}/src/IDLs_open
                -o ${gen_dir}
                ${gen_dir}/${base}TypeSupport.idl
            COMMENT "Generating DDS bindings for ${base}.idl"
        )

        list(APPEND GENERATED_SOURCES
            ${gen_dir}/${base}TypeSupportImpl.cpp
            ${gen_dir}/${base}TypeSupportImpl.h
        )
    endforeach()

    ###############################################
    ### PHASE 2: COMPILE YOUR MAIN EXECUTABLE #####
    ###############################################

    # Sources: generated + implementation
    file(GLOB_RECURSE OPENDDS_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/src/implementation/open_dds/*.cpp
        ${CMAKE_SOURCE_DIR}/src/implementation/open_dds/*.hpp
        ${GENERATED_SOURCES}
    )

    add_executable(${PROJECT_NAME}
        ${MAIN_SOURCE}
        ${COMMON_SOURCES}
        ${OPENDDS_SOURCES}
    )

    # Make sure the executable waits for IDL generation
    add_dependencies(${PROJECT_NAME} generate_dds_code)

    # Include directories
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/stress_test
        ${CMAKE_SOURCE_DIR}/src/stress_test/core
        ${CMAKE_SOURCE_DIR}/src/stress_test/core/patterns
        ${CMAKE_SOURCE_DIR}/src/implementation/open_dds
        ${GENERATED_DIR}/test_control
        ${GENERATED_DIR}/inse
        ${OPENDDS_INCLUDE}
    )

    # Link libraries
    target_link_directories(${PROJECT_NAME} PUBLIC ${OPENDDS_LIB})

    target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        OpenDDS_Dcps
        ACE
        TAO
        TAO_Valuetype
        TAO_AnyTypeCode
        pthread
    )

    # Preprocessor definitions
    target_compile_definitions(${PROJECT_NAME} PUBLIC USE_OPEN_DDS=1)

    # Runtime library paths
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "${OPENDDS_LIB}"
        INSTALL_RPATH "${OPENDDS_LIB}"
    )
endif()

