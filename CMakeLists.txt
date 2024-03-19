cmake_minimum_required(VERSION 3.22)

project(Client VERSION 0.0.1 LANGUAGES CXX)

find_package(Boost 1.80.0 REQUIRED)
find_package(OpenSSL REQUIRED)

set(CERTIFICATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Certificates)
add_definitions(-DserverCrt="${CERTIFICATE_DIR}/server.crt")
add_definitions(-DserverKey="${CERTIFICATE_DIR}/server.key")

add_subdirectory(Server)
add_subdirectory(Client)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC 
        ${Boost_LIBRARIES}
        OpenSSL::SSL 
        OpenSSL::Crypto
)

# https://linux.how2shout.com/how-to-install-boost-c-on-ubuntu-20-04-or-22-04/

# sudo apt update
# sudo apt install nlohmann-json3-dev
# sudo apt-get install cmake

