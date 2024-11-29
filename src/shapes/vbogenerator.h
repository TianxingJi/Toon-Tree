#ifndef VBOGENERATOR_H
#define VBOGENERATOR_H

#include <vector>
#include "utils/scenedata.h"
#include "shapes/cube.h"
#include "shapes/cone.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include <iostream>

// Inline definition to avoid duplicate symbols
inline void generateVBOBasedOnType(int phiTesselations, int thetaTesselations, std::vector<float>& vbo, PrimitiveType type) {
    vbo.clear();

    switch (type) {
    case PrimitiveType::PRIMITIVE_CUBE:
        Cube().generateCubeData(phiTesselations, vbo);
        break;
    case PrimitiveType::PRIMITIVE_CONE:
        Cone().generateConeData(phiTesselations, thetaTesselations, vbo);
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        Cylinder().generateCylinderData(phiTesselations, thetaTesselations, vbo);
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        Sphere().generateSphereData(phiTesselations, thetaTesselations, vbo);
        break;
    default:
        std::cerr << "Unknown primitive type!" << std::endl;
        break;
    }
}

#endif // VBOGENERATOR_H
