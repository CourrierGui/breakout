/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#pragma once

#include <string>

#include <glad/glad.h>

#include <pangolin/game-object.hpp>
#include <pgl-math/vector.hpp>

// The size of a PowerUp block
const pgl::float2 POWERUP_SIZE(60.0f, 20.0f);
// Velocity a PowerUp block has when spawned
const pgl::float2 VELOCITY(0.0f, 150.0f);


// PowerUp inherits its state and rendering functions from
// GameObject but also holds extra information to state its
// active duration and whether it is activated or not. 
// The type of PowerUp is stored as a string.
class PowerUp : public pgl::GameObject {
  public:
    // powerup state
    std::string Type;
    float       Duration;	
    bool        Activated;
    // constructor
    PowerUp(
      std::string type, pgl::float3 color,
      float duration, pgl::float2 position,
      pgl::Texture2D texture) 
      : GameObject(position, POWERUP_SIZE,
                   texture, color, VELOCITY),
      Type(type), Duration(duration),
      Activated() { }
};
