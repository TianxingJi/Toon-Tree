#include "realtime.h"
#include "shapes/vbogenerator.h"
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Helper Function to take rotate
glm::mat4 Realtime::customRotate(const glm::vec3& axis, float radians) {
    glm::vec3 normalizedAxis = glm::normalize(axis);

    float x = normalizedAxis.x;
    float y = normalizedAxis.y;
    float z = normalizedAxis.z;

    float cosTheta = std::cos(radians);
    float sinTheta = std::sin(radians);
    float oneMinusCos = 1.0f - cosTheta;

    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix[0][0] = cosTheta + x * x * oneMinusCos;
    rotationMatrix[0][1] = x * y * oneMinusCos - z * sinTheta;
    rotationMatrix[0][2] = x * z * oneMinusCos + y * sinTheta;

    rotationMatrix[1][0] = x * y * oneMinusCos + z * sinTheta;
    rotationMatrix[1][1] = cosTheta + y * y * oneMinusCos;
    rotationMatrix[1][2] = y * z * oneMinusCos - x * sinTheta;

    rotationMatrix[2][0] = x * z * oneMinusCos - y * sinTheta;
    rotationMatrix[2][1] = y * z * oneMinusCos + x * sinTheta;
    rotationMatrix[2][2] = cosTheta + z * z * oneMinusCos;

    return rotationMatrix;
}

void Realtime::initializeLights() {
    // // Clear existing lights
    // lights.clear();

    // Create a directional light
    CustomLightData directionalLight;
    directionalLight.type = 1; // 1 represents a directional light
    directionalLight.color = glm::vec4(1.f, 1.0f, 1.0f, 1.0f); // White light
    directionalLight.direction = glm::vec4(-3.f, -2.0f, -1.f, 0.0f); // Light direction in world space
    directionalLight.function = glm::vec3(1.0f, 0.0f, 0.0f); // No attenuation (constant light intensity)
    directionalLight.position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Directional lights do not use position

    // Add the light to the lights vector
    lights.push_back(directionalLight);
}

void Realtime::initializeBase() {
    float radius = 4.0f; // Same radius as used for the forest distribution
    float margin = 2.0f; // Add some margin around the circular forest area
    float baseWidth = (radius * 2.0f) + margin; // For radius=4, diameter=8, plus margin=2, total=10

    // Use the same width and depth
    glm::vec3 baseSize = glm::vec3(baseWidth, 0.1f, baseWidth);
    glm::vec3 basePosition = glm::vec3(0.0f, -0.5f - baseSize.y / 2.0f, 0.0f);

    std::vector<GLfloat> baseVertices;
    generateShape(PrimitiveType::PRIMITIVE_CUBE, baseVertices);

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), basePosition) *
                            glm::scale(glm::mat4(1.0f), baseSize);

    createShapeData(
        baseVertices,
        glm::vec4(0.2f, 0.1f, 0.0f, 1.0f), // Dark brown soil color
        glm::vec4(0.3f, 0.2f, 0.1f, 1.0f), // Diffuse color
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), // Specular color
        16.0f,                             // Shininess
        m_ground_texture,
        modelMatrix,
        true
        );
}

void Realtime::interpretLSystem(const std::string& lSystemString, float angle, float length) {
    m_shapeData.clear();
    templateTree.clear();

    // Initialize turtle state and stack
    std::stack<TurtleState> stateStack;
    TurtleState turtle(glm::vec3(0.0f, -0.5f, 0.0f)); // Start at origin with default directions

    for (char c : lSystemString) {
        switch (c) {
        case 'F': { // Root or Trunk
            glm::vec3 newPosition = turtle.position + turtle.growDirection * length;

            std::vector<GLfloat> trunkVertices;
            generateShape(PrimitiveType::PRIMITIVE_CYLINDER, trunkVertices);

            float thickness = 0.08f - 0.01f * turtle.position.y;
            thickness = glm::max(thickness, 0.005f);

            glm::mat4 modelMatrix = calculateModelMatrix(turtle.position, newPosition, thickness);

            createShapeData(
                trunkVertices,
                glm::vec4(0.4f, 0.3f, 0.2f, 1.0f), // Root ambient color
                glm::vec4(0.5f, 0.4f, 0.3f, 1.0f), // Root diffuse color
                glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), // Root specular color
                32.0f,                              // Shininess
                m_trunk_texture,  // Texture
                modelMatrix,                      // Model matrix
                false                             // not base
                );

            turtle.position = newPosition;
            break;
        }
        case 'X': { // Branch
            glm::vec3 newPosition = turtle.position + turtle.growDirection * (length * 0.5f);

            std::vector<GLfloat> branchVertices;
            generateShape(PrimitiveType::PRIMITIVE_CYLINDER, branchVertices);

            float thickness = 0.08f - 0.01f * turtle.position.y;
            thickness = glm::max(thickness, 0.005f);

            glm::mat4 modelMatrix = calculateModelMatrix(turtle.position, newPosition, thickness);

            createShapeData(
                branchVertices,
                glm::vec4(0.4f, 0.3f, 0.2f, 1.0f), // Branch ambient color
                glm::vec4(0.5f, 0.4f, 0.3f, 1.0f), // Branch diffuse color
                glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), // Branch specular color
                32.0f,                              // Shininess
                m_branch_texture,  // Texture
                modelMatrix,                      // Model matrix
                false                             // not base
                );

            turtle.position = newPosition;
            break;
        }
        case 'L': { // Create a leaf
            glm::vec3 newPosition = turtle.position + turtle.growDirection * (length * 0.5f);

            std::vector<GLfloat> leafVertices;
            generateShape(PrimitiveType::PRIMITIVE_CUBE, leafVertices); // Use cone as leaf

            float thickness = 0.05f - 0.001f * turtle.position.y;
            thickness = glm::max(thickness, 0.005f);

            glm::mat4 modelMatrix = calculateModelMatrix(turtle.position, newPosition, thickness);

            createShapeData(
                leafVertices,
                glm::vec4(0.0f, 0.8f, 0.0f, 1.0f), // Leaf ambient color
                glm::vec4(0.1f, 0.9f, 0.1f, 1.0f), // Leaf diffuse color
                glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // Leaf specular color
                16.0f,                              // Shininess
                m_leaf_texture,  // Leaf texture
                modelMatrix,                      // Model matrix
                false                             // not base
                );

            turtle.position = newPosition;
            break;
        }
        case '+': { // Rotate GrowDirection left/right (Yaw)
            glm::mat4 rotationMatrix = customRotate(turtle.forwardDirection, glm::radians(angle));
            turtle.growDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.growDirection, 0.0f)));
            turtle.rightDirection = glm::normalize(glm::cross(turtle.growDirection, turtle.forwardDirection));
            break;
        }
        case '-': { // Rotate GrowDirection right/left (Yaw, opposite)
            glm::mat4 rotationMatrix = customRotate(turtle.forwardDirection, glm::radians(-angle));
            turtle.growDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.growDirection, 0.0f)));
            turtle.rightDirection = glm::normalize(glm::cross(turtle.growDirection, turtle.forwardDirection));
            break;
        }
        case '&': { // Roll GrowDirection clockwise (Roll)
            glm::mat4 rotationMatrix = customRotate(turtle.growDirection, glm::radians(-angle));
            turtle.forwardDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.forwardDirection, 0.0f)));
            turtle.rightDirection = glm::normalize(glm::cross(turtle.growDirection, turtle.forwardDirection));
            break;
        }
        case '^': { // Roll GrowDirection counter-clockwise (Roll, opposite)
            glm::mat4 rotationMatrix = customRotate(turtle.growDirection, glm::radians(angle));
            turtle.forwardDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.forwardDirection, 0.0f)));
            turtle.rightDirection = glm::normalize(glm::cross(turtle.growDirection, turtle.forwardDirection));
            break;
        }
        case '<': { // Rotate GrowDirection forward/backward (Pitch)
            glm::mat4 rotationMatrix = customRotate(turtle.rightDirection, glm::radians(-angle));
            turtle.growDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.growDirection, 0.0f)));
            turtle.forwardDirection = glm::normalize(glm::cross(turtle.rightDirection, turtle.growDirection));
            break;
        }
        case '>': { // Rotate GrowDirection backward/forward (Pitch, opposite)
            glm::mat4 rotationMatrix = customRotate(turtle.rightDirection, glm::radians(angle));
            turtle.growDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.growDirection, 0.0f)));
            turtle.forwardDirection = glm::normalize(glm::cross(turtle.rightDirection, turtle.growDirection));
            break;
        }
        case '|': { // Turn around (Rotate 180 degrees)
            glm::mat4 rotationMatrix = customRotate(turtle.growDirection, glm::radians(180.0f));
            turtle.forwardDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.forwardDirection, 0.0f)));
            turtle.rightDirection = glm::normalize(glm::cross(turtle.growDirection, turtle.forwardDirection));
            break;
        }
        case '[': { // Save current state
            stateStack.push(turtle);
            break;
        }
        case ']': { // Restore saved state
            if (!stateStack.empty()) {
                turtle = stateStack.top();
                stateStack.pop();
            }
            break;
        }
        default:
            break;
        }
    }

    // Form Forest
    if(settings.extraCredit2){

        int numTrees = 6;
        float radius = 4.0f;
        float angleStep = 360.0f / numTrees;
        initializeBase();

        for (int i = 0; i < numTrees; i++) {
            float angleDegrees = angleStep * i;
            float angleRadians = glm::radians(angleDegrees);

            float xOffset = radius * std::cos(angleRadians);
            float zOffset = radius * std::sin(angleRadians);

            glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset, 0.0f, zOffset));

            for (const ShapeData &shape : templateTree) {
                ShapeData newShape = shape;
                newShape.modelMatrix = translation * newShape.modelMatrix;
                m_shapeData.push_back(newShape);
            }
        }

    } else {
        m_shapeData = templateTree;
        initializeBase();
    }
}

void Realtime::generateShape(PrimitiveType type, std::vector<GLfloat> &vertices) {
    vertices.clear(); // Clear any previous vertices
    int phiTess = 12; // Number of slices
    int thetaTess = 12; // Number of stacks

    // Generate base shape data (object space)
    generateVBOBasedOnType(phiTess, thetaTess, vertices, type);
}

glm::mat4 Realtime::calculateModelMatrix(const glm::vec3 &start, const glm::vec3 &end, float thickness) {
    glm::vec3 direction = glm::normalize(end - start);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    // If the direction is parallel to `up`, use an alternative vector
    if (glm::length(glm::cross(direction, up)) < 0.001f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    // Get the right direction (new z-axis)
    glm::vec3 right = glm::normalize(glm::cross(up, direction));

    // Update the up vector (new x-axis)
    up = glm::normalize(glm::cross(direction, right));

    // Generate rotation matrix
    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix[0] = glm::vec4(up, 0.0f);        // New x-axis
    rotationMatrix[1] = glm::vec4(direction, 0.0f); // New y-axis
    rotationMatrix[2] = glm::vec4(right, 0.0f);     // New z-axis

    // Generate scaling matrix: scale along the y-axis based on length, and x/z based on thickness
    float scaleLength = glm::length(end - start);
    glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(thickness, scaleLength, thickness));

    // Generate translation matrix: align the base to the `start` point
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), start + 0.5f * scaleLength * direction);

    // Final `modelMatrix`: scale first, then rotate, and finally translate
    return translationMatrix * rotationMatrix * scalingMatrix;
}

void Realtime::createShapeData(
    const std::vector<GLfloat>& vertices,
    const glm::vec4& ambientColor,
    const glm::vec4& diffuseColor,
    const glm::vec4& specularColor,
    float shininess,
    const GLuint& texture,
    const glm::mat4& modelMatrix,
    bool isBase,
    float blend,
    float repeatU,
    float repeatV
    ) {
    ShapeData shapeData;

    // Generate VAO and VBO
    glGenVertexArrays(1, &shapeData.vao);
    glGenBuffers(1, &shapeData.vbo);

    glBindVertexArray(shapeData.vao);
    glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // Define vertex attributes
    glEnableVertexAttribArray(0); // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1); // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2); // UV coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    shapeData.vertexCount = vertices.size() / 8;

    // Set material properties
    shapeData.ambient = ambientColor;
    shapeData.diffuse = diffuseColor;
    shapeData.specular = specularColor;
    shapeData.shininess = shininess;

    // Use provided texture
    if (texture != 0) {
        shapeData.textureUsed = true;
        shapeData.diffuseTexture = texture;
        shapeData.blend = blend;
        shapeData.repeatU = repeatU;
        shapeData.repeatV = repeatV;
    } else {
        shapeData.textureUsed = false;
    }

    shapeData.modelMatrix = modelMatrix;

    // Store in shape data list
    if(isBase){
        m_shapeData.push_back(shapeData);
    } else {
        templateTree.push_back(shapeData);
    }

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::paintLSystem() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);

    // Pass view and projection matrices
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "viewMatrix"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "projMatrix"), 1, GL_FALSE, &m_proj[0][0]);

    // Pass camera position
    glUniform4fv(glGetUniformLocation(m_shader, "cameraPosition"), 1, &glm::vec4(eye, 1.0f)[0]);

    // Set ka, kd, ks coefficients
    float ka = 0.5f;
    float kd = 0.5f;
    float ks = 0.5f;

    glUniform1f(glGetUniformLocation(m_shader, "ka"), ka);
    glUniform1f(glGetUniformLocation(m_shader, "kd"), kd);
    glUniform1f(glGetUniformLocation(m_shader, "ks"), ks);

    // Pass light data
    int numLights = std::min(static_cast<int>(lights.size()), 8); // Max 8 lights
    glUniform1i(glGetUniformLocation(m_shader, "numLights"), numLights);

    for (int i = 0; i < numLights; ++i) {
        const CustomLightData& light = lights[i];
        std::string baseName = "lights[" + std::to_string(i) + "]";

        glUniform4fv(glGetUniformLocation(m_shader, (baseName + ".color").c_str()), 1, &light.color[0]);
        glUniform3fv(glGetUniformLocation(m_shader, (baseName + ".function").c_str()), 1, &light.function[0]);
        glUniform4fv(glGetUniformLocation(m_shader, (baseName + ".position").c_str()), 1, &light.position[0]);
        glUniform4fv(glGetUniformLocation(m_shader, (baseName + ".direction").c_str()), 1, &light.direction[0]);
        glUniform1i(glGetUniformLocation(m_shader, (baseName + ".type").c_str()), light.type);
        glUniform1f(glGetUniformLocation(m_shader, (baseName + ".penumbra").c_str()), light.penumbra);
        glUniform1f(glGetUniformLocation(m_shader, (baseName + ".angle").c_str()), light.angle);
    }

    // Draw L-System geometry (use existing logic)
    for (const ShapeData& shape : m_shapeData) {
        glBindVertexArray(shape.vao);

        // Set material properties
        glUniform4fv(glGetUniformLocation(m_shader, "material.ambient"), 1, &shape.ambient[0]);
        glUniform4fv(glGetUniformLocation(m_shader, "material.diffuse"), 1, &shape.diffuse[0]);
        glUniform4fv(glGetUniformLocation(m_shader, "material.specular"), 1, &shape.specular[0]);
        glUniform1f(glGetUniformLocation(m_shader, "material.shininess"), shape.shininess);

        // Set model matrix
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "modelMatrix"), 1, GL_FALSE, &shape.modelMatrix[0][0]);

        // Set normal matrix
        glm::mat3 normalMatrix = glm::inverse(glm::transpose(glm::mat3(shape.modelMatrix)));
        GLint normalMatrixLocation = glGetUniformLocation(m_shader, "normalMatrix");
        glUniformMatrix3fv(normalMatrixLocation, 1, GL_FALSE, &normalMatrix[0][0]);

        if (shape.textureUsed) {
            // Pass the textureUsed uniform
            GLint textureUsedLocation = glGetUniformLocation(m_shader, "textureUsed");
            glUniform1i(textureUsedLocation, true); // Set to true

            // Pass the texture to shader (already bound to slot 1 as per your setup)
            GLint textureLocation = glGetUniformLocation(m_shader, "Texture");
            glActiveTexture(GL_TEXTURE1); // Set the active texture slot
            glBindTexture(GL_TEXTURE_2D, shape.diffuseTexture);
            glUniform1i(textureLocation, 1); // Slot 1

            // Pass blend value
            GLint blendLocation = glGetUniformLocation(m_shader, "blend");
            glUniform1f(blendLocation, shape.blend);

            // Pass repeatU and repeatV values
            GLint repeatULocation = glGetUniformLocation(m_shader, "repeatU");
            glUniform1f(repeatULocation, shape.repeatU);

            GLint repeatVLocation = glGetUniformLocation(m_shader, "repeatV");
            glUniform1f(repeatVLocation, shape.repeatV);
        }

        glDrawArrays(GL_TRIANGLES, 0, shape.vertexCount);

        glBindVertexArray(0);
    }

    glUseProgram(0);
}
