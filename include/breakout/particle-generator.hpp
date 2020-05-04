/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#pragma once

#include <iostream>
#include <pangolin/debug.hpp>

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <pangolin/shader.hpp>
#include <pangolin/texture.hpp>
#include <breakout/game-object.hpp>


// Represents a single particle and its state
struct Particle {
  glm::vec2 position, velocity;
  glm::vec4 color;
  float     life;

  Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator {
  public:
    // constructor
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    // update all particles
    void update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    // render all particles
    void draw();
  private:
    // state
    std::vector<Particle> m_particles;
    unsigned int m_amount;
    // render state
    Shader m_shader;
    Texture2D m_texture;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};
