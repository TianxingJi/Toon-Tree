#include "realtime.h"
#include "shapes/vbogenerator.h"
#include <iostream>
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

void Realtime::interpretLSystem(const std::string& lSystemString, float angle, float length) {
    m_shapeData.clear();

    // Initialize turtle state and stack
    std::stack<TurtleState> stateStack;
    TurtleState turtle(glm::vec3(0.0f, -0.5f, 0.0f)); // Start at origin with default directions

    for (char c : lSystemString) {
        switch (c) {
        case 'B': { // Create a base or soil
            // Define the size and position of the base
            glm::vec3 baseSize = glm::vec3(length * 10, length * 0.2f, length * 10); // Base dimensions
            glm::vec3 basePosition = turtle.position - glm::vec3(0.0f, baseSize.y / 2.0f, 0.0f); // Place base below current position

            // Generate cube geometry for the base
            std::vector<GLfloat> baseVertices;
            generateShape(PrimitiveType::PRIMITIVE_CUBE, baseVertices);

            // Calculate model matrix for the base
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), basePosition) *
                                    glm::scale(glm::mat4(1.0f), baseSize);

            // Add the base to the scene
            createShapeData(
                baseVertices,
                glm::vec4(0.2f, 0.1f, 0.0f, 1.0f), // Dark brown color for soil
                glm::vec4(0.3f, 0.2f, 0.1f, 1.0f), // Diffuse color
                glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), // Specular color
                16.0f,                              // Shininess
                m_ground_texture,                   // Base texture (if available)
                modelMatrix                         // Model matrix
                );

            // // Generate grass in a grid pattern
            // float grassSpacing = length; // Distance between grass instances
            // int gridCount = 10;          // Number of grass instances in one row/column

            // for (int i = -gridCount / 2; i <= gridCount / 2; ++i) {
            //     for (int j = -gridCount / 2; j <= gridCount / 2; ++j) {
            //         glm::vec3 grassPosition = basePosition + glm::vec3(i * grassSpacing, baseSize.y, j * grassSpacing);

            //         // Define the size of each grass instance
            //         glm::vec3 grassSize = glm::vec3(length * 0.1f, length * 0.5f, length * 0.1f); // Tall and thin

            //         // Generate cone geometry (as grass)
            //         std::vector<GLfloat> grassVertices;
            //         generateShape(PrimitiveType::PRIMITIVE_CONE, grassVertices); // Use cone for grass

            //         // Calculate model matrix for the grass
            //         glm::mat4 grassModelMatrix = glm::translate(glm::mat4(1.0f), grassPosition) *
            //                                      glm::scale(glm::mat4(1.0f), grassSize);

            //         // Add the grass to the scene
            //         createShapeData(
            //             grassVertices,
            //             glm::vec4(0.2f, 0.8f, 0.2f, 1.0f), // Green grass color
            //             glm::vec4(0.3f, 0.9f, 0.3f, 1.0f), // Diffuse color
            //             glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // Specular color
            //             8.0f,                              // Shininess
            //             0,                                 // No texture for now
            //             grassModelMatrix                   // Model matrix
            //             );
            //     }
            // }

            break;
        }
        case 'F': { // Create a trunk or branch
            glm::vec3 newPosition = turtle.position + turtle.growDirection * length;

            std::vector<GLfloat> trunkVertices;
            generateShape(PrimitiveType::PRIMITIVE_CYLINDER, trunkVertices);

            glm::mat4 modelMatrix = calculateModelMatrix(turtle.position, newPosition, 0.02f);

            createShapeData(
                trunkVertices,
                glm::vec4(0.4f, 0.3f, 0.2f, 1.0f), // Root ambient color
                glm::vec4(0.5f, 0.4f, 0.3f, 1.0f), // Root diffuse color
                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Root specular color
                32.0f,                              // Shininess
                m_trunk_texture,  // Texture
                modelMatrix                      // Model matrix
                );

            turtle.position = newPosition;
            break;
        }
        case 'X': { // Create a root or thinner branch
            glm::vec3 newPosition = turtle.position + turtle.growDirection * (length * 0.5f);

            std::vector<GLfloat> branchVertices;
            generateShape(PrimitiveType::PRIMITIVE_CYLINDER, branchVertices);

            glm::mat4 modelMatrix = calculateModelMatrix(turtle.position, newPosition, 0.02f);

            createShapeData(
                branchVertices,
                glm::vec4(0.4f, 0.3f, 0.2f, 1.0f), // Root ambient color
                glm::vec4(0.5f, 0.4f, 0.3f, 1.0f), // Root diffuse color
                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Root specular color
                32.0f,                              // Shininess
                m_branch_texture,  // Texture
                modelMatrix                      // Model matrix
                );

            turtle.position = newPosition;
            break;
        }
        case 'L': { // Create a leaf
            glm::vec3 newPosition = turtle.position + turtle.growDirection * (length * 0.5f);

            std::vector<GLfloat> leafVertices;
            generateShape(PrimitiveType::PRIMITIVE_CUBE, leafVertices); // Use cone as leaf

            glm::mat4 modelMatrix = calculateModelMatrix(turtle.position, newPosition, 0.1f);

            createShapeData(
                leafVertices,
                glm::vec4(0.0f, 0.8f, 0.0f, 1.0f), // Leaf ambient color
                glm::vec4(0.1f, 0.9f, 0.1f, 1.0f), // Leaf diffuse color
                glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // Leaf specular color
                16.0f,                              // Shininess
                m_leaf_texture,  // Leaf texture
                modelMatrix                      // Model matrix
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
    m_shapeData.push_back(shapeData);

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
