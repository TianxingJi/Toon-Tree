#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "utils/sceneloader.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

struct ShapeData {
    GLuint vao;           // VAO for shape
    GLuint vbo;           // VBO for shape
    int vertexCount;      // Number of vertices
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;
    glm::mat4 modelMatrix; // Transformation matrix for the shape
    bool textureUsed;
    // only textureUsed is true, the blow data will be used
    GLuint diffuseTexture;
    float blend;
    float repeatU;
    float repeatV;
};

struct CustomLightData {
    int type;          // 0 = Point, 1 = Directional, 2 = Spot
    glm::vec4 color;   // Light color
    glm::vec3 function; // Attenuation coefficients (x, y, z)
    glm::vec4 position; // Position in world space
    glm::vec4 direction; // Direction in world space
    float penumbra;    // Inner cone angle for spotlights
    float angle;       // Outer cone angle for spotlights
};

struct TurtleState {
    glm::vec3 position;         // Current position of the turtle
    glm::vec3 growDirection;    // Y-axis equivalent: direction in which the turtle "grows"
    glm::vec3 forwardDirection; // Z-axis equivalent: direction the turtle is "facing"
    glm::vec3 rightDirection;   // X-axis equivalent: right-hand direction

    TurtleState(const glm::vec3& pos,
                const glm::vec3& growDir = glm::vec3(0.0f, 1.0f, 0.0f),
                const glm::vec3& forwardDir = glm::vec3(0.0f, 0.0f, -1.0f),
                const glm::vec3& rightDir = glm::vec3(1.0f, 0.0f, 0.0f))
        : position(pos),
        growDirection(glm::normalize(growDir)),
        forwardDirection(glm::normalize(forwardDir)),
        rightDirection(glm::normalize(rightDir)) {}
};

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();

    // Below is new logic for l system
    glm::mat4 customRotate(const glm::vec3& axis, float radians);
    void LSystemShapeDataGeneration();
    void lSystemGeneration();
    void interpretLSystem(const std::string& lSystemString, float angle, float length);
    void generateShape(PrimitiveType type, std::vector<GLfloat> &vertices);
    glm::mat4 calculateModelMatrix(const glm::vec3 &start, const glm::vec3 &end, float thickness);
    void createShapeData(
        const std::vector<GLfloat>& vertices,
        const glm::vec4& ambientColor,
        const glm::vec4& diffuseColor,
        const glm::vec4& specularColor,
        float shininess,
        const GLuint& texture,
        const glm::mat4& modelMatrix,
        float blend = 1.0f,  // Default blend factor
        float repeatU = 1.0f, // Default U texture repeat
        float repeatV = 1.0f  // Default V texture repeat
        );
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void generateShapeData();

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;

    // For Frame Buffer Object
    void makeFBO();

    // For original Gemoetry painting
    void paintGeometry();

    // For L System
    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);
    GLuint m_trunk_texture;
    GLuint m_branch_texture;
    GLuint m_leaf_texture;
    GLuint m_ground_texture;
    void loadTexture(const std::string& filepath, GLuint& texture);

    // Camera parameters
    glm::vec3 eye = glm::vec3(0.0f, 0.0f, 3.0f);  // Default camera position
    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);  // Look-at target
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);  // Up direction

    void initializeLights();
    void paintLSystem();

    // Task 30: Update the paintTexture function signature
    void paintFBOTexture(GLuint texture, bool enablePerPixelFilter, bool enableKernelFilter);

    GLuint m_defaultFBO;
    int m_fbo_width;
    int m_fbo_height;

    GLuint m_texture_shader;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    QImage m_image;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;

    int m_width; // m_screen_width
    int m_height; // m_screen_height

    GLuint m_shader;     // Stores id of shader program
    GLuint m_vbo; // Stores id of vbo
    GLuint m_vao; // Stores id of vao
    std::vector<float> m_data;
    std::vector<ShapeData> m_shapeData;  // Container for all shapes' data
    std::vector<CustomLightData> lights;

    RenderData metaData;
    sceneloader sceneLoader;
};
