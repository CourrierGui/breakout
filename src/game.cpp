#include <breakout/game.hpp>

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -550.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

GameObject*        player;
BallObject*        ball;
SpriteRenderer*    renderer;
ParticleGenerator* particles;
PostProcessor*     effects;

float shake_time = 0.0f; 

Game::Game(unsigned int width, unsigned int height)
  : width(width), height(height)
{

}

Game::~Game() {
  delete renderer;
  delete player;
  delete ball;
  delete particles;
  delete effects;
}

void Game::init() {
  // load shaders
  ResourceManager::load_shader(
    "../resources/shaders/sprite.vs",
    "../resources/shaders/sprite.fs",
    nullptr, "sprite");
  ResourceManager::load_shader(
    "../resources/shaders/particle.vs",
    "../resources/shaders/particle.fs",
    nullptr, "particle");
  ResourceManager::load_shader(
    "../resources/shaders/postprocessor.vs",
    "../resources/shaders/postprocessor.fs",
    nullptr, "postprocessing");

  // configure shaders
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),
                                    static_cast<float>(height), 0.0f, -1.0f, 1.0f);
  ResourceManager::get_shader("sprite").use().setInteger("image", 0);
  ResourceManager::get_shader("sprite").setMatrix4("projection", projection);
  ResourceManager::get_shader("particle").use().setInteger("sprite", 0);
  ResourceManager::get_shader("particle").setMatrix4("projection", projection);
  // set render-specific controls
  renderer = new SpriteRenderer(ResourceManager::get_shader("sprite"));
  effects = new PostProcessor(ResourceManager::get_shader("postprocessing"),
                              width, height);

  // load textures
  ResourceManager::load_texture("../resources/textures/background.jpg", false, "background");
  ResourceManager::load_texture("../resources/textures/awesomeface.png", true, "face");
  ResourceManager::load_texture("../resources/textures/block.png", false, "block");
  ResourceManager::load_texture("../resources/textures/block_solid.png", false, "block_solid");
  ResourceManager::load_texture("../resources/textures/paddle.png", true, "paddle");
  ResourceManager::load_texture("../resources/textures/particle.png", true, "particle"); 

  // load levels
  GameLevel one;   one.load("../resources/levels/one.lvl",   width, height / 2);
  GameLevel two;   two.load("../resources/levels/two.lvl",   width, height / 2);
  GameLevel three; three.load("../resources/levels/three.lvl", width, height / 2);
  GameLevel four;  four.load("../resources/levels/four.lvl",  width, height / 2);
  m_levels.push_back(one);
  m_levels.push_back(two);
  m_levels.push_back(three);
  m_levels.push_back(four);
  m_level = 0;

  glm::vec2 player_pos = glm::vec2(
    width / 2.0f - PLAYER_SIZE.x / 2.0f,
    height - PLAYER_SIZE.y
  );
  player = new GameObject(player_pos, PLAYER_SIZE, ResourceManager::get_texture("paddle"));

  glm::vec2 ball_pos = player_pos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                            -BALL_RADIUS * 2.0f);
  ball = new BallObject(ball_pos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                        ResourceManager::get_texture("face"));

  particles = new ParticleGenerator(
    ResourceManager::get_shader("particle"), 
    ResourceManager::get_texture("particle"), 
    500
  );
}

void Game::update(float dt) {
  ball->move(dt, width);
  process_collisions();
  particles->update(dt, *ball, 2, glm::vec2(ball->m_radius / 2.0f));

  if (shake_time > 0.0f) {
    shake_time -= dt;
    if (shake_time <= 0.0f) {
      effects->Shake = false;
    }
  }

  if (ball->m_position.y >= height) { // did ball reach bottom edge?
    reset_level();
    reset_player();
  }
}

void Game::render() {
  if(m_state == GAME_ACTIVE) {
    // draw background
    effects->BeginRender();
    renderer->draw(ResourceManager::get_texture("background"),
                   glm::vec2(0.0f, 0.0f), glm::vec2(width, height), 0.0f);
    // draw level
    m_levels[m_level].draw(*renderer);
    player->draw(*renderer);
    particles->draw(); 
    ball->draw(*renderer);
    effects->EndRender();
    effects->Render(glfwGetTime());
  }
}

void Game::reset_level() {
  if (m_level == 0)
    m_levels[0].load("../resources/levels/one.lvl", width, height / 2);
  else if (m_level == 1)
    m_levels[1].load("../resources/levels/two.lvl", width, height / 2);
  else if (m_level == 2)
    m_levels[2].load("../resources/levels/three.lvl", width, height / 2);
  else if (m_level == 3)
    m_levels[3].load("../resources/levels/four.lvl", width, height / 2);
}

void Game::reset_player() {
  // reset player/ball stats
  player->m_size = PLAYER_SIZE;
  player->m_position = glm::vec2(width / 2.0f - PLAYER_SIZE.x / 2.0f, height - PLAYER_SIZE.y);
  ball->reset(
    player->m_position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                   -(BALL_RADIUS * 2.0f)),
    INITIAL_BALL_VELOCITY);
}

void Game::process_input(float dt) {
  if (m_state == GAME_ACTIVE) {
    float velocity = PLAYER_VELOCITY * dt;
    // move playerboard
    if (m_keys[GLFW_KEY_A]) {
      if (player->m_position.x >= 0.0f) {
        player->m_position.x -= velocity;
        if (ball->m_stuck)
          ball->m_position.x -= velocity;
      }
    }
    if (m_keys[GLFW_KEY_D]) {
      if (player->m_position.x <= width - player->m_size.x) {
        player->m_position.x += velocity;
        if (ball->m_stuck)
          ball->m_position.x += velocity;
      }
    }
    if (m_keys[GLFW_KEY_SPACE])
      ball->m_stuck = false;
  }
}

void Game::process_collisions() {
  for (GameObject& box: m_levels[m_level].m_bricks) {
    if (!box.m_destroyed) {
      Collision collision = CheckCollision(*ball, box);
      if (std::get<0>(collision)) {
        if (!box.m_isSolid)
          box.m_destroyed = true;
        else {   // if block is solid, enable shake effect
          shake_time = 0.05f;
          effects->Shake = true;
        }
        Direction dir = std::get<1>(collision);
        glm::vec2 diff_vector = std::get<2>(collision);
        if (dir == LEFT || dir == RIGHT) { // horizontal collision
          ball->m_velocity.x = -ball->m_velocity.x; // reverse horizontal velocity
          // relocate
          float penetration = ball->m_radius - std::abs(diff_vector.x);
          if (dir == LEFT)
            ball->m_position.x += penetration; // move ball to right
          else
            ball->m_position.x -= penetration; // move ball to left;
        }
        else { // vertical collision
          ball->m_velocity.y = -ball->m_velocity.y; // reverse vertical velocity
          // relocate
          float penetration = ball->m_radius - std::abs(diff_vector.y);
          if (dir == UP)
            ball->m_position.y -= penetration; // move ball back up
          else
            ball->m_position.y += penetration; // move ball back down
        }
      }
    }
  }

  Collision result = CheckCollision(*ball, *player);
  if (!ball->m_stuck && std::get<0>(result)) {
    // check where it hit the board, and change velocity based on where it hit the board
    float centerBoard = player->m_position.x + player->m_size.x / 2.0f;
    float distance = (ball->m_position.x + ball->m_radius) - centerBoard;
    float percentage = distance / (player->m_size.x / 2.0f);

    // then move accordingly
    float strength = 2.0f;
    glm::vec2 oldm_velocity = ball->m_velocity;
    ball->m_velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    ball->m_velocity.y = -1.0f * std::abs(ball->m_velocity.y);  
    ball->m_velocity = glm::normalize(ball->m_velocity) * glm::length(oldm_velocity);
  }
}

bool CheckCollision(GameObject& one, GameObject& two) { // AABB - AABB collision
  // Collision x-axis?
  bool collisionX = one.m_position.x + one.m_size.x >= two.m_position.x &&
    two.m_position.x + two.m_size.x >= one.m_position.x;
  // Collision y-axis?
  bool collisionY = one.m_position.y + one.m_size.y >= two.m_position.y &&
    two.m_position.y + two.m_size.y >= one.m_position.y;
  // Collision only if on both axes
  return collisionX && collisionY;
}

Collision CheckCollision(BallObject &one, GameObject &two) { // AABB - Circle collision
  // get center point circle first
  glm::vec2 center(one.m_position + one.m_radius);
  // calculate AABB info (center, half-extents)
  glm::vec2 aabb_half_extents(two.m_size.x / 2.0f, two.m_size.y / 2.0f);
  glm::vec2 aabb_center(
    two.m_position.x + aabb_half_extents.x,
    two.m_position.y + aabb_half_extents.y
    );
  // get difference vector between both centers
  glm::vec2 difference = center - aabb_center;
  glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
  // add clamped value to AABB_center and we get the value of box closest to circle
  glm::vec2 closest = aabb_center + clamped;
  // retrieve vector between center circle and closest point AABB and check if length <= radius
  difference = closest - center;

  if (glm::length(difference) < one.m_radius) {
    return {true, vector_direction(difference), difference};
  } else {
    return {false, UP, glm::vec2(0.0f, 0.0f)};
  }
}

Direction vector_direction(glm::vec2 target) {
  glm::vec2 compass[] = {
    glm::vec2(0.0f, 1.0f),	// up
    glm::vec2(1.0f, 0.0f),	// right
    glm::vec2(0.0f, -1.0f),	// down
    glm::vec2(-1.0f, 0.0f)	// left
  };
  float max = 0.0f;
  unsigned int best_match = -1;
  for (unsigned int i = 0; i < 4; i++) {
    float dot_product = glm::dot(glm::normalize(target), compass[i]);
    if (dot_product > max) {
      max = dot_product;
      best_match = i;
    }
  }
  return (Direction)best_match;
}
