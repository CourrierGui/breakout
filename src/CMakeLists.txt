add_library(game-utils STATIC breakout.cpp)
target_include_directories(game-utils PUBLIC ${CMAKE_SOURCE_DIR}/extern/pangolin/include)
target_include_directories(game-utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(game-utils pangolin)
