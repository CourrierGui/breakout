cmake_minimum_required(VERSION 3.1)

if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(beat-the-boredom VERSION 1.0
  DESCRIPTION "javidx9 game jam."
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set (CMAKE_CXX_STANDARD 17)

add_subdirectory(extern/pangolin)
add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(docs)

# option(BUILD_TESTING "Build the tests" ON)
# if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
#   enable_testing()
#   include(GoogleTest)
#   add_subdirectory(tests)
# endif()
