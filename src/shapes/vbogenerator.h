#ifndef VBOGENERATOR_H
#define VBOGENERATOR_H

#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "utils/scenedata.h"
#include <iomanip>
#include <vector>
#include <iostream>

// Function to generate VBO data based on the primitive type
void generateVBOBasedOnType(int phiTesselations, int thetaTesselations, std::vector<float>& vbo, PrimitiveType type) {
    vbo.clear(); // Clear any existing data in the VBO

    switch (type) {
    case PrimitiveType::PRIMITIVE_CUBE: {
        Cube cube;
        cube.generateCubeData(phiTesselations, vbo); // Cube data does not need theta tesselation
        break;
    }
    case PrimitiveType::PRIMITIVE_CONE: {
        Cone cone;
        cone.generateConeData(phiTesselations, thetaTesselations, vbo);
        break;
    }
    case PrimitiveType::PRIMITIVE_CYLINDER: {
        Cylinder cylinder;
        cylinder.generateCylinderData(phiTesselations, thetaTesselations, vbo);
        break;
    }
    case PrimitiveType::PRIMITIVE_SPHERE: {
        Sphere sphere;
        sphere.generateSphereData(phiTesselations, thetaTesselations, vbo);
        break;
    }
    case PrimitiveType::PRIMITIVE_MESH:
        break;
    default:
        std::cerr << "Unknown primitive type!" << std::endl;
        break;
    }

    // // Debug: Print out all generated vertices
    // std::cout << std::fixed << std::setprecision(3); // Set precision for floating-point numbers
    // std::cout << "Generated vertices for " << static_cast<int>(type) << ":\n";
    // for (size_t i = 0; i < vbo.size(); i += 6) {
    //     std::cout << "Position: (" << vbo[i] << ", " << vbo[i + 1] << ", " << vbo[i + 2] << "), "
    //               << "Normal: (" << vbo[i + 3] << ", " << vbo[i + 4] << ", " << vbo[i + 5] << ")\n";
    // }
}

#endif // VBOGENERATOR_H
