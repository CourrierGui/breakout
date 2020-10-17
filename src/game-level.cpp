#include <breakout/game-level.hpp>

void GameLevel::load(
  const char* file,
  unsigned int level_width,
  unsigned int level_height)
{
  // clear old data
  bricks.clear();

  // load from file
  unsigned int tileCode;
  GameLevel level;
  std::string line;
  std::ifstream fstream(file);
  std::vector<std::vector<unsigned int>> tile_data;
  if (fstream) {
    while (std::getline(fstream, line)) { // read each line from level file
      std::istringstream sstream(line);
      std::vector<unsigned int> row;
      while (sstream >> tileCode) // read each word seperated by spaces
        row.push_back(tileCode);
      tile_data.push_back(row);
    }
    if (tile_data.size() > 0)
      init(tile_data, level_width, level_height);
  }
}

void GameLevel::init(
  std::vector<std::vector<unsigned int>>& tile_data,
  unsigned int level_width,
  unsigned int level_height)
{
  // calculate dimensions
  unsigned int height = tile_data.size();
  unsigned int width  = tile_data[0].size();
  float unit_width    = level_width / static_cast<float>(width);
  float unit_height   = level_height / height;
  // initialize level tiles based on tile_data		
  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      // check block type from level data (2D level array)
      if (tile_data[y][x] == 1) { // solid
        pgl::float2 pos(unit_width * x, unit_height * y);
        pgl::float2 size(unit_width, unit_height);
        pgl::GameObject obj(
          pos, size, 
          pgl::ResourceManager::get_texture("block_solid"), 
          pgl::float3(0.8f, 0.8f, 0.7f));
        obj.is_solid = true;
        bricks.push_back(obj);
      }
      else if (tile_data[y][x] > 1)	{
        pgl::float3 color = pgl::float3(1.0f); // original: white
        if (tile_data[y][x] == 2)
          color = pgl::float3(0.2f, 0.6f, 1.0f);
        else if (tile_data[y][x] == 3)
          color = pgl::float3(0.0f, 0.7f, 0.0f);
        else if (tile_data[y][x] == 4)
          color = pgl::float3(0.8f, 0.8f, 0.4f);
        else if (tile_data[y][x] == 5)
          color = pgl::float3(1.0f, 0.5f, 0.0f);

        pgl::float2 pos(unit_width * x, unit_height * y);
        pgl::float2 size(unit_width, unit_height);
        bricks.push_back(
          pgl::GameObject(pos, size, pgl::ResourceManager::get_texture("block"), color));
      }
    }
  }  
}

void GameLevel::draw(pgl::render2D::SpriteRenderer& renderer) {
  for (pgl::GameObject& tile : bricks)
    if (!tile.destroyed)
      tile.draw(renderer);
}

bool GameLevel::isCompleted() {
  for (pgl::GameObject& tile : bricks)
    if (!tile.is_solid && !tile.destroyed)
      return false;
  return true;
}
