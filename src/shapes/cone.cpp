#include "cone.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Constructor
Cone::Cone() {}

// Push position and normal vector into the VBO vector
void Cone::pushVertexWithNormal(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &uv, std::vector<float>& vbo) {
    vbo.push_back(position.x);
    vbo.push_back(position.y);
    vbo.push_back(position.z);
    vbo.push_back(normal.x);
    vbo.push_back(normal.y);
    vbo.push_back(normal.z);
    vbo.push_back(uv.x); // Add U coordinate
    vbo.push_back(uv.y); // Add V coordinate
}


void Cone::makeWedge(float currentTheta, float nextTheta, int heightSegments, std::vector<float> &vbo) {
    float heightStep = 1.0f / heightSegments;
    float radiusStep = 0.5f / heightSegments;
    float slope = 0.5f;  // Calculated based on radius / height

    glm::vec3 apex(0.0f, 0.5f, 0.0f);  // Apex of the cone

    // Precompute normals for the wedge
    glm::vec3 normalLeft = glm::normalize(glm::vec3(cos(currentTheta), slope, -sin(currentTheta)));
    glm::vec3 normalRight = glm::normalize(glm::vec3(cos(nextTheta), slope, -sin(nextTheta)));
    glm::vec3 coneTipNormal = glm::normalize(normalLeft + normalRight); // Step 1: Average and normalize

    coneTipNormal.y = 0; // Step 2: Set y component to 0
    coneTipNormal = glm::normalize(coneTipNormal); // Step 3: Normalize again

    coneTipNormal.y = 0.5f; // Step 4: Set y component to 0.5
    coneTipNormal = glm::normalize(coneTipNormal); // Step 5: Normalize again

    for (int i = 0; i < heightSegments; ++i) {
        float currentHeight = i * heightStep - 0.5f;
        float nextHeight = (i + 1) * heightStep - 0.5f;

        float currentRadius = 0.5f - i * radiusStep;
        float nextRadius = 0.5f - (i + 1) * radiusStep;

        glm::vec3 bottomLeft(currentRadius * cos(currentTheta), currentHeight, -currentRadius * sin(currentTheta));
        glm::vec3 bottomRight(currentRadius * cos(nextTheta), currentHeight, -currentRadius * sin(nextTheta));
        glm::vec3 topLeft(nextRadius * cos(currentTheta), nextHeight, -nextRadius * sin(currentTheta));
        glm::vec3 topRight(nextRadius * cos(nextTheta), nextHeight, -nextRadius * sin(nextTheta));

        // Calculate UV coordinates
        float uLeft = currentTheta / (2.0f * glm::pi<float>());
        float uRight = nextTheta / (2.0f * glm::pi<float>());
        float vBottom = (currentHeight + 0.5f);
        float vTop = (nextHeight + 0.5f);

        if (nextHeight >= 0.5f) {  // Reached the apex
            glm::vec2 uvApex(0.5f, 1.0f); // UV for the cone tip
            glm::vec2 uvBottomLeft(uLeft, vBottom);
            glm::vec2 uvBottomRight(uRight, vBottom);

            pushVertexWithNormal(apex, coneTipNormal, uvApex, vbo);
            pushVertexWithNormal(bottomLeft, normalLeft, uvBottomLeft, vbo);
            pushVertexWithNormal(bottomRight, normalRight, uvBottomRight, vbo);
        } else {
            glm::vec2 uvBottomLeft(uLeft, vBottom);
            glm::vec2 uvBottomRight(uRight, vBottom);
            glm::vec2 uvTopLeft(uLeft, vTop);
            glm::vec2 uvTopRight(uRight, vTop);

            // First triangle
            pushVertexWithNormal(topLeft, normalLeft, uvTopLeft, vbo);
            pushVertexWithNormal(bottomLeft, normalLeft, uvBottomLeft, vbo);
            pushVertexWithNormal(bottomRight, normalRight, uvBottomRight, vbo);

            // Second triangle
            pushVertexWithNormal(topLeft, normalLeft, uvTopLeft, vbo);
            pushVertexWithNormal(bottomRight, normalRight, uvBottomRight, vbo);
            pushVertexWithNormal(topRight, normalRight, uvTopRight, vbo);
        }
    }
}


// Generate the base cap of the cone
void Cone::makeCap(int radialSegments, std::vector<float> &vbo) {
    float thetaStep = glm::radians(360.f / radialSegments);
    float y = -0.5f;  // Bottom y-coordinate
    glm::vec3 normal(0.0f, -1.0f, 0.0f);

    glm::vec3 center(0.0f, y, 0.0f);  // Center point of the base
    glm::vec2 uvCenter(0.5f, 0.5f);  // UV center of the base

    for (int i = 0; i < radialSegments; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;

        glm::vec3 edge1(0.5f * cos(currentTheta), y, 0.5f * sin(currentTheta));
        glm::vec3 edge2(0.5f * cos(nextTheta), y, 0.5f * sin(nextTheta));

        // UV coordinates
        glm::vec2 uvEdge1(0.5f + 0.5f * cos(currentTheta), 0.5f + 0.5f * sin(currentTheta));
        glm::vec2 uvEdge2(0.5f + 0.5f * cos(nextTheta), 0.5f + 0.5f * sin(nextTheta));

        // Create triangles for the base
        pushVertexWithNormal(center, normal, uvCenter, vbo);
        pushVertexWithNormal(edge1, normal, uvEdge1, vbo);
        pushVertexWithNormal(edge2, normal, uvEdge2, vbo);
    }
}

// Main function to generate all cone data
void Cone::generateConeData(int heightSegments, int radialSegments, std::vector<float> &vbo) {
    vbo.clear();
    vbo.reserve(heightSegments * radialSegments * 6 * 6 + radialSegments * 3 * 6);

    if(radialSegments < 3){
        radialSegments = 3;
    }

    // Generate the cone's sides
    float thetaStep = glm::radians(360.f / radialSegments);
    for (int i = 0; i < radialSegments; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(currentTheta, nextTheta, heightSegments, vbo);
    }

    // Generate the base cap
    makeCap(radialSegments, vbo);
}

