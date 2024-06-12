cmake_minimum_required(VERSION 3.9.1)
project(GayTracer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PNG REQUIRED)

add_executable(gaytracer main.cpp)

target_include_directories(gaytracer PRIVATE ${PNG_INCLUDE_DIRS})
target_link_libraries(gaytracer PRIVATE ${PNG_LIBRARIES})

set_target_properties(gaytracer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    message("-- Configuring for debug...")
    target_compile_options(gaytracer PRIVATE -fsanitize=address -Wall -Wextra -Werror -g -O1)
    target_link_options(gaytracer PRIVATE -fsanitize=address)
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
    message("-- Configuring for debug...")
    target_compile_options(gaytracer PRIVATE -Wall -Wextra -O3 -DNDEBUG)
endif()


target_include_directories(gaytracer PRIVATE ${PROJECT_SOURCE_DIR})