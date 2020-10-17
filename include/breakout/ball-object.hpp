#pragma once

#include <pangolin/glfw-support.hpp>
#include <pangolin/game-object.hpp>
#include <pgl-math/vector.hpp>

class BallObject : public pgl::GameObject {
  public:
    // ball state	
    float radius;
    bool  stuck;
    bool sticky, pass_through;

    BallObject();
    BallObject(
      pgl::float2 pos, float radius,
      pgl::float2 velocity,
      pgl::Texture2D sprite
    );

    auto move(float dt, unsigned int window_width) -> pgl::float2;
    void reset(pgl::float2 position, pgl::float2 velocity);
}; 
