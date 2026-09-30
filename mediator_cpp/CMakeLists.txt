cmake_minimum_required(VERSION 3.16)
project(mediator_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(libssh2 CONFIG REQUIRED)

add_executable(mediator_cpp
    src/main.cpp
    src/connections/sshImpl.cpp
    # src/drivers/PayloadImpl.cpp
)

target_link_libraries(mediator_cpp
    PRIVATE
        libssh2::libssh2
)
