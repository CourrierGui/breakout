#pragma once

#include <pangolin/glfw-support.hpp>
#include <pangolin/game-object.hpp>

class BallObject : public GameObject {
  public:
    // ball state	
    float radius;
    bool  stuck;
    bool sticky, pass_through;

    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

    glm::vec2 move(float dt, unsigned int window_width);
    void      reset(glm::vec2 position, glm::vec2 velocity);
}; 
