cmake_minimum_required(VERSION 3.15)

project(ConferenceOrganizationTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")


include_directories(include)


file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(ConferenceTool ${SOURCES})