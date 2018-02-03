#include <glm/glm.hpp>

typedef struct {
    float x, y ,z;
} Point;

class Frustum {
    public:
        void getFrustum(glm::mat4 modelViewMatrix, glm::mat4 projectionMatrix);
        bool cubeInFrustum(float x, float y, float z, float size);
    private:
        float frustum[6][4];
};