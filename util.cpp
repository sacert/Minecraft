#include "util.h"

// Textures are flipped vertically due to how opengl reads them
Texture* LoadTexture(std::string fileLocation) {
    Bitmap bmp = Bitmap::bitmapFromFile(fileLocation);
    return new Texture(bmp);
}