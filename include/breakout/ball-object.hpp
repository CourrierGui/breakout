#pragma once

#include <pangolin/glfw-support.hpp>
#include <breakout/game-object.hpp>

class BallObject : public GameObject {
  public:
    // ball state	
    float m_radius;
    bool  m_stuck;

    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

    glm::vec2 move(float dt, unsigned int window_width);
    void      reset(glm::vec2 position, glm::vec2 velocity);
}; 
