#ifndef CYLINDER_H
#define CYLINDER_H

#include <vector>
#include <glm/glm.hpp>

class Cylinder
{
public:
    Cylinder();

    // Method to generate cylinder data and store in a float vector (to be later uploaded to a VBO)
    void generateCylinderData(int heightSegments, int radialSegments, std::vector<float> &vbo);

private:
    // Helper methods
    void pushVertexWithNormalAndUV(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &uv, std::vector<float>& vbo);
    void makeWedge(float currentTheta, float nextTheta, int heightSegments, std::vector<float> &vbo);
    void makeCap(bool isTop, int radialSegments, int radiusSegments, std::vector<float> &vbo);
};

#endif // CYLINDER_H
