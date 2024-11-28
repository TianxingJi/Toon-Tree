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
    void drawLine(const glm::vec3& start, const glm::vec3& end, std::vector<GLfloat>& vertices);

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

    void initializeLights();
    void paintLSystem();

    // Task 30: Update the paintTexture function signature
    void paintFBOTexture(GLuint texture, bool enablePerPixelFilter, bool enableKernelFilter, bool enablePerPixelFilter2, bool enableKernelFilter2);

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
