#include <breakout/ball-object.hpp>


BallObject::BallObject() 
  : GameObject(), m_radius(12.5f), m_stuck(true),
  m_sticky(false), m_pass_through(false) { }

BallObject::BallObject(
  glm::vec2 pos, float radius,
  glm::vec2 velocity, Texture2D sprite)
  : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f),
               sprite, glm::vec3(1.0f), velocity),
  m_radius(radius),
  m_sticky(false),
  m_pass_through(false),
  m_stuck(true) { }

glm::vec2 BallObject::move(float dt, unsigned int window_width) {
  // if not stuck to player board
  if (!m_stuck) { 
    // move the ball
    m_position += m_velocity * dt;
    // check if outside window bounds; if so, reverse velocity and restore at correct position
    if (m_position.x <= 0.0f) {
      m_velocity.x = -m_velocity.x;
      m_position.x = 0.0f;
    }
    else if (m_position.x + m_size.x >= window_width)
    {
      m_velocity.x = -m_velocity.x;
      m_position.x = window_width - m_size.x;
    }
    if (m_position.y <= 0.0f)
    {
      m_velocity.y = -m_velocity.y;
      m_position.y = 0.0f;
    }

  }
  return m_position;
} 

// resets the ball to initial Stuck Position (if ball is outside window bounds)
void BallObject::reset(glm::vec2 position, glm::vec2 velocity) {
  m_position = position;
  m_velocity = velocity;
  m_stuck = true;
  m_sticky = false;
  m_pass_through = false;
}
