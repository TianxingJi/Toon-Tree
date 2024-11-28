#include "cube.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Constructor
Cube::Cube() {}

// Helper function to push a vertex position and normal into the VBO vector
void Cube::pushVertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &uv, std::vector<float> &vbo) {
    vbo.push_back(position.x);
    vbo.push_back(position.y);
    vbo.push_back(position.z);
    vbo.push_back(normal.x);
    vbo.push_back(normal.y);
    vbo.push_back(normal.z);
    vbo.push_back(uv.x);
    vbo.push_back(uv.y);
}


// Generate cube data and store it directly in the VBO vector
void Cube::generateCubeData(int phi, std::vector<float> &vbo) {
    vbo.clear();
    vbo.reserve(6 * phi * phi * 6 * 3); // Reserve space based on subdivisions

    // Front face (z = 0.5)
    makeFace(glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             phi, vbo);

    // Back face (z = -0.5), adjust the order of vertices
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             phi, vbo);

    // Left face (x = -0.5)
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f,  0.5f),
             phi, vbo);

    // Right face (x = 0.5)
    makeFace(glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             phi, vbo);

    // Top face (y = 0.5)
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f,  0.5f),
             phi, vbo);

    // Bottom face (y = -0.5)
    makeFace(glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             phi, vbo);
}


void Cube::makeTile(const glm::vec3 &topLeft, const glm::vec3 &topRight,
                    const glm::vec3 &bottomLeft, const glm::vec3 &bottomRight,
                    const glm::vec2 &uvTopLeft, const glm::vec2 &uvTopRight,
                    const glm::vec2 &uvBottomLeft, const glm::vec2 &uvBottomRight,
                    std::vector<float> &vbo) {
    glm::vec3 normal = glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft));

    // Triangle 1
    pushVertex(topLeft, normal, uvTopLeft, vbo);
    pushVertex(bottomLeft, normal, uvBottomLeft, vbo);
    pushVertex(bottomRight, normal, uvBottomRight, vbo);

    // Triangle 2
    pushVertex(topLeft, normal, uvTopLeft, vbo);
    pushVertex(bottomRight, normal, uvBottomRight, vbo);
    pushVertex(topRight, normal, uvTopRight, vbo);
}


// Subdivide a face based on param1 and add tiles to VBO
void Cube::makeFace(const glm::vec3 &topLeft, const glm::vec3 &topRight,
                    const glm::vec3 &bottomLeft, const glm::vec3 &bottomRight,
                    int param1, std::vector<float> &vbo) {
    float step = 1.0f / param1;

    for (int i = 0; i < param1; ++i) {
        for (int j = 0; j < param1; ++j) {
            glm::vec3 topLerpLeft = glm::mix(topLeft, bottomLeft, i * step);
            glm::vec3 topLerpRight = glm::mix(topRight, bottomRight, i * step);
            glm::vec3 nextTopLerpLeft = glm::mix(topLeft, bottomLeft, (i + 1) * step);
            glm::vec3 nextTopLerpRight = glm::mix(topRight, bottomRight, (i + 1) * step);

            glm::vec3 tileTopLeft = glm::mix(topLerpLeft, topLerpRight, j * step);
            glm::vec3 tileTopRight = glm::mix(topLerpLeft, topLerpRight, (j + 1) * step);
            glm::vec3 tileBottomLeft = glm::mix(nextTopLerpLeft, nextTopLerpRight, j * step);
            glm::vec3 tileBottomRight = glm::mix(nextTopLerpLeft, nextTopLerpRight, (j + 1) * step);

            glm::vec2 uvTopLeft = glm::vec2(j * step, 1.0f - i * step);
            glm::vec2 uvTopRight = glm::vec2((j + 1) * step, 1.0f - i * step);
            glm::vec2 uvBottomLeft = glm::vec2(j * step, 1.0f - (i + 1) * step);
            glm::vec2 uvBottomRight = glm::vec2((j + 1) * step, 1.0f - (i + 1) * step);

            makeTile(tileTopLeft, tileTopRight, tileBottomLeft, tileBottomRight,
                     uvTopLeft, uvTopRight, uvBottomLeft, uvBottomRight, vbo);
        }
    }
}

