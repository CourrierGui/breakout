set(
  breakout_src
  game.cpp
  sprite-renderer.cpp
  game-object.cpp
  game-level.cpp
  ball-object.cpp
  particle-generator.cpp
  post-processor.cpp
)

add_library(game-utils STATIC ${breakout_src})
target_include_directories(game-utils PUBLIC ${CMAKE_SOURCE_DIR}/extern/pangolin/include)
target_include_directories(game-utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(game-utils pangolin glad)
