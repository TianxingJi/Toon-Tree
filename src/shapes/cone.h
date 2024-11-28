#ifndef CONE_H
#define CONE_H

#include <vector>
#include <glm/glm.hpp>

class Cone
{
public:
    Cone();

    // Method to generate cone data and store it in a float vector (for VBO)
    void generateConeData(int heightSegments, int radialSegments, std::vector<float> &vbo);

private:
    // Helper methods
    void pushVertexWithNormal(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &uv, std::vector<float>& vbo);
    void makeWedge(float currentTheta, float nextTheta, int heightSegments, std::vector<float> &vbo);
    void makeCap(int radialSegments, std::vector<float> &vbo);
};

#endif // CONE_H
