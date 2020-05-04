#pragma once

#include <pangolin/resource-manager.hpp>
#include <pangolin/glfw-support.hpp>

#include <breakout/sprite-renderer.hpp>
#include <breakout/game-level.hpp>
#include <breakout/ball-object.hpp>
#include <breakout/particle-generator.hpp>
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

using Collision = std::tuple<bool, Direction, glm::vec2>;

bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction vector_direction(glm::vec2 target);
bool should_spawn(unsigned int chance);
void ActivatePowerUp(PowerUp& powerUp);
bool isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

class Game {
  public:
    std::vector<PowerUp>   m_power_ups;
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
    void spawn_power_ups(GameObject& block);
    void update_power_ups(float dt);
};
