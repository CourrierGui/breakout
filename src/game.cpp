#include <breakout/game.hpp>

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -250.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

pgl::GameObject*        player;
BallObject*        ball;
pgl::render2D::SpriteRenderer*    renderer;
pgl::ParticleGenerator* particles;
PostProcessor*     effects;
pgl::text::TextRenderer*      text;

irrklang::ISoundEngine* sound_engine = irrklang::createIrrKlangDevice();

float shake_time = 0.0f; 

Game::Game(unsigned int width, unsigned int height)
  : width(width), height(height)
{

}

Game::~Game() {
  /* delete renderer; */
  /* delete player; */
  /* delete ball; */
  /* delete particles; */
  /* delete effects; */
  /* delete text; */
  /* delete sound_engine; */
}

void Game::init() {
  lives = 3;
  // load shaders
  pgl::resources::ResourceManager::load_shader(
    "../resources/shaders/sprite.vs",
    "../resources/shaders/sprite.fs",
    "", "sprite");
  pgl::resources::ResourceManager::load_shader(
    "../resources/shaders/particle.vs",
    "../resources/shaders/particle.fs",
    "", "particle");
  pgl::resources::ResourceManager::load_shader(
    "../resources/shaders/postprocessor.vs",
    "../resources/shaders/postprocessor.fs",
    "", "postprocessing");
  pgl::resources::ResourceManager::load_shader(
    "../resources/shaders/text.vs",
    "../resources/shaders/text.fs",
    "", "text"
  );


  // configure shaders
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),
                                    static_cast<float>(height), 0.0f, -1.0f, 1.0f);
  pgl::resources::ResourceManager::get_shader("sprite").use().setInteger("image", 0);
  pgl::resources::ResourceManager::get_shader("sprite").setMatrix4("projection", projection);
  pgl::resources::ResourceManager::get_shader("particle").use().setInteger("sprite", 0);
  pgl::resources::ResourceManager::get_shader("particle").setMatrix4("projection", projection);
  // set render-specific controls
  renderer = new pgl::render2D::SpriteRenderer(pgl::resources::ResourceManager::get_shader("sprite"));
  effects = new PostProcessor(pgl::resources::ResourceManager::get_shader("postprocessing"),
                              width, height);
  sound_engine->play2D("../resources/sound/breakout.mp3", true);

  // load textures
  pgl::resources::ResourceManager::load_texture("../resources/textures/background.jpg",          false, "background");
  pgl::resources::ResourceManager::load_texture("../resources/textures/awesomeface.png",         true,  "face");
  pgl::resources::ResourceManager::load_texture("../resources/textures/block.png",               false, "block");
  pgl::resources::ResourceManager::load_texture("../resources/textures/block_solid.png",         false, "block_solid");
  pgl::resources::ResourceManager::load_texture("../resources/textures/paddle.png",              true,  "paddle");
  pgl::resources::ResourceManager::load_texture("../resources/textures/particle.png",            true,  "particle"); 
  pgl::resources::ResourceManager::load_texture("../resources/textures/powerup_speed.png",       true,  "powerup_speed");
  pgl::resources::ResourceManager::load_texture("../resources/textures/powerup_sticky.png",      true,  "powerup_sticky");
  pgl::resources::ResourceManager::load_texture("../resources/textures/powerup_increase.png",    true,  "powerup_increase");
  pgl::resources::ResourceManager::load_texture("../resources/textures/powerup_confuse.png",     true,  "powerup_confuse");
  pgl::resources::ResourceManager::load_texture("../resources/textures/powerup_chaos.png",       true,  "powerup_chaos");
  pgl::resources::ResourceManager::load_texture("../resources/textures/powerup_passthrough.png", true,  "powerup_passthrough");

  // load levels
  GameLevel one;   one.load("../resources/levels/one.lvl",   width, height / 2);
  GameLevel two;   two.load("../resources/levels/two.lvl",   width, height / 2);
  GameLevel three; three.load("../resources/levels/three.lvl", width, height / 2);
  GameLevel four;  four.load("../resources/levels/four.lvl",  width, height / 2);
  levels.push_back(one);
  levels.push_back(two);
  levels.push_back(three);
  levels.push_back(four);
  level = 0;

  glm::vec2 player_pos = glm::vec2(
    width / 2.0f - PLAYER_SIZE.x / 2.0f,
    height - PLAYER_SIZE.y
  );
  player = new pgl::GameObject(player_pos, PLAYER_SIZE, pgl::resources::ResourceManager::get_texture("paddle"));

  glm::vec2 ball_pos = player_pos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                            -BALL_RADIUS * 2.0f);
  ball = new BallObject(ball_pos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                        pgl::resources::ResourceManager::get_texture("face"));
  text = new pgl::text::TextRenderer(width, height, pgl::resources::ResourceManager::get_shader("text"));
  text->load("../resources/fonts/ocraext.TTF", 24);

  particles = new pgl::ParticleGenerator(
    pgl::resources::ResourceManager::get_shader("particle"), 
    pgl::resources::ResourceManager::get_texture("particle"), 
    500
  );
}

void Game::update(float dt) {
  ball->move(dt, width);
  process_collisions();
  particles->update(dt, *ball, 2, glm::vec2(ball->radius / 2.0f));

  if (shake_time > 0.0f) {
    shake_time -= dt;
    if (shake_time <= 0.0f) {
      effects->shake = false;
    }
  }
  update_power_ups(dt);

  if (ball->position.y >= height) { // did ball reach bottom edge?
    --lives;
    if (lives == 0) {
      reset_level();
      state = GAME_MENU;
    }
    reset_player();
  }

  if (state == GAME_ACTIVE && levels[level].isCompleted()) {
    reset_level();
    reset_player();
    effects->chaos = true;
    state = GAME_WIN;
  }
}

void Game::render() {
  if(state == GAME_ACTIVE || state == GAME_MENU) {
    // draw background
    effects->begin_render();
    renderer->draw(pgl::resources::ResourceManager::get_texture("background"),
                   glm::vec2(0.0f, 0.0f), glm::vec2(width, height), 0.0f);
    // draw level
    levels[level].draw(*renderer);
    player->draw(*renderer);
    particles->draw(); 
    for (PowerUp &powerUp : power_ups)
      if (!powerUp.destroyed)
        powerUp.draw(*renderer);
    ball->draw(*renderer);
    effects->end_render();
    effects->render(glfwGetTime());

    std::stringstream ss; ss << lives;
    text->render_text("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
  }
  if (state == GAME_MENU) {
    text->render_text("Press ENTER to start", 250.0f, height / 2, 1.0f);
    text->render_text("Press W or S to select level", 245.0f, height / 2 + 20.0f, 0.75f);
  }
  if (state == GAME_WIN) {
    text->render_text(
      "You WON!!!", 320.0, height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0)
      );
    text->render_text(
      "Press ENTER to retry or ESC to quit", 130.0, height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0)
      );
  }
}

bool should_spawn(unsigned int chance) {
  unsigned int random = rand() % chance;
  return random == 0;
}

void Game::spawn_power_ups(pgl::GameObject& block) {
  if (should_spawn(GOOD_RATE)) // 1 in GOOD_RATE chance
    power_ups.push_back(
      PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f,
              block.position, pgl::resources::ResourceManager::get_texture("powerup_speed")));
  if (should_spawn(GOOD_RATE))
    power_ups.push_back(
      PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f,
              block.position, pgl::resources::ResourceManager::get_texture("powerup_sticky")));
  if (should_spawn(GOOD_RATE))
      power_ups.push_back(
        PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f,
                block.position, pgl::resources::ResourceManager::get_texture("powerup_passthrough")));
  if (should_spawn(GOOD_RATE))
  power_ups.push_back(
        PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f,
                block.position, pgl::resources::ResourceManager::get_texture("powerup_increase")));
  if (should_spawn(BAD_RATE)) // negative powerups should spawn more often
    power_ups.push_back(
      PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 5.0f,
              block.position, pgl::resources::ResourceManager::get_texture("powerup_confuse")));
  if (should_spawn(BAD_RATE))
    power_ups.push_back(
      PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 5.0f,
              block.position, pgl::resources::ResourceManager::get_texture("powerup_chaos")));
}

void Game::reset_level() {
  lives = 3;
  if (level == 0)
    levels[0].load("../resources/levels/one.lvl", width, height / 2);
  else if (level == 1)
    levels[1].load("../resources/levels/two.lvl", width, height / 2);
  else if (level == 2)
    levels[2].load("../resources/levels/three.lvl", width, height / 2);
  else if (level == 3)
    levels[3].load("../resources/levels/four.lvl", width, height / 2);
}

void Game::reset_player() {
  // reset player/ball stats
  player->size = PLAYER_SIZE;
  player->position = glm::vec2(width / 2.0f - PLAYER_SIZE.x / 2.0f, height - PLAYER_SIZE.y);
  ball->reset(
    player->position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                 -(BALL_RADIUS * 2.0f)),
    INITIAL_BALL_VELOCITY);
}

void Game::process_input(float dt) {
  if (state == GAME_ACTIVE) {
    float velocity = PLAYER_VELOCITY * dt;
    // move playerboard
    if (keys[GLFW_KEY_A]) {
      if (player->position.x >= 0.0f) {
        player->position.x -= velocity;
        if (ball->stuck)
          ball->position.x -= velocity;
      }
    }
    if (keys[GLFW_KEY_D]) {
      if (player->position.x <= width - player->size.x) {
        player->position.x += velocity;
        if (ball->stuck)
          ball->position.x += velocity;
      }
    }
    if (keys[GLFW_KEY_SPACE])
      ball->stuck = false;
  }

  if (state == GAME_MENU && !key_processed[GLFW_KEY_ENTER]) {
    if (keys[GLFW_KEY_ENTER]) {
      state = GAME_ACTIVE;
      key_processed[GLFW_KEY_ENTER] = true;
    }
    if (keys[GLFW_KEY_W] && !key_processed[GLFW_KEY_W]) {
      level = (level + 1) % 4;
      key_processed[GLFW_KEY_W] = true;
    }
    if (keys[GLFW_KEY_S] && !key_processed[GLFW_KEY_W]) {
      if (level > 0)
        --level;
      else
        level = 3;   
      key_processed[GLFW_KEY_S] = true;
    }
  }

  if (state == GAME_WIN) {
    if (keys[GLFW_KEY_ENTER]) {
      key_processed[GLFW_KEY_ENTER] = true;
      effects->chaos = false;
      state = GAME_MENU;
    }
  }
}

void Game::process_collisions() {
  for (pgl::GameObject& box: levels[level].bricks) {
    if (!box.destroyed) {
      Collision collision = CheckCollision(*ball, box);
      if (std::get<0>(collision)) {
        if (!box.is_solid) {
          box.destroyed = true;
          this->spawn_power_ups(box);
          sound_engine->play2D("../resources/sound/bleep.mp3", GL_FALSE);
        } else {   // if block is solid, enable shake effect
          shake_time = 0.05f;
          effects->shake = true;
          sound_engine->play2D("../resources/sound/solid.wav", GL_FALSE);
        }
        Direction dir = std::get<1>(collision);
        glm::vec2 diff_vector = std::get<2>(collision);
        if (!(ball->pass_through && !box.is_solid)) {
          if (dir == LEFT || dir == RIGHT) { // horizontal collision
            ball->velocity.x = -ball->velocity.x; // reverse horizontal velocity
            // relocate
            float penetration = ball->radius - std::abs(diff_vector.x);
            if (dir == LEFT)
              ball->position.x += penetration; // move ball to right
            else
              ball->position.x -= penetration; // move ball to left;
          }
          else { // vertical collision
            ball->velocity.y = -ball->velocity.y; // reverse vertical velocity
            // relocate
            float penetration = ball->radius - std::abs(diff_vector.y);
            if (dir == UP)
              ball->position.y -= penetration; // move ball back up
            else
              ball->position.y += penetration; // move ball back down
          }
        }
      }
    }
  }

  for (PowerUp& powerUp : power_ups) {
    if (!powerUp.destroyed) {
      if (powerUp.position.y >= height)
        powerUp.destroyed = true;
      if (CheckCollision(*player, powerUp)) {
        // collided with player, now activate powerup
        ActivatePowerUp(powerUp);
        powerUp.destroyed = true;
        powerUp.Activated = true;
        sound_engine->play2D("../resources/sound/powerup.wav", false);
      }
    }
  }

  Collision result = CheckCollision(*ball, *player);
  if (!ball->stuck && std::get<0>(result)) {
    // check where it hit the board, and change velocity based on where it hit the board
    ball->stuck = ball->sticky;
    float centerBoard = player->position.x + player->size.x / 2.0f;
    float distance = (ball->position.x + ball->radius) - centerBoard;
    float percentage = distance / (player->size.x / 2.0f);

    // then move accordingly
    float strength = 2.0f;
    glm::vec2 oldvelocity = ball->velocity;
    ball->velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    ball->velocity.y = -1.0f * std::abs(ball->velocity.y);  
    ball->velocity = glm::normalize(ball->velocity) * glm::length(oldvelocity);
    sound_engine->play2D("../resources/sound/bleep.wav", false);
  }
}

void ActivatePowerUp(PowerUp& powerUp) {
  if (powerUp.Type == "speed") {
    ball->velocity *= 1.2;
  }
  else if (powerUp.Type == "sticky") {
    ball->sticky = true;
    player->color = glm::vec3(1.0f, 0.5f, 1.0f);
  }
  else if (powerUp.Type == "pass-through") {
    ball->pass_through = true;
    ball->color = glm::vec3(1.0f, 0.5f, 0.5f);
  }
  else if (powerUp.Type == "pad-size-increase") {
    player->size.x += 50;
  }
  else if (powerUp.Type == "confuse") {
    if (!effects->chaos)
      effects->confuse = true; // only activate if chaos wasn't already active
  }
  else if (powerUp.Type == "chaos") {
    if (!effects->confuse)
      effects->chaos = true;
  }
} 

void Game::update_power_ups(float dt) {
  for (PowerUp &powerUp : power_ups) {
    powerUp.position += powerUp.velocity * dt;
    if (powerUp.Activated) {
      powerUp.Duration -= dt;

      if (powerUp.Duration <= 0.0f) {
        // remove powerup from list (will later be removed)
        powerUp.Activated = false;
        // deactivate effects
        if (powerUp.Type == "sticky") {
          if (!isOtherPowerUpActive(power_ups, "sticky")) {
            // only reset if no other PowerUp of type sticky is active
            ball->sticky = false;
            player->color = glm::vec3(1.0f);
          }
        }
        else if (powerUp.Type == "pass-through") {
          if (!isOtherPowerUpActive(power_ups, "pass-through")) {
            // only reset if no other PowerUp of type pass-through is active
            ball->pass_through = false;
            ball->color = glm::vec3(1.0f);
          }
        }
        else if (powerUp.Type == "confuse") {
          if (!isOtherPowerUpActive(power_ups, "confuse")) {
            // only reset if no other PowerUp of type confuse is active
            effects->confuse = false;
          }
        }
        else if (powerUp.Type == "chaos") {
          if (!isOtherPowerUpActive(power_ups, "chaos")) {
            // only reset if no other PowerUp of type chaos is active
            effects->chaos = false;
          }
        }                
      }
    }
  }
  power_ups.erase(
    std::remove_if(
      power_ups.begin(),
      power_ups.end(),
      [](const PowerUp &powerUp) {
        return powerUp.destroyed && !powerUp.Activated;
      }),
    power_ups.end());
}

bool isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type) {
  for (const PowerUp &powerUp : powerUps) {
    if (powerUp.Activated)
      if (powerUp.Type == type)
        return true;
  }
  return false;
} 

bool CheckCollision(pgl::GameObject& one, pgl::GameObject& two) { // AABB - AABB collision
  // Collision x-axis?
  bool collisionX = one.position.x + one.size.x >= two.position.x &&
    two.position.x + two.size.x >= one.position.x;
  // Collision y-axis?
  bool collisionY = one.position.y + one.size.y >= two.position.y &&
    two.position.y + two.size.y >= one.position.y;
  // Collision only if on both axes
  return collisionX && collisionY;
}

Collision CheckCollision(BallObject &one, pgl::GameObject &two) { // AABB - Circle collision
  // get center point circle first
  glm::vec2 center(one.position + one.radius);
  // calculate AABB info (center, half-extents)
  glm::vec2 aabb_half_extents(two.size.x / 2.0f, two.size.y / 2.0f);
  glm::vec2 aabb_center(
    two.position.x + aabb_half_extents.x,
    two.position.y + aabb_half_extents.y
    );
  // get difference vector between both centers
  glm::vec2 difference = center - aabb_center;
  glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
  // add clamped value to AABB_center and we get the value of box closest to circle
  glm::vec2 closest = aabb_center + clamped;
  // retrieve vector between center circle and closest point AABB and check if length <= radius
  difference = closest - center;

  if (glm::length(difference) < one.radius) {
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
