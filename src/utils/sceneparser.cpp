#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

bool loadOBJ(const std::string& filepath, std::vector<float>& vboData) {
    std::ifstream objFile(filepath);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filepath << std::endl;
        return false;
    }

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::string line;

    bool hasNormals = false;

    while (std::getline(objFile, line)) {
        std::istringstream lineStream(line);
        std::string prefix;
        lineStream >> prefix;

        if (prefix == "v") {
            // Parse vertex position (x, y, z)
            float x, y, z;
            lineStream >> x >> y >> z;
            vertices.push_back(glm::vec3(x, y, z));

        } else if (prefix == "vn") {
            // Parse vertex normal (nx, ny, nz)
            float nx, ny, nz;
            lineStream >> nx >> ny >> nz;
            normals.push_back(glm::vec3(nx, ny, nz));
            hasNormals = true;

        } else if (prefix == "f") {
            std::string v0, v1, v2;
            lineStream >> v0 >> v1 >> v2;

            int v0Index, v1Index, v2Index;
            int vn0Index = -1, vn1Index = -1, vn2Index = -1;

            if (v0.find("//") != std::string::npos) {
                // Face format: vertex//normal
                sscanf(v0.c_str(), "%d//%d", &v0Index, &vn0Index);
                sscanf(v1.c_str(), "%d//%d", &v1Index, &vn1Index);
                sscanf(v2.c_str(), "%d//%d", &v2Index, &vn2Index);
            } else {
                // Face format: vertex only
                sscanf(v0.c_str(), "%d", &v0Index);
                sscanf(v1.c_str(), "%d", &v1Index);
                sscanf(v2.c_str(), "%d", &v2Index);
            }

            // Adjust for 1-based OBJ indexing
            glm::vec3 v0Pos = vertices[v0Index - 1];
            glm::vec3 v1Pos = vertices[v1Index - 1];
            glm::vec3 v2Pos = vertices[v2Index - 1];

            glm::vec3 n0, n1, n2;
            if (hasNormals && vn0Index != -1 && vn1Index != -1 && vn2Index != -1) {
                // Use the normals from the file
                n0 = normals[vn0Index - 1];
                n1 = normals[vn1Index - 1];
                n2 = normals[vn2Index - 1];
            } else {
                // Compute the normal vector for the face if no normals are provided
                glm::vec3 faceNormal = glm::normalize(glm::cross(v1Pos - v0Pos, v2Pos - v0Pos));
                n0 = n1 = n2 = faceNormal;
            }

            // Store vertex positions and normals in vboData
            auto pushVertexData = [&vboData](const glm::vec3& pos, const glm::vec3& norm) {
                vboData.push_back(pos.x);
                vboData.push_back(pos.y);
                vboData.push_back(pos.z);
                vboData.push_back(norm.x);
                vboData.push_back(norm.y);
                vboData.push_back(norm.z);
            };

            // Push data for each vertex of the triangle
            pushVertexData(v0Pos, n0);
            pushVertexData(v1Pos, n1);
            pushVertexData(v2Pos, n2);
        }
    }

    objFile.close();
    return true;
}


void computeRenderShapeData(RenderData &renderData, SceneNode* currentNode, glm::mat4 parentCTM) {
    glm::mat4 currentTransform = glm::mat4(1.0f);

    // check whether there is transformations in current node
    if (!currentNode->transformations.empty()) {
        // Apply all transformations in the current node
        for (SceneTransformation* sceneTransform : currentNode->transformations) {
            glm::mat4 transformationMatrix;
            switch (sceneTransform->type) {
            case TransformationType::TRANSFORMATION_TRANSLATE:
                transformationMatrix = glm::translate(glm::mat4(1.0f), sceneTransform->translate);
                break;
            case TransformationType::TRANSFORMATION_ROTATE:
                transformationMatrix = glm::rotate(glm::mat4(1.0f), sceneTransform->angle, sceneTransform->rotate);
                break;
            case TransformationType::TRANSFORMATION_SCALE:
                transformationMatrix = glm::scale(glm::mat4(1.0f), sceneTransform->scale);
                break;
            case TransformationType::TRANSFORMATION_MATRIX:
                transformationMatrix = sceneTransform->matrix;
                break;
            }
            currentTransform *= transformationMatrix;
        }
    }

    glm::mat4 currentCTM = parentCTM * currentTransform;

    // check whether there is primitives in current node
    if (!currentNode->primitives.empty()) {
        // Handle primitives
        for (ScenePrimitive* scenePrimitive : currentNode->primitives) {
            RenderShapeData currentRenderShapeData(*scenePrimitive, currentCTM);

            if(scenePrimitive -> type == PrimitiveType::PRIMITIVE_MESH){
                 if (!scenePrimitive->meshfile.empty()) {
                      std::vector<float> loadedData;

                       if (loadOBJ(scenePrimitive->meshfile, loadedData)) {
                          currentRenderShapeData.vboData = loadedData;
                       }
                 } else {
                     std::cerr << "Failed to load mesh: " << scenePrimitive->meshfile << std::endl;
                 }
            }

            renderData.shapes.push_back(currentRenderShapeData);
        }
    }

    // check whether there is lights in current node
    if (!currentNode->lights.empty()) {
        for (SceneLight* sceneLight : currentNode->lights) {

            SceneLightData lightData;
            lightData.id = sceneLight->id;
            lightData.type = sceneLight->type;
            lightData.color = sceneLight->color;
            lightData.function = sceneLight->function;

            if (sceneLight->type == LightType::LIGHT_POINT || sceneLight->type == LightType::LIGHT_SPOT) {
                lightData.pos = currentCTM * glm::vec4(0, 0, 0, 1);
            } else {
                lightData.pos = glm::vec4(0, 0, 0, 0);  // Directional Light does not have position
            }

            if (sceneLight->type == LightType::LIGHT_DIRECTIONAL || sceneLight->type == LightType::LIGHT_SPOT) {
                lightData.dir = glm::normalize(currentCTM * glm::vec4(sceneLight->dir));
            } else {
                lightData.dir = glm::vec4(0, 0, 0, 0);  // Point Light does not have direction
            }

            if (sceneLight->type == LightType::LIGHT_SPOT) {
                lightData.angle = sceneLight->angle;
                lightData.penumbra = sceneLight->penumbra;
            }

            renderData.lights.push_back(lightData);
        }
    }

    // check whether there is child nodes in current node
    if (!currentNode->children.empty()) {
        // recursively handle it
        for (SceneNode* childNode : currentNode->children) {
            computeRenderShapeData(renderData, childNode, currentCTM);
        }
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // TODO: Use your Lab 5 code here
    // Task 5: populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    SceneNode* rootNode = fileReader.getRootNode();
    glm::mat4 identityMatrix = glm::mat4(1.0f);


    // Clear existing shapes and lights to avoid accumulating data from previous parses
    renderData.shapes.clear();
    renderData.lights.clear();

    computeRenderShapeData(renderData, rootNode, identityMatrix);

    return true;
}
