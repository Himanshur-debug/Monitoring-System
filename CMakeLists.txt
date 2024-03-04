cmake_minimum_required(VERSION 3.22)

project(Client VERSION 0.0.1 LANGUAGES CXX)

find_package(Boost 1.84.0 REQUIRED)

add_subdirectory(Server)
add_subdirectory(Client)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC 
        ${Boost_LIBRARIES}
)

# https://linux.how2shout.com/how-to-install-boost-c-on-ubuntu-20-04-or-22-04/
