cmake_minimum_required(VERSION 3.16)
project(Fast_DDS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/*.cxx"
    )

# set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install")

# Configure RPATH settings
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")
set(CMAKE_BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/lib")



#1 Definimos ejecutable
add_executable(${PROJECT_NAME} ${SOURCES})

#2 Añadir includes desde la instalación local
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/src/stress_test/core
    ${CMAKE_SOURCE_DIR}/src/stress_test/core/patterns
    ${CMAKE_SOURCE_DIR}/src/IDLs/generated
    ${CMAKE_SOURCE_DIR}/src/indra_dds
    ${CMAKE_SOURCE_DIR}/lib/Fast-DDS/install/include
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

