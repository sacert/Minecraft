#include <GL/glew.h>
#include "camera.h"

class SkyBox {
    public:
        void LoadSkyBox();
        void Render(Camera camera);
    private:
        GLuint skyboxVAO;
        GLuint skyboxVBO;
        GLint shaders;
};