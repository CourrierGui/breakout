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

irrklang::ISoundEngine* sound_engine = irrklang::createIrrKlangDevice();

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
    "../../resources/shaders/sprite.vs",
    "../../resources/shaders/sprite.fs",
    nullptr, "sprite");
  ResourceManager::load_shader(
    "../../resources/shaders/particle.vs",
    "../../resources/shaders/particle.fs",
    nullptr, "particle");
  ResourceManager::load_shader(
    "../../resources/shaders/postprocessor.vs",
    "../../resources/shaders/postprocessor.fs",
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
  sound_engine->play2D("../../resources/sound/breakout.mp3", true);

  // load textures
  ResourceManager::load_texture("../../resources/textures/background.jpg",          false, "background");
  ResourceManager::load_texture("../../resources/textures/awesomeface.png",         true,  "face");
  ResourceManager::load_texture("../../resources/textures/block.png",               false, "block");
  ResourceManager::load_texture("../../resources/textures/block_solid.png",         false, "block_solid");
  ResourceManager::load_texture("../../resources/textures/paddle.png",              true,  "paddle");
  ResourceManager::load_texture("../../resources/textures/particle.png",            true,  "particle"); 
  ResourceManager::load_texture("../../resources/textures/powerup_speed.png",       true,  "powerup_speed");
  ResourceManager::load_texture("../../resources/textures/powerup_sticky.png",      true,  "powerup_sticky");
  ResourceManager::load_texture("../../resources/textures/powerup_increase.png",    true,  "powerup_increase");
  ResourceManager::load_texture("../../resources/textures/powerup_confuse.png",     true,  "powerup_confuse");
  ResourceManager::load_texture("../../resources/textures/powerup_chaos.png",       true,  "powerup_chaos");
  ResourceManager::load_texture("../../resources/textures/powerup_passthrough.png", true,  "powerup_passthrough");

  // load levels
  GameLevel one;   one.load("../../resources/levels/one.lvl",   width, height / 2);
  GameLevel two;   two.load("../../resources/levels/two.lvl",   width, height / 2);
  GameLevel three; three.load("../../resources/levels/three.lvl", width, height / 2);
  GameLevel four;  four.load("../../resources/levels/four.lvl",  width, height / 2);
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
  update_power_ups(dt);

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
    for (PowerUp &powerUp : m_power_ups)
      if (!powerUp.m_destroyed)
        powerUp.draw(*renderer);
    ball->draw(*renderer);
    effects->EndRender();
    effects->Render(glfwGetTime());
  }
}

bool should_spawn(unsigned int chance) {
  unsigned int random = rand() % chance;
  return random == 0;
}

void Game::spawn_power_ups(GameObject& block) {
  if (should_spawn(GOOD_RATE)) // 1 in GOOD_RATE chance
    m_power_ups.push_back(
      PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f,
              block.m_position, ResourceManager::get_texture("powerup_speed")));
  if (should_spawn(GOOD_RATE))
    m_power_ups.push_back(
      PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f,
              block.m_position, ResourceManager::get_texture("powerup_sticky")));
  if (should_spawn(GOOD_RATE))
      m_power_ups.push_back(
        PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f,
                block.m_position, ResourceManager::get_texture("powerup_passthrough")));
  if (should_spawn(GOOD_RATE))
  m_power_ups.push_back(
        PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f,
                block.m_position, ResourceManager::get_texture("powerup_increase")));
  if (should_spawn(BAD_RATE)) // negative powerups should spawn more often
    m_power_ups.push_back(
      PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 5.0f,
              block.m_position, ResourceManager::get_texture("powerup_confuse")));
  if (should_spawn(BAD_RATE))
    m_power_ups.push_back(
      PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 5.0f,
              block.m_position, ResourceManager::get_texture("powerup_chaos")));
}

void Game::reset_level() {
  if (m_level == 0)
    m_levels[0].load("../../resources/levels/one.lvl", width, height / 2);
  else if (m_level == 1)
    m_levels[1].load("../../resources/levels/two.lvl", width, height / 2);
  else if (m_level == 2)
    m_levels[2].load("../../resources/levels/three.lvl", width, height / 2);
  else if (m_level == 3)
    m_levels[3].load("../../resources/levels/four.lvl", width, height / 2);
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
        if (!box.m_isSolid) {
          box.m_destroyed = true;
          this->spawn_power_ups(box);
          sound_engine->play2D("../../resources/sound/bleep.mp3", GL_FALSE);
        } else {   // if block is solid, enable shake effect
          shake_time = 0.05f;
          effects->Shake = true;
          sound_engine->play2D("../../resources/sound/solid.wav", GL_FALSE);
        }
        Direction dir = std::get<1>(collision);
        glm::vec2 diff_vector = std::get<2>(collision);
        if (!(ball->m_pass_through && !box.m_isSolid)) {
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
  }

  for (PowerUp& powerUp : m_power_ups) {
    if (!powerUp.m_destroyed) {
      if (powerUp.m_position.y >= height)
        powerUp.m_destroyed = true;
      if (CheckCollision(*player, powerUp)) {
        // collided with player, now activate powerup
        ActivatePowerUp(powerUp);
        powerUp.m_destroyed = true;
        powerUp.Activated = true;
        sound_engine->play2D("../../resources/sound/powerup.wav", true);
      }
    }
  }

  Collision result = CheckCollision(*ball, *player);
  if (!ball->m_stuck && std::get<0>(result)) {
    // check where it hit the board, and change velocity based on where it hit the board
    ball->m_stuck = ball->m_sticky;
    float centerBoard = player->m_position.x + player->m_size.x / 2.0f;
    float distance = (ball->m_position.x + ball->m_radius) - centerBoard;
    float percentage = distance / (player->m_size.x / 2.0f);

    // then move accordingly
    float strength = 2.0f;
    glm::vec2 oldm_velocity = ball->m_velocity;
    ball->m_velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    ball->m_velocity.y = -1.0f * std::abs(ball->m_velocity.y);  
    ball->m_velocity = glm::normalize(ball->m_velocity) * glm::length(oldm_velocity);
    sound_engine->play2D("../../resources/sound/bleep.wav", false);
  }
}

void ActivatePowerUp(PowerUp& powerUp) {
  if (powerUp.Type == "speed") {
    ball->m_velocity *= 1.2;
  }
  else if (powerUp.Type == "sticky") {
    ball->m_sticky = true;
    player->m_color = glm::vec3(1.0f, 0.5f, 1.0f);
  }
  else if (powerUp.Type == "pass-through") {
    ball->m_pass_through = true;
    ball->m_color = glm::vec3(1.0f, 0.5f, 0.5f);
  }
  else if (powerUp.Type == "pad-size-increase") {
    player->m_size.x += 50;
  }
  else if (powerUp.Type == "confuse") {
    if (!effects->Chaos)
      effects->Confuse = true; // only activate if chaos wasn't already active
  }
  else if (powerUp.Type == "chaos") {
    if (!effects->Confuse)
      effects->Chaos = true;
  }
} 

void Game::update_power_ups(float dt) {
  for (PowerUp &powerUp : m_power_ups) {
    powerUp.m_position += powerUp.m_velocity * dt;
    if (powerUp.Activated) {
      powerUp.Duration -= dt;

      if (powerUp.Duration <= 0.0f) {
        // remove powerup from list (will later be removed)
        powerUp.Activated = false;
        // deactivate effects
        if (powerUp.Type == "sticky") {
          if (!isOtherPowerUpActive(m_power_ups, "sticky")) {
            // only reset if no other PowerUp of type sticky is active
            ball->m_sticky = false;
            player->m_color = glm::vec3(1.0f);
          }
        }
        else if (powerUp.Type == "pass-through") {
          if (!isOtherPowerUpActive(m_power_ups, "pass-through")) {
            // only reset if no other PowerUp of type pass-through is active
            ball->m_pass_through = false;
            ball->m_color = glm::vec3(1.0f);
          }
        }
        else if (powerUp.Type == "confuse") {
          if (!isOtherPowerUpActive(m_power_ups, "confuse")) {
            // only reset if no other PowerUp of type confuse is active
            effects->Confuse = false;
          }
        }
        else if (powerUp.Type == "chaos") {
          if (!isOtherPowerUpActive(m_power_ups, "chaos")) {
            // only reset if no other PowerUp of type chaos is active
            effects->Chaos = false;
          }
        }                
      }
    }
  }
  m_power_ups.erase(
    std::remove_if(
      m_power_ups.begin(),
      m_power_ups.end(),
      [](const PowerUp &powerUp) {
        return powerUp.m_destroyed && !powerUp.Activated;
      }),
    m_power_ups.end());
}

bool isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type) {
  for (const PowerUp &powerUp : powerUps) {
    if (powerUp.Activated)
      if (powerUp.Type == type)
        return true;
  }
  return false;
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
