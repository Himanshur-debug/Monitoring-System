cmake_minimum_required(VERSION 3.22)

project(Server VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC 
    PRIVATE
)
