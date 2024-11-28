#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "camera/camera.h"
#include "utils/sceneparser.h"

class sceneloader
{
public:
    sceneloader();

    void setSceneLoader(int width, int height, const RenderData &metaData);

    // The getter of the width of the scene
    const int& width() const;

    // The getter of the height of the scene
    const int& height() const;

    // The getter of the global data of the scene
    const SceneGlobalData& getGlobalData() const;

    // The getter of the shared pointer to the camera instance of the scene
    const camera& getCamera() const;

    const glm::mat4& getViewMatrix() const;

    const glm::mat4& getInverseViewMatrix() const;

    const glm::mat4& getProjMatrix() const;

    const glm::mat4& getInverseProjMatrix() const;

    void updateViewMatrix(const glm::vec3& moveDirection);

    void updateViewMatrixRotation(const glm::vec4& newLook, const glm::vec4& newUp);

    void updateProjMatrixResize(int w, int h);

    void updateProjMatrixParam();

    const std::vector<SceneLightData>& getLights() const;

    const std::vector<RenderShapeData>& getShapes() const;

    glm::mat4 viewMatrix;
    glm::mat4 inverseViewMatrix;
    glm::mat4 projMatrix;
    glm::mat4 inverseProjMatrix;
    int canvasWidth;
    int canvasHeight;
    SceneGlobalData sceneGlobalData;
    camera camera;

    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;
};

#endif // SCENELOADER_H
