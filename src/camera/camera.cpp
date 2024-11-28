#include "camera.h"


glm::mat4 camera::getViewMatrix() const {
    // Optional TODO: implement the getter or make your own design
    // Step 1: Convert vec4 to vec3 for position, look, and up (ignore the w component)
    glm::vec3 pos3 = glm::vec3(pos);
    glm::vec3 look3 = glm::vec3(look);
    glm::vec3 up3 = glm::vec3(up);

    // Step 2: Calculate the w axis (camera forward direction, opposite of look)
    glm::vec3 w = glm::normalize(-look3);  // w is the inverse of the look direction

    // Step 3: Calculate the v axis (up direction orthogonalized to w)
    glm::vec3 v = glm::normalize(up3 - glm::dot(up3, w) * w);  // Orthogonalizing up against w

    // Step 4: Calculate the u axis (camera right direction)
    glm::vec3 u = glm::cross(v, w);  // u is the cross product of v and w (right direction)

    // Step 5: Build the rotation matrix using u, v, and w
    glm::mat4 rotate = glm::mat4(1.0f);  // Initialize to identity matrix
    rotate[0][0] = u.x; rotate[1][0] = u.y; rotate[2][0] = u.z;
    rotate[0][1] = v.x; rotate[1][1] = v.y; rotate[2][1] = v.z;
    rotate[0][2] = w.x; rotate[1][2] = w.y; rotate[2][2] = w.z;

    // Step 6: Build the translation matrix to move the camera position to the origin
    glm::mat4 translate = glm::mat4(1.0f);
    translate[3][0] = -pos3.x;
    translate[3][1] = -pos3.y;
    translate[3][2] = -pos3.z;

    // Step 7: Combine rotation and translation matrices to get the final view matrix
    glm::mat4 result = rotate * translate;

    return result;  // Return the calculated view matrix
}

glm::mat4 camera::getInverseViewMatrix(glm::mat4 viewMatrix) const{
    return glm::inverse(viewMatrix);
}

glm::mat4 camera::getProjMatrix(){
    nearPlane = settings.nearPlane;
    farPlane = settings.farPlane;

    glm::mat4 M1 = glm::mat4(1.0f);
    M1[2][2] = -2.0f;
    M1[3][2] = -1.0f;

    float c = -nearPlane / farPlane;
    glm::mat4 M2 = glm::mat4(1.0f);
    M2[2][2] = 1.0f / (1.0f + c);
    M2[3][2] = -c / (1.0f + c);
    M2[2][3] = -1.0f;
    M2[3][3] = 0.0f;

    float tanHalfWidthAngle = tan(widthAngle / 2.0f);
    float tanHalfHeightAngle = tan(heightAngle / 2.0f);
    glm::mat4 M3 = glm::mat4(1.0f);
    M3[0][0] = 1.0f / (farPlane * tanHalfWidthAngle);
    M3[1][1] = 1.0f / (farPlane * tanHalfHeightAngle);
    M3[2][2] = 1.0f / farPlane;

    glm::mat4 projMatrix = M1 * M2 * M3;

    return projMatrix;
}

glm::vec4 camera::getSide(){
    glm::vec3 look3 = glm::vec3(look);
    glm::vec3 up3 = glm::vec3(up);

    // Calculate the side vector (right vector)
    glm::vec3 side3 = glm::normalize(glm::cross(look3, up3));

    // Store side3 as a vec4 with w = 0.0, since it is a direction vector
    glm::vec4 side4 = glm::vec4(side3, 0.0f);

    return side4;
}

glm::mat4 camera::getInverseProjMatrix(glm::mat4 projMatrix) const{
    return glm::inverse(projMatrix);
}

float camera::getAspectRatio(float width, float height) const {
    return width / height;
}

float camera::getHeightAngle() const {
    return heightAngle;
}

float camera::getFocalLength() const {
    return focalLength;
}

float camera::getAperture() const {
    return aperture;
}

float camera::getPlaneDepth() const{
    return planeDepth;
}

glm::vec3 camera::getRight() const {
    return u;
}
