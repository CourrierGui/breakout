set(
  breakout_src
  game.cpp
  sprite-renderer.cpp
  game-object.cpp
  game-level.cpp
  ball-object.cpp
  particle-generator.cpp
  post-processor.cpp
  text-renderer.cpp
)

add_library(irrKlanglib SHARED IMPORTED)
set_target_properties(irrKlanglib PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/extern/irrKlang/irrKlang-64bit-1.6.0/bin/linux-gcc-64/libIrrKlang.so"
)

add_library(freetype SHARED IMPORTED)
set_target_properties(freetype PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/extern/freetype2/objs/.libs/libfreetype.so"
)

add_library(pnglib SHARED IMPORTED)
set_target_properties(pnglib PROPERTIES
  IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/libpng.so")

add_library(game-utils STATIC ${breakout_src})
target_include_directories(game-utils PUBLIC ${CMAKE_SOURCE_DIR}/extern/pangolin/include)
target_include_directories(game-utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(game-utils
  PUBLIC ${CMAKE_SOURCE_DIR}/extern/freetype2/include)
target_include_directories(game-utils
  PUBLIC ${CMAKE_SOURCE_DIR}/extern/irrKlang/irrKlang-64bit-1.6.0/include)
target_link_libraries(game-utils pnglib png freetype pangolin glad irrKlanglib)
