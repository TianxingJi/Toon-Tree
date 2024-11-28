#include "sphere.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Constructor
Sphere::Sphere() {}

// Convert spherical coordinates to Cartesian
glm::vec4 Sphere::sphericalToCartesian(float phi, float theta) {
    return glm::vec4 (0.5f * glm::cos(theta) * glm::sin(phi), // x
                      0.5f * glm::cos(phi) // y
                     , - 0.5f * glm::sin(theta) * glm::sin(phi) // z
                     , 1.0f); // w
}

// Push the x, y, z components of a vec4, normal, and UV into the VBO vector
void Sphere::pushVertexWithNormalAndUV(glm::vec4 vec, glm::vec2 uv, std::vector<float>& vbo) {
    // Push position components
    vbo.push_back(vec.x);
    vbo.push_back(vec.y);
    vbo.push_back(vec.z);

    // Normalize the position as a vec3 to get the correct normal
    glm::vec3 normal = glm::normalize(glm::vec3(vec.x, vec.y, vec.z));
    vbo.push_back(normal.x);
    vbo.push_back(normal.y);
    vbo.push_back(normal.z);

    // Push UV coordinates
    vbo.push_back(uv.x);
    vbo.push_back(uv.y);
}

// Generate sphere vertices and store them directly in the VBO vector
void Sphere::generateSphereData(int phiTesselations, int thetaTesselations, std::vector<float> &vbo) {
    // Clear and reserve space in the vector to avoid reallocation
    vbo.clear();
    vbo.reserve(phiTesselations * thetaTesselations * 6 * 8); // 6 vertices * 8 attributes (x, y, z, nx, ny, nz, u, v)

    if(phiTesselations < 2){
        phiTesselations = 2;
    }

    if(thetaTesselations < 3){
        thetaTesselations = 3;
    }

    // Generate vertices
    for(int iTheta = 0; iTheta < thetaTesselations; iTheta++) {
        for(int iPhi = 0; iPhi < phiTesselations; iPhi++) {
            // Calculate phi and theta for spherical coordinates
            float phi1 = iPhi / static_cast<float>(phiTesselations) * glm::pi<float>();
            float phi2 = (iPhi + 1) / static_cast<float>(phiTesselations) * glm::pi<float>();
            float the1 = iTheta / static_cast<float>(thetaTesselations) * 2.0f * glm::pi<float>();
            float the2 = (iTheta + 1) / static_cast<float>(thetaTesselations) * 2.0f * glm::pi<float>();

            // Cartesian coordinates
            glm::vec4 p1 = sphericalToCartesian(phi1, the1);
            glm::vec4 p2 = sphericalToCartesian(phi2, the1);
            glm::vec4 p3 = sphericalToCartesian(phi2, the2);
            glm::vec4 p4 = sphericalToCartesian(phi1, the2);

            // UV coordinates
            glm::vec2 uv1 = glm::vec2(the1 / (2.0f * glm::pi<float>()), 1.0f - phi1 / glm::pi<float>());
            glm::vec2 uv2 = glm::vec2(the1 / (2.0f * glm::pi<float>()), 1.0f - phi2 / glm::pi<float>());
            glm::vec2 uv3 = glm::vec2(the2 / (2.0f * glm::pi<float>()), 1.0f - phi2 / glm::pi<float>());
            glm::vec2 uv4 = glm::vec2(the2 / (2.0f * glm::pi<float>()), 1.0f - phi1 / glm::pi<float>());

            // Triangle 1
            pushVertexWithNormalAndUV(p1, uv1, vbo);
            pushVertexWithNormalAndUV(p2, uv2, vbo);
            pushVertexWithNormalAndUV(p3, uv3, vbo);

            // Triangle 2
            pushVertexWithNormalAndUV(p1, uv1, vbo);
            pushVertexWithNormalAndUV(p3, uv3, vbo);
            pushVertexWithNormalAndUV(p4, uv4, vbo);
        }
    }
}
