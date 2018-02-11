#include <GL/glew.h>
#include "camera.h"
#include "texture.h"

class GUI {
    public:
        void LoadGUI();
        void RenderCrosshair();
    private:
        GLuint guiVAO;
        GLuint guiVBO;
        GLint shaders;
        Texture* texture;
        double scaleX;
        double scaleY;
};