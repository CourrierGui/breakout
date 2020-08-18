#include <breakout/ball-object.hpp>


BallObject::BallObject()
  : GameObject(), radius(12.5f), stuck(true),
  sticky(false), pass_through(false) { }

BallObject::BallObject(
  glm::vec2 pos, float radius,
  glm::vec2 velocity,
  pgl::Texture2D sprite)
  : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f),
               sprite, glm::vec3(1.0f), velocity),
  radius(radius),
  sticky(false),
  pass_through(false),
  stuck(true) { }

glm::vec2 BallObject::move(float dt, unsigned int window_width) {
  // if not stuck to player board
  if (!stuck) {
    // move the ball
    position += velocity * dt;
    // check if outside window bounds; if so, reverse velocity and restore at correct position
    if (position.x <= 0.0f) {
      velocity.x = -velocity.x;
      position.x = 0.0f;
    }
    else if (position.x + size.x >= window_width)
    {
      velocity.x = -velocity.x;
      position.x = window_width - size.x;
    }
    if (position.y <= 0.0f)
    {
      velocity.y = -velocity.y;
      position.y = 0.0f;
    }

  }
  return position;
}

// resets the ball to initial Stuck Position (if ball is outside window bounds)
void BallObject::reset(glm::vec2 position, glm::vec2 velocity) {
  this->position = position;
  this->velocity = velocity;
  stuck = true;
  sticky = false;
  pass_through = false;
}
