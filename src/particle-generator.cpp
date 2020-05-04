/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#include <breakout/particle-generator.hpp>

ParticleGenerator::ParticleGenerator(
  Shader shader,
  Texture2D texture,
  unsigned int amount)
  : m_shader(shader),
  m_texture(texture),
  m_amount(amount)
{
  init();
}

void ParticleGenerator::update(
  float dt, GameObject &object,
  unsigned int newParticles, glm::vec2 offset)
{
  // add new particles 
  for (unsigned int i = 0; i < newParticles; ++i) {
    int unusedParticle = firstUnusedParticle();
    respawnParticle(m_particles[unusedParticle], object, offset);
  }
  // update all particles
  for (unsigned int i = 0; i < m_amount; ++i) {
    Particle& p = m_particles[i];
    p.life -= dt; // reduce life
    if (p.life > 0.0f) { // particle is alive, thus update
      p.position -= p.velocity * dt; 
      p.color.a -= dt * 2.5f;
    }
  }
}

// render all particles
void ParticleGenerator::draw() {
  // use additive blending to give it a 'glow' effect
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  m_shader.use();
  for (Particle particle : m_particles) {
    if (particle.life > 0.0f) {
      m_shader.setVector2f("offset", particle.position);
      m_shader.setVector4f("color", particle.color);
      m_texture.bind();
      glBindVertexArray(this->VAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
    }
  }
  // don't forget to reset to default blending mode
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init() {
  // set up mesh and attribute properties
  unsigned int VBO;
  float particle_quad[] = {
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
  }; 
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(this->VAO);
  // fill mesh buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
  // set mesh attributes
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindVertexArray(0);

  // create this->amount default particle instances
  for (unsigned int i = 0; i < m_amount; ++i)
    m_particles.push_back(Particle());
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle() {
  // first search from last used particle, this will usually return almost instantly
  for (unsigned int i = lastUsedParticle; i < m_amount; ++i) {
    if (m_particles[i].life <= 0.0f){
      lastUsedParticle = i;
      return i;
    }
  }
  // otherwise, do a linear search
  for (unsigned int i = 0; i < lastUsedParticle; ++i){
    if (m_particles[i].life <= 0.0f){
      lastUsedParticle = i;
      return i;
    }
  }
  // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
  lastUsedParticle = 0;
  return 0;
}

void ParticleGenerator::respawnParticle(
  Particle& particle,
  GameObject& object,
  glm::vec2 offset)
{
  float random = ((rand() % 100) - 50) / 10.0f;
  float rColor = 0.5f + ((rand() % 100) / 100.0f);
  particle.position = object.m_position + random + offset;
  particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
  particle.life = 1.0f;
  particle.velocity = object.m_velocity * 0.1f;
}


