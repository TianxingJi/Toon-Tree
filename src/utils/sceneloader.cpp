#include "sceneloader.h"

sceneloader::sceneloader()
    : canvasWidth(0), canvasHeight(0) {
}

void sceneloader::setSceneLoader(int width, int height, const RenderData &metaData) {
    // Record camera data
    camera.pos = metaData.cameraData.pos;
    camera.look = metaData.cameraData.look;
    camera.up = metaData.cameraData.up;
    camera.side = camera.getSide();
    camera.heightAngle = metaData.cameraData.heightAngle;
    camera.aperture = metaData.cameraData.aperture;
    camera.focalLength = metaData.cameraData.focalLength;

    // Compute camera basis vectors
    glm::vec3 w = glm::normalize(-glm::vec3(metaData.cameraData.look));
    glm::vec3 v = glm::normalize(glm::vec3(metaData.cameraData.up) - glm::dot(glm::vec3(metaData.cameraData.up), w) * w);
    glm::vec3 u = glm::cross(v, w);

    camera.u = u;
    camera.v = v;
    camera.w = w;

    float aspectRatio = camera.getAspectRatio(width, height);
    camera.widthAngle = 2.0f * atan(aspectRatio * tan(camera.heightAngle / 2.0f));

    // Record the view matrix and the inverse one
    viewMatrix = camera.getViewMatrix();
    inverseViewMatrix = camera.getInverseViewMatrix(viewMatrix);

    // Record the proj matrix and the inverse one
    projMatrix = camera.getProjMatrix();
    inverseProjMatrix = camera.getInverseProjMatrix(projMatrix);

    // Record the width and height
    canvasWidth = width;
    canvasHeight = height;

    // Record scene global data
    sceneGlobalData = metaData.globalData;

    // Record lights and shapes data
    lights = metaData.lights;
    shapes = metaData.shapes;
}


const int& sceneloader::width() const {
    return canvasWidth;
}

const int& sceneloader::height() const {
    return canvasHeight;
}

const SceneGlobalData& sceneloader::getGlobalData() const {
    return sceneGlobalData;
}

const camera& sceneloader::getCamera() const {
    return camera;
}

const glm::mat4& sceneloader::getViewMatrix() const {
    return viewMatrix;
}

const glm::mat4& sceneloader::getInverseViewMatrix() const {
    return inverseViewMatrix;
}

// View Matrix should be updated when the camera position changes
void sceneloader::updateViewMatrix(const glm::vec3& moveDirection) {
    camera.pos += glm::vec4(moveDirection, 0.0f);
    viewMatrix = camera.getViewMatrix();
}

void sceneloader::updateViewMatrixRotation(const glm::vec4& newLook, const glm::vec4& newUp) {
    // Update the camera's look and up vectors
    camera.look = glm::normalize(newLook); // Ensure the look vector is normalized
    camera.up = glm::normalize(newUp);     // Ensure the up vector is normalized
    camera.side = camera.getSide();
    viewMatrix = camera.getViewMatrix();
}

void sceneloader::updateProjMatrixResize(int w, int h) {
    float aspectRatio = camera.getAspectRatio(w, h);
    camera.widthAngle = 2.0f * atan(aspectRatio * tan(camera.heightAngle / 2.0f));
    projMatrix = camera.getProjMatrix();
    // inverseProjMatrix = camera.getInverseProjMatrix(projMatrix);
}

void sceneloader::updateProjMatrixParam() {
    projMatrix = camera.getProjMatrix();
    // inverseProjMatrix = camera.getInverseProjMatrix(projMatrix);
}


const glm::mat4& sceneloader::getProjMatrix() const {
    return projMatrix;
}

const glm::mat4& sceneloader::getInverseProjMatrix() const {
    return inverseProjMatrix;
}

const std::vector<SceneLightData>& sceneloader::getLights() const {
    return lights;
}

const std::vector<RenderShapeData>& sceneloader::getShapes() const {
    return shapes;
}
