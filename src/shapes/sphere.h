#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
    Sphere();

    // Method to generate sphere data and store in a float vector (to be later uploaded to a VBO)
    void generateSphereData(int phiTesselations, int thetaTesselations, std::vector<float> &vbo);

private:
    // Helper methods
    glm::vec4 sphericalToCartesian(float phi, float theta);
    void pushVertexWithNormalAndUV(glm::vec4 vec, glm::vec2 uv, std::vector<float>& vbo);
};

#endif // SPHERE_H
