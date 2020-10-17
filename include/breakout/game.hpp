#pragma once

#include <pangolin/resource-manager.hpp>
#include <pangolin/glfw-support.hpp>
#include <pangolin/sprite-renderer.hpp>
#include <pangolin/particle-generator.hpp>
#include <pangolin/text-renderer.hpp>
#include <pangolin/game-object.hpp>

#include <pgl-math/vector.hpp>
#include <pgl-math/matrix.hpp>
#include <pgl-math/algorithms.hpp>

#include <breakout/game-level.hpp>
#include <breakout/ball-object.hpp>
#include <breakout/post-processor.hpp>
#include <breakout/power-up.hpp>

#include <irrKlang.h>
#include <algorithm>
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

const unsigned int BAD_RATE = 15;
const unsigned int GOOD_RATE = 30;

using Collision = std::tuple<bool, Direction, pgl::float2>;

bool CheckCollision(pgl::GameObject& one, pgl::GameObject& two);
auto CheckCollision(BallObject& one, pgl::GameObject& two) -> Collision;
auto vector_direction(pgl::float2 target) -> Direction;
bool should_spawn(unsigned int chance);
void ActivatePowerUp(PowerUp& powerUp);
bool isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

class Game {
  public:
    std::vector<PowerUp>   power_ups;
    std::vector<GameLevel> levels;
    unsigned int level;
    GameState state;
    bool keys[1024];
    bool key_processed[1024];
    unsigned int width, height;
    unsigned int lives;

    Game(unsigned int width, unsigned int height);
    ~Game();

    void init();
    void update(float dt);
    void render();
    void process_collisions();
    void process_input(float dt);
    void reset_level();
    void reset_player();
    void spawn_power_ups(pgl::GameObject& block);
    void update_power_ups(float dt);
};
