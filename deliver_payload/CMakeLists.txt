cmake_minimum_required(VERSION 3.15)

project(deliver_payload)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Strict warning handling
add_definitions()

find_package(MAVSDK REQUIRED)

# App's executable and source files
add_executable(deliver_payload
  deliver_payload.cpp
)

# App's executable and linked libraries
target_link_libraries(deliver_payload
    MAVSDK::mavsdk
)

