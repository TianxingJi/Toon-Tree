#include "cylinder.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Constructor
Cylinder::Cylinder() {}

// Push position, normal, and UV into the VBO vector
void Cylinder::pushVertexWithNormalAndUV(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &uv, std::vector<float>& vbo) {
    vbo.push_back(position.x);
    vbo.push_back(position.y);
    vbo.push_back(position.z);
    vbo.push_back(normal.x);
    vbo.push_back(normal.y);
    vbo.push_back(normal.z);
    vbo.push_back(uv.x);
    vbo.push_back(uv.y);
}

// Generate the cylinder body with vertical and radial subdivisions
void Cylinder::makeWedge(float currentTheta, float nextTheta, int heightSegments, std::vector<float> &vbo) {
    float heightStep = 1.0f / heightSegments;

    for (int i = 0; i < heightSegments; ++i) {
        float currentHeight = i * heightStep - 0.5f;
        float nextHeight = (i + 1) * heightStep - 0.5f;

        glm::vec3 bottomLeft(0.5f * cos(currentTheta), currentHeight, -0.5f * sin(currentTheta));
        glm::vec3 bottomRight(0.5f * cos(nextTheta), currentHeight, -0.5f * sin(nextTheta));
        glm::vec3 topLeft(0.5f * cos(currentTheta), nextHeight, -0.5f * sin(currentTheta));
        glm::vec3 topRight(0.5f * cos(nextTheta), nextHeight, -0.5f * sin(nextTheta));

        glm::vec3 normalLeft = glm::normalize(glm::vec3(cos(currentTheta), 0.0f, -sin(currentTheta)));
        glm::vec3 normalRight = glm::normalize(glm::vec3(cos(nextTheta), 0.0f, -sin(nextTheta)));

        // Compute UV coordinates
        glm::vec2 uvBottomLeft(currentTheta / (2.0f * glm::pi<float>()), (currentHeight + 0.5f));
        glm::vec2 uvBottomRight(nextTheta / (2.0f * glm::pi<float>()), (currentHeight + 0.5f));
        glm::vec2 uvTopLeft(currentTheta / (2.0f * glm::pi<float>()), (nextHeight + 0.5f));
        glm::vec2 uvTopRight(nextTheta / (2.0f * glm::pi<float>()), (nextHeight + 0.5f));

        // Triangle 1
        pushVertexWithNormalAndUV(topLeft, normalLeft, uvTopLeft, vbo);
        pushVertexWithNormalAndUV(bottomLeft, normalLeft, uvBottomLeft, vbo);
        pushVertexWithNormalAndUV(bottomRight, normalRight, uvBottomRight, vbo);

        // Triangle 2
        pushVertexWithNormalAndUV(topLeft, normalLeft, uvTopLeft, vbo);
        pushVertexWithNormalAndUV(bottomRight, normalRight, uvBottomRight, vbo);
        pushVertexWithNormalAndUV(topRight, normalRight, uvTopRight, vbo);
    }
}

// Generate top or bottom cap of the cylinder with radial subdivisions
void Cylinder::makeCap(bool isTop, int radialSegments, int heightSegments, std::vector<float> &vbo) {
    float thetaStep = glm::radians(360.f / radialSegments);
    float y = isTop ? 0.5f : -0.5f;
    glm::vec3 normal = isTop ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, -1.0f, 0.0f);

    // Loop for radial segments from center to outer edge
    for (int j = 0; j < heightSegments; ++j) {
        float innerRadius = j * (0.5f / heightSegments);
        float outerRadius = (j + 1) * (0.5f / heightSegments);

        for (int i = 0; i < radialSegments; ++i) {
            float currentTheta = i * thetaStep;
            float nextTheta = (i + 1) * thetaStep;

            glm::vec3 innerEdge1(innerRadius * cos(currentTheta), y, innerRadius * sin(currentTheta));
            glm::vec3 innerEdge2(innerRadius * cos(nextTheta), y, innerRadius * sin(nextTheta));
            glm::vec3 outerEdge1(outerRadius * cos(currentTheta), y, outerRadius * sin(currentTheta));
            glm::vec3 outerEdge2(outerRadius * cos(nextTheta), y, outerRadius * sin(nextTheta));

            // Compute UV coordinates for the cap
            glm::vec2 uvInnerEdge1(0.5f + innerRadius * cos(currentTheta), 0.5f + (isTop ? -innerRadius : innerRadius) * sin(currentTheta));
            glm::vec2 uvInnerEdge2(0.5f + innerRadius * cos(nextTheta), 0.5f + (isTop ? -innerRadius : innerRadius) * sin(nextTheta));
            glm::vec2 uvOuterEdge1(0.5f + outerRadius * cos(currentTheta), 0.5f + (isTop ? -outerRadius : outerRadius) * sin(currentTheta));
            glm::vec2 uvOuterEdge2(0.5f + outerRadius * cos(nextTheta), 0.5f + (isTop ? -outerRadius : outerRadius) * sin(nextTheta));

            // Create triangles for cap
            if (isTop) {
                pushVertexWithNormalAndUV(innerEdge1, normal, uvInnerEdge1, vbo);
                pushVertexWithNormalAndUV(outerEdge2, normal, uvOuterEdge2, vbo);
                pushVertexWithNormalAndUV(outerEdge1, normal, uvOuterEdge1, vbo);

                pushVertexWithNormalAndUV(innerEdge1, normal, uvInnerEdge1, vbo);
                pushVertexWithNormalAndUV(innerEdge2, normal, uvInnerEdge2, vbo);
                pushVertexWithNormalAndUV(outerEdge2, normal, uvOuterEdge2, vbo);
            } else {
                pushVertexWithNormalAndUV(innerEdge1, normal, uvInnerEdge1, vbo);
                pushVertexWithNormalAndUV(outerEdge1, normal, uvOuterEdge1, vbo);
                pushVertexWithNormalAndUV(outerEdge2, normal, uvOuterEdge2, vbo);

                pushVertexWithNormalAndUV(innerEdge1, normal, uvInnerEdge1, vbo);
                pushVertexWithNormalAndUV(outerEdge2, normal, uvOuterEdge2, vbo);
                pushVertexWithNormalAndUV(innerEdge2, normal, uvInnerEdge2, vbo);
            }
        }
    }
}

// Main function to generate all cylinder data
void Cylinder::generateCylinderData(int heightSegments, int radialSegments, std::vector<float> &vbo) {
    vbo.clear();
    vbo.reserve(heightSegments * radialSegments * 6 * 6 + 2 * heightSegments * radialSegments * 6 * 6);

    if(radialSegments < 3){
        radialSegments = 3;
    }

    // Generate top cap
    makeCap(true, radialSegments, heightSegments, vbo);

    // Generate cylinder body
    float thetaStep = glm::radians(360.f / radialSegments);
    for (int i = 0; i < radialSegments; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(currentTheta, nextTheta, heightSegments, vbo);
    }

    // Generate bottom cap
    makeCap(false, radialSegments, heightSegments, vbo);
}
