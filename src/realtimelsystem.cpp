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
    lights.clear();
    // Example point light
    CustomLightData pointLight;
    pointLight.type = 0; // Point light
    pointLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // White light
    pointLight.function = glm::vec3(1.0f, 0.1f, 0.01f); // Attenuation
    pointLight.position = glm::vec4(2.0f, 2.0f, 2.0f, 1.0f); // Position in world space
    lights.push_back(pointLight);

    // Example directional light
    CustomLightData directionalLight;
    directionalLight.type = 1; // Directional light
    directionalLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // White light
    directionalLight.direction = glm::vec4(-1.0f, -1.0f, -1.0f, 0.0f); // Direction in world space
    lights.push_back(directionalLight);
}

void Realtime::interpretLSystem(const std::string& lSystemString, float angle, float length) {
    std::vector<GLfloat> vertices; // To store all line segment vertices

    std::stack<std::pair<glm::vec3, glm::vec3>> stateStack;

    glm::vec3 position(0.0f, 0.0f, 0.0f);    // Starting position
    glm::vec3 direction(0.0f, 1.0f, 0.0f);   // Starting direction (Y-axis)

    glm::vec3 up(0.0f, 0.0f, 1.0f);          // Default "up" vector (Z-axis)

    for (char c : lSystemString) {
        if (c == 'F') {
            glm::vec3 newPosition = position + direction * length;
            drawLine(position, newPosition, vertices); // Add line vertices
            position = newPosition;
        } else if (c == '+') {
            glm::mat4 rotationMatrix = customRotate(up, glm::radians(-angle));
            direction = glm::vec3(rotationMatrix * glm::vec4(direction, 0.0f));
        } else if (c == '-') {
            glm::mat4 rotationMatrix = customRotate(up, glm::radians(angle));
            direction = glm::vec3(rotationMatrix * glm::vec4(direction, 0.0f));
        } else if (c == '[') {
            stateStack.push({position, direction});
        } else if (c == ']') {
            if (!stateStack.empty()) {
                auto state = stateStack.top();
                stateStack.pop();
                position = state.first;
                direction = state.second;
            }
        }
    }

    // Create VBO and VAO for the entire line data
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

    // Initialize material properties
    lineShape.ambient = glm::vec4(1.f, 0.f, 0.f, 1.0f); // Example ambient color
    lineShape.diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f); // Example diffuse color
    lineShape.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Example specular color
    lineShape.shininess = 32.0f; // Example shininess coefficient

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

    // Define view and projection matrices
    glm::vec3 eye(-3.0f, -3.0f, -3.0f); // Camera position
    glm::vec3 center(0.0f, 0.0f, 0.0f); // Look-at target
    glm::vec3 up(0.0f, 1.0f, 0.0f);     // Up direction

    glm::mat4 viewMatrix = glm::lookAt(eye, center, up);
    glm::mat4 projMatrix = glm::perspective(glm::radians(30.0f), static_cast<float>(m_width) / m_height, 0.1f, 100.0f);

    // Pass view and projection matrices
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "projMatrix"), 1, GL_FALSE, &projMatrix[0][0]);

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
