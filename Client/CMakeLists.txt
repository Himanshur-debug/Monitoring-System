cmake_minimum_required(VERSION 3.22)

project(Client VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
find_package(OpenSSL REQUIRED)

file(GLOB SOURCES src/* src/*/* src/*/*/*)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC 
        ${Boost_LIBRARIES} 
        ${OPENSSL_LIBRARIES}
        OpenSSL::SSL 
        OpenSSL::Crypto
    PRIVATE
)