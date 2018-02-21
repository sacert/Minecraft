#include <GL/glew.h>
#include "camera.h"
#include "texture.h"

class GUI {
    public:
        void LoadCrosshair();
        void RenderCrosshair();
        void LoadInventory();
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