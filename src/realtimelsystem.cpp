#include "realtime.h"
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
    // Clear existing lights
    lights.clear();

    // Create a directional light
    CustomLightData directionalLight;
    directionalLight.type = 1; // 1 represents a directional light
    directionalLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // White light
    directionalLight.direction = glm::vec4(-0.5f, -1.0f, -0.5f, 0.0f); // Light direction in world space
    directionalLight.function = glm::vec3(1.0f, 0.0f, 0.0f); // No attenuation (constant light intensity)
    directionalLight.position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Directional lights do not use position

    // Add the light to the lights vector
    lights.push_back(directionalLight);
}

void Realtime::interpretLSystem(const std::string& lSystemString, float angle, float length) {
    m_shapeData.clear();
    std::vector<GLfloat> vertices; // Store vertices for lines

    // Initialize turtle state and stack
    std::stack<TurtleState> stateStack;
    TurtleState turtle(glm::vec3(0.0f, -1.f, 0.0f)); // Start at origin with default directions

    for (char c : lSystemString) {
        switch (c) {
        case 'F': { // Move forward in the grow direction
            glm::vec3 newPosition = turtle.position + turtle.growDirection * length;
            drawLine(turtle.position, newPosition, vertices);
            turtle.position = newPosition;
            break;
        }
        case 'X': { // Draw the root
            glm::vec3 newPosition = turtle.position + turtle.growDirection * (length * 0.5f);
            drawLine(turtle.position, newPosition, vertices);
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
        case '&': { // Rotate GrowDirection forward/backward (Pitch)
            glm::mat4 rotationMatrix = customRotate(turtle.rightDirection, glm::radians(-angle));
            turtle.growDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.growDirection, 0.0f)));
            turtle.forwardDirection = glm::normalize(glm::cross(turtle.rightDirection, turtle.growDirection));
            break;
        }
        case '^': { // Rotate GrowDirection backward/forward (Pitch, opposite)
            glm::mat4 rotationMatrix = customRotate(turtle.rightDirection, glm::radians(angle));
            turtle.growDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.growDirection, 0.0f)));
            turtle.forwardDirection = glm::normalize(glm::cross(turtle.rightDirection, turtle.growDirection));
            break;
        }
        case '/': { // Roll GrowDirection clockwise (Roll)
            glm::mat4 rotationMatrix = customRotate(turtle.growDirection, glm::radians(-angle));
            turtle.forwardDirection = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(turtle.forwardDirection, 0.0f)));
            turtle.rightDirection = glm::normalize(glm::cross(turtle.growDirection, turtle.forwardDirection));
            break;
        }
        case '\\': { // Roll GrowDirection counter-clockwise (Roll, opposite)
            glm::mat4 rotationMatrix = customRotate(turtle.growDirection, glm::radians(angle));
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

    // Generate VAO/VBO for the collected vertices
    ShapeData lineShape;
    glGenVertexArrays(1, &lineShape.vao);
    glGenBuffers(1, &lineShape.vbo);

    glBindVertexArray(lineShape.vao);
    glBindBuffer(GL_ARRAY_BUFFER, lineShape.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(1); // For normal vectors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    lineShape.vertexCount = vertices.size() / 6;

    // Set material properties
    lineShape.ambient = glm::vec4(1.f, 0.f, 0.f, 1.0f); // Example color
    lineShape.diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    lineShape.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    lineShape.shininess = 32.0f;

    lineShape.modelMatrix = glm::mat4(1.0f); // Identity matrix for now
    m_shapeData.push_back(lineShape);

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::drawLine(const glm::vec3& start, const glm::vec3& end, std::vector<GLfloat>& vertices) {
    // Define a default normal vector for the line (assume it's in the Z direction for 2D L-System)
    glm::vec3 normal(0.0f, 0.0f, 1.0f);

    // Add start vertex: position + normal
    vertices.push_back(start.x);
    vertices.push_back(start.y);
    vertices.push_back(start.z);
    vertices.push_back(normal.x);
    vertices.push_back(normal.y);
    vertices.push_back(normal.z);

    // Add end vertex: position + normal
    vertices.push_back(end.x);
    vertices.push_back(end.y);
    vertices.push_back(end.z);
    vertices.push_back(normal.x);
    vertices.push_back(normal.y);
    vertices.push_back(normal.z);
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

        glDrawArrays(GL_LINES, 0, shape.vertexCount);

        glBindVertexArray(0);
    }

    glUseProgram(0);
}
