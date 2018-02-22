#include <GL/glew.h>
#include "camera.h"
#include "texture.h"
#include "chunk_manager.h"

class GUI {
    public:
        GUI();
        void LoadCrosshair();
        void RenderCrosshair();
        void LoadInventory(BlockType selected);
        void RenderInventory();
    private:
        GLuint guiVAO;
        GLuint guiVBO;
        GLint crosshair_shaders;
        Texture* crosshair_texture;
        double scaleX;
        double scaleY;
        Camera camera;
        GLuint inventoryVAO;
        GLuint inventoryVBO;
        GLint inventory_shaders;
        Texture* inventory_texture;
};