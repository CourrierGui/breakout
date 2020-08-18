#pragma once

#include <pangolin/game-object.hpp>
#include <pangolin/resource-manager.hpp>

#include <vector>
#include <fstream>
#include <sstream>

class GameLevel {
  public:
    // level state
    std::vector<pgl::GameObject> bricks;
    // constructor
    GameLevel() { }
    // loads level from file
    void load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
    // render level
    void draw(pgl::render2D::SpriteRenderer &renderer);
    // check if the level is completed (all non-solid tiles are destroyed)
    bool isCompleted();

  private:
    // initialize level from tile data
    void init(
			std::vector<std::vector<unsigned int>>& tile_data,
			unsigned int level_width, unsigned int level_height
		);
};
