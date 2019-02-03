project (Balancer)
cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Use default cmake build type: Debug")
    set(CMAKE_BUILD_TYPE Debug)
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -std=c++11 -pthread")

set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_PATH})

include_directories(
    ./../
)

link_directories(
    ./../proto/lib/
    ./../logger/lib/
)

file(GLOB SRC_LIST
    ./src/*.cpp
    ./src/tcp-server/*.h
    ./src/tcp-server/*.cpp
    ./src/tcp-session/*.h
    ./src/logger/*.h
    ./src/logger/*.cpp
)

set(MODULE_NAME ${PROJECT_NAME})

add_executable(${MODULE_NAME} ${SRC_LIST})

target_link_libraries(
    ${PROJECT_NAME}
    event
    log4cplus
    Proto
    Logger
)
