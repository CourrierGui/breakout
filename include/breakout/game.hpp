#pragma once

#include <pangolin/resource-manager.hpp>
#include <pangolin/glfw-support.hpp>

#include <breakout/sprite-renderer.hpp>
#include <breakout/game-level.hpp>
#include <breakout/ball-object.hpp>
#include <breakout/particle-generator.hpp>
#include <breakout/post-processor.hpp>

#include <tuple>

enum GameState {
  GAME_ACTIVE,
  GAME_MENU,
  GAME_WIN
};

enum Direction {
  UP,
  RIGHT,
  DOWN,
  LEFT
};

using Collision =  std::tuple<bool, Direction, glm::vec2>;

bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction vector_direction(glm::vec2 target);

class Game {
  public:
    std::vector<GameLevel> m_levels;
    unsigned int m_level;
    GameState m_state;
    bool m_keys[1024];
    unsigned int width, height;

    Game(unsigned int width, unsigned int height);
    ~Game();

    void init();
    void update(float dt);
    void render();
    void process_collisions();
    void process_input(float dt);
    void reset_level();
    void reset_player();
};
