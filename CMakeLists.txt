cmake_minimum_required(VERSION 3.15)
project(ConferenceOrganizationTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "src/*.cpp"
        "include/*.cpp"
)

add_executable(ConferenceTool ${SOURCES})

target_include_directories(ConferenceTool PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/data_structures
)