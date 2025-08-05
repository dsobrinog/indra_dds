cmake_minimum_required(VERSION 3.16)
project(Fast_DDS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE DDS_HELLOWORLD_SOURCES_CXX "src/example_fast_dds/*.cxx")

# set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install")

# Configure RPATH settings
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")
set(CMAKE_BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")

set(DDS_HELLOWORLD_SOURCES_CXX
    ${CMAKE_SOURCE_DIR}/src/example_fast_dds/HelloWorldPubSubTypes.cxx
    ${CMAKE_SOURCE_DIR}/src/example_fast_dds/HelloWorldTypeObjectSupport.cxx
)

#1 Definimos ejecutable
add_executable(${PROJECT_NAME} ${SOURCES})

# add_executable(${PROJECT_NAME}
#     src/example_fast_dds/HelloWorldPublisher.cpp
#     ${DDS_HELLOWORLD_SOURCES_CXX}
# )
# add_executable(${PROJECT_NAME}
#     src/example_fast_dds/HelloWorldSubscriber.cpp
#     ${DDS_HELLOWORLD_SOURCES_CXX}
# )

#2 Añadir includes desde la instalación local
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/include
    ${CMAKE_SOURCE_DIR}/src/indra_dds
    ${CMAKE_SOURCE_DIR}/src/indra_dds/Fast-DDS-API
    ${CMAKE_SOURCE_DIR}/src/indra_dds/Fast-DDS-API/DomainParticipant

)

#3 Link libraries
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib"
    INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib"
    SKIP_BUILD_RPATH OFF
)

target_link_libraries(${PROJECT_NAME}
    fastdds
    fastcdr
    pthread
)

