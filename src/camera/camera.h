#pragma once

#include "settings.h"
#include <glm/glm.hpp>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class camera {
public:
    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    glm::mat4 getInverseViewMatrix(glm::mat4 viewMatrix) const;

    glm::mat4 getProjMatrix();

    glm::mat4 getInverseProjMatrix(glm::mat4 viewMatrix) const;

    glm::vec4 getSide();

    // Returns the aspect ratio of the camera.
    float getAspectRatio(float width, float height) const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

    float getPlaneDepth() const;

    glm::vec3 getRight() const;

    glm::vec4 pos;
    glm::vec4 look;
    glm::vec4 up;
    glm::vec4 side;
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 w;
    float widthAngle;
    float heightAngle;
    float aperture;
    float focalLength;
    float nearPlane;
    float farPlane;

    float const planeDepth = 1;
};
