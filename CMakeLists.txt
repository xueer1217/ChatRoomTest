cmake_minimum_required(VERSION 3.15)
project(ChatRoomTest)

set(CMAKE_CXX_STANDARD 11)

message( project source dir ${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME}
        src/server.cpp
        src/client.cpp
        src/main.cpp
        src/socketUtil.cpp )


target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        include
)