/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/

#include <pangolin/glfw-support.hpp>
#include <pangolin/resource-manager.hpp>

#include <breakout/game.hpp> 

#include <iostream>

// GLFW function declerations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 800;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[]) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, false);

  GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // OpenGL configuration
  // --------------------
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // initialize game
  // ---------------
  Breakout.init();

  // deltaTime variables
  // -------------------
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  // start game within menu state
  // ----------------------------
  Breakout.state = GAME_MENU;

  while (!glfwWindowShouldClose(window)) {
    // calculate delta time
    // --------------------
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glfwPollEvents();

    // manage user input
    // -----------------
    Breakout.process_input(deltaTime);

    // update game state
    // -----------------
    Breakout.update(deltaTime);

    // render
    // ------
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Breakout.render();

    glfwSwapBuffers(window);
  }

  // delete all resources as loaded using the resource manager
  // ---------------------------------------------------------
  ResourceManager::clear();

  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
  // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS)
      Breakout.keys[key] = true;
    else if (action == GLFW_RELEASE) {
      Breakout.keys[key] = false;
      Breakout.key_processed[key] = false;
    }

  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}
