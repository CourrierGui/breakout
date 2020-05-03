#include <breakout/game.hpp>

Game::Game(unsigned int width, unsigned int height)
  : width(width), height(height)
{

}

Game::~Game() {

}

SpriteRenderer  *renderer;

void Game::init() {
  // load shaders
  ResourceManager::load_shader(
    "../resources/shaders/sprite.vs",
    "../resources/shaders/sprite.fs",
    nullptr, "sprite");

  // configure shaders
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 
                                    static_cast<float>(height), 0.0f, -1.0f, 1.0f);
  ResourceManager::get_shader("sprite").use().setInteger("image", 0);
  ResourceManager::get_shader("sprite").setMatrix4("projection", projection);
  // set render-specific controls
  renderer = new SpriteRenderer(ResourceManager::get_shader("sprite"));
  // load textures
  ResourceManager::load_texture("../resources/textures/awesomeface.png", true, "face");
}

void Game::update(float dt) {

}

void Game::render() {
  renderer->draw(ResourceManager::get_texture("face"),
                 glm::vec2(200.0f, 200.0f),
                 glm::vec2(300.0f, 400.0f),
                 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Game::process_input(float ft) {

}
