#include "texture.h"
#include <string>
#include <glm/glm.hpp>

// constants
const glm::vec2 SCREEN_SIZE(800, 600);
const int CHUNK_RENDER_DISTANCE = 4;

Texture* LoadTexture(std::string fileLocation); 