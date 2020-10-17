cmake_minimum_required(VERSION 3.1)

if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(beat-the-boredom
	VERSION 1.0
  DESCRIPTION "Breakout game from learnopengl.com"
  LANGUAGES CXX
)

# set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(pangolin REQUIRED)
find_package(OpenAL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(target-flags INTERFACE)
target_compile_options(target-flags
	INTERFACE
	-Wall -Wextra -Wpedantic -Weffc++
	)
target_compile_features(target-flags INTERFACE cxx_std_20)

add_library(irrKlanglib SHARED IMPORTED)
set_target_properties(irrKlanglib PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/extern/irrKlang/irrKlang-64bit-1.6.0/bin/linux-gcc-64/libIrrKlang.so"
)

add_library(game-utils STATIC
  src/game.cpp
  src/game-level.cpp
  src/ball-object.cpp
  src/post-processor.cpp
)
target_include_directories(game-utils
  PUBLIC
    include
    extern/irrKlang/irrKlang-64bit-1.6.0/include
)
target_link_libraries(game-utils
	PUBLIC
		pangolin::pangolin irrKlanglib target-flags
		pangolin::glad pangolin::pgl-math
)

add_executable(breakout apps/breakout.cpp)
target_include_directories(breakout PUBLIC include)
target_link_libraries(breakout PUBLIC game-utils glfw)

# add_subdirectory(docs)

# option(BUILD_TESTING "Build the tests" ON)
# if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
#   enable_testing()
#   include(GoogleTest)
#   add_subdirectory(tests)
# endif()
