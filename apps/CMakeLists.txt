add_executable(breakout breakout.cpp)
target_include_directories(breakout PUBLIC ${CMAKE_SOURCE_DIR}/extern/pangolin/include)
target_include_directories(breakout PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(breakout PUBLIC pangolin game-utils)
