#include "realtime.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>

float randomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

// Initialize particles
void Realtime::initializeParticles() {
    particles.clear();
    particles.reserve(maxParticles);

    for (int i = 0; i < maxParticles; ++i) {
        Particle p;
        // Randomly place particles within the range x,z in [-10,10] and y in [5,10]
        p.position = glm::vec3(
            randomFloat(-10.0f, 10.0f),
            randomFloat(5.0f, 10.0f),
            randomFloat(-10.0f, 10.0f)
            );

        // Initial velocity mainly downward, with slight random horizontal components
        p.velocity = glm::vec3(
            randomFloat(-0.25f, 0.25f),       // Small random offset in X direction
            -(1.0f + randomFloat(0.0f, 1.0f)), // Downward direction in Y
            randomFloat(-0.25f, 0.25f)        // Small random offset in Z direction
            );

        p.lifetime = 0.0f;
        p.size = 5.0f; // Adjust point size as needed
        p.opacity = 1.0f;

        particles.push_back(p);
    }

    // Create VAO and VBO for storing particle data
    glGenVertexArrays(1, &m_particleVAO);
    glBindVertexArray(m_particleVAO);

    glGenBuffers(1, &m_particleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

    // Position attribute: vec3
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));

    // Size attribute: float
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::updateParticles(float deltaTime) {
    const float gravity = 0.3f;
    const float groundLevel = -0.8f; // Ground level Y coordinate
    const float resetY = 5.0f;

    for (Particle &p : particles) {
        // Apply gravity
        p.velocity.y -= gravity * deltaTime;

        // Apply slight wind disturbance
        p.velocity.x += randomFloat(-0.01f, 0.01f);
        p.velocity.z += randomFloat(-0.01f, 0.01f);

        // Update position
        p.position += p.velocity * deltaTime;

        // If the particle falls below the ground level, reset it
        if (p.position.y < groundLevel - 1.0f) {
            p.position.y = resetY + randomFloat(0.0f, 5.0f);
            p.position.x = randomFloat(-10.0f, 10.0f);
            p.position.z = randomFloat(-10.0f, 10.0f);

            p.velocity = glm::vec3(
                randomFloat(-0.25f, 0.25f),
                -(1.0f + randomFloat(0.0f, 1.0f)),
                randomFloat(-0.25f, 0.25f)
                );
        }
    }
}

void Realtime::renderParticles() {
    glUseProgram(m_particle_shader);

    glUniformMatrix4fv(glGetUniformLocation(m_particle_shader, "viewMatrix"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_particle_shader, "projMatrix"), 1, GL_FALSE, &m_proj[0][0]);
    glUniform1f(glGetUniformLocation(m_particle_shader, "nearPlane"), settings.nearPlane);
    glUniform1f(glGetUniformLocation(m_particle_shader, "u_time"), m_time);

    // Upload updated particle data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(m_particleVAO);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays(GL_POINTS, 0, particles.size());

    glBindVertexArray(0);
    glUseProgram(0);
}
