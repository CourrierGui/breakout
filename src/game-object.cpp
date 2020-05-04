/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#include <breakout/game-object.hpp>

GameObject::GameObject() 
  : m_position(0.0f, 0.0f), m_size(1.0f, 1.0f),
  m_velocity(0.0f), m_color(1.0f), m_rotation(0.0f),
  m_sprite(), m_isSolid(false), m_destroyed(false)
{

}

GameObject::GameObject(
  glm::vec2 pos, glm::vec2 size,
  Texture2D sprite, glm::vec3 color,
  glm::vec2 velocity) 
  : m_position(pos), m_size(size),
  m_velocity(velocity), m_color(color),
  m_rotation(0.0f), m_sprite(sprite),
  m_isSolid(false), m_destroyed(false) { }

void GameObject::draw(SpriteRenderer &renderer) {
  renderer.draw(m_sprite, m_position, m_size, m_rotation, m_color);
}


