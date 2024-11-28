#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <glm/glm.hpp>

class Cube
{
public:
    Cube();

    // Method to generate cube data and store it directly in the VBO vector
    void generateCubeData(int phi, std::vector<float> &vbo);

private:
    // Helper methods
    void pushVertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &uv, std::vector<float> &vbo);
    void makeTile(const glm::vec3 &topLeft, const glm::vec3 &topRight,
                        const glm::vec3 &bottomLeft, const glm::vec3 &bottomRight,
                        const glm::vec2 &uvTopLeft, const glm::vec2 &uvTopRight,
                        const glm::vec2 &uvBottomLeft, const glm::vec2 &uvBottomRight,
                        std::vector<float> &vbo);
    void makeFace(const glm::vec3 &topLeft, const glm::vec3 &topRight,
                  const glm::vec3 &bottomLeft, const glm::vec3 &bottomRight,
                  int param1, std::vector<float> &vbo);
};

#endif // CUBE_H
