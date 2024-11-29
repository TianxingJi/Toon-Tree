#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "lsystem/lsystem.h"
#include "settings.h"
#include "utils/shaderloader.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    // Delete VBO and VAO and Shader
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteProgram(m_shader);

    glDeleteProgram(m_texture_shader);
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);

    // Task 35: Delete OpenGL memory here
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    this->doneCurrent();
}

void Realtime::initializeGL() { // TODO: m_Data should be finished
    m_devicePixelRatio = this->devicePixelRatio();

    m_defaultFBO = 2;
    m_width = size().width() * m_devicePixelRatio;
    m_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_width;
    m_fbo_height = m_height;

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, m_width, m_height);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    // Set the clear color here
    glClearColor(0, 0, 0, 1);

    // Shader Loader
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/phong.vert", ":/resources/shaders/phong.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");

    // generateShapeData();
    initializeLights();

    // Prepare filepath
    QString treeTrunk_filepath = QString(":/resources/images/treeTrunk.jpg");

    // Task 1: Obtain image from filepath
    m_image = QImage(treeTrunk_filepath);

    // Task 2: Format image to fit OpenGL
    m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();

    // Task 3: Generate kitten texture
    glGenTextures(1, &m_trunk_texture);

    // Task 9: Set the active texture slot to texture slot 0
    glActiveTexture(GL_TEXTURE0);

    // Task 4: Bind kitten texture
    glBindTexture(GL_TEXTURE_2D, m_trunk_texture);

    // Task 5: Load image into kitten texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());

    // Task 6: Set min and mag filters' interpolation mode to linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Task 7: Unbind kitten texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Set up for the frame buffer object
    // Task 10: Set the texture.frag uniform for our texture
    glUseProgram(m_texture_shader);

    GLint textureLocation = glGetUniformLocation(m_texture_shader, "Texture");

    glUniform1i(textureLocation, 0);

    glUseProgram(0);

    // Set the phong.frag uniform for our texture
    glUseProgram(m_shader);

    GLint textureMapLocation = glGetUniformLocation(m_shader, "Texture");

    glUniform1i(textureMapLocation, 1);

    glUseProgram(0);

    std::vector<GLfloat> fullscreen_quad_data =
        { // POSITIONS       // UV COORDINATES
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,  // Upper Left
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // Bottom Left
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // Bottom Right

            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,  // Upper Left
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // Bottom Right
            1.0f,  1.0f, 0.0f,   1.0f, 1.0f   // Upper Right
        };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // Task 14: modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(0 * sizeof(GLfloat)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();

    // L System Logic Below
    m_view = glm::lookAt(eye, center, up);
    m_proj = glm::perspective(glm::radians(30.0f), static_cast<float>(m_width) / m_height, settings.nearPlane, settings.farPlane);
}

void Realtime::makeFBO(){
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Task 18: Generate anzd bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::paintGL() {

    // Task 24: Bind our FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Task 28: Call glViewport
    glViewport(0, 0, m_fbo_width, m_fbo_height);
    // glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // paintGeometry();
    // Paint L-System geometry
    paintLSystem();

    // Task 25: Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_width, m_height);
    // glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    // Task 26: Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Task 27: Call paintTexture to draw our FBO color attachment texture | Task 31: Set bool parameter to true
    paintFBOTexture(m_fbo_texture, settings.perPixelFilter, settings.kernelBasedFilter, settings.extraCredit3, settings.extraCredit4);
}

// Task 31: Update the paintTexture function signature
void Realtime::paintFBOTexture(GLuint texture, bool enablePerPixelFilter, bool enableKernelFilter, bool enablePerPixelFilter2, bool enableKernelFilter2){
    glUseProgram(m_texture_shader);
    // Task 32: Set your bool uniform on whether or not to filter the texture drawn
    glUniform1f(glGetUniformLocation(m_texture_shader, "enablePerPixelFilter"), enablePerPixelFilter);
    glUniform1i(glGetUniformLocation(m_texture_shader, "enableKernelFilter"), enableKernelFilter);
    glUniform1f(glGetUniformLocation(m_texture_shader, "enablePerPixelFilter2"), enablePerPixelFilter2);
    glUniform1i(glGetUniformLocation(m_texture_shader, "enableKernelFilter2"), enableKernelFilter2);

    // Task: Calculate texelSize and pass it to the shader
    float texelWidth = 1.0f / static_cast<float>(m_fbo_width);
    float texelHeight = 1.0f / static_cast<float>(m_fbo_height);
    glUniform2f(glGetUniformLocation(m_texture_shader, "texelSize"), texelWidth, texelHeight);

    glBindVertexArray(m_fullscreen_vao);
    // Task 10: Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Task 34: Delete Texture, Renderbuffer, and Framebuffer memory
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_width = size().width() * m_devicePixelRatio;
    m_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_width;
    m_fbo_height = m_height;

    makeFBO();

    // // Tells OpenGL how big the screen is
    // glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    // new perspective should be calculated here
    m_proj = glm::perspective(glm::radians(30.0f), static_cast<float>(m_width) / m_height, settings.nearPlane, settings.farPlane);

    update();
}

void clearShapeData(std::vector<ShapeData>& shapeData) {
    for (ShapeData& shape : shapeData) {
        // Delete the associated OpenGL resources
        glDeleteBuffers(1, &shape.vbo);        // Delete the Vertex Buffer Object (VBO)
        glDeleteVertexArrays(1, &shape.vao);  // Delete the Vertex Array Object (VAO)
        shape.vbo = 0;
        shape.vao = 0;
    }

    // Clear the vector to remove all ShapeData entries
    shapeData.clear();
}

void Realtime::sceneChanged() {

    if(SceneParser::parse(settings.sceneFilePath, metaData)){
        sceneLoader.setSceneLoader(m_width, m_height, metaData);

        // clean the shape Data that store all of the handled shape data
        clearShapeData(m_shapeData);
        // TODO data changed
        // Regenerate shape data with the new scene configuration
        generateShapeData();

        update(); // asks for a PaintGL() call to occur
    } else {
        // output file failed to read
        std::cerr << "Failed to load scene file: " << settings.sceneFilePath << std::endl;
    }

}

void Realtime::LSystemShapeDataGeneration() {
    clearShapeData(m_shapeData);

    // Define the axiom and 3D branching rules
    std::string axiom = "X";
    std::unordered_map<char, std::string> rules = {
        {'X', "XXXF"},  // 3D branching with all operators
        {'F', "FF"}                       // Rule for elongation
    };

    // Set the number of iterations
    int iterations = settings.shapeParameter1;

    // Generate the L-System string
    LSystem lSystem(axiom, rules, iterations);
    std::string lSystemString = lSystem.generate();

    // Set angle and length based on user parameters
    float angle = 25.0f * settings.shapeParameter3;    // Base angle
    float length = settings.shapeParameter2 * 0.1f;   // Segment length

    // Interpret the generated L-System string to create geometry
    interpretLSystem(lSystemString, angle, length);
}

void Realtime::lSystemGeneration() {
    LSystemShapeDataGeneration();
    update(); // Request redraw
}

void Realtime::settingsChanged() {
    // Static variables to track previous settings
    static Settings previousSettings = settings;

    // Check if nearPlane or farPlane has changed
    if (settings.nearPlane != previousSettings.nearPlane ||
        settings.farPlane != previousSettings.farPlane) {
        // TODO::only update proj matrix here
        m_proj = glm::perspective(glm::radians(30.0f), static_cast<float>(m_width) / m_height, settings.nearPlane, settings.farPlane);
    }

    // Check if shapeParameter1 or shapeParameter2 has changed
    if (settings.shapeParameter1 != previousSettings.shapeParameter1 ||
        settings.shapeParameter2 != previousSettings.shapeParameter2 ||
        settings.shapeParameter3 != previousSettings.shapeParameter3) {
        LSystemShapeDataGeneration();
    }

    // Update the stored previous settings
    previousSettings = settings;

    // Always update the view
    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        // Get the current mouse position
        float posX = event->position().x();
        float posY = event->position().y();

        // Calculate the mouse movement offset
        float deltaX = posX - m_prev_mouse_pos.x;
        float deltaY = posY - m_prev_mouse_pos.y;

        // Update the previous mouse position
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Set mouse sensitivity for rotation
        float sensitivity = 0.005f;

        // Horizontal rotation (rotate around the world's Y-axis)
        if (deltaX != 0) {
            float angle = -deltaX * sensitivity; // Reverse the sign for intuitive left/right rotation
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis

            // Update the look direction (center - eye)
            glm::vec3 lookDir = glm::normalize(center - eye);
            lookDir = glm::mat3(rotationMatrix) * lookDir; // Apply rotation to the look direction
            center = eye + lookDir; // Update the target point
        }

        // // Vertical rotation (rotate around the camera's right vector)
        // if (deltaY != 0) {
        //     float angle = -deltaY * sensitivity; // Reverse the sign for intuitive up/down rotation
        //     glm::vec3 right = glm::normalize(glm::cross(center - eye, up)); // Calculate the right vector of the camera

        //     // Create a rotation matrix around the right vector and apply it
        //     glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, right);
        //     glm::vec3 lookDir = glm::normalize(center - eye); // Current look direction
        //     glm::vec3 newLookDir = glm::mat3(rotationMatrix) * lookDir; // New look direction

        //     // Prevent flipping by clamping the vertical rotation
        //     if (glm::dot(newLookDir, up) > 0.99f || glm::dot(newLookDir, up) < -0.99f) {
        //         // Ignore rotation to prevent flipping if the look direction gets too vertical
        //     } else {
        //         center = eye + newLookDir; // Update the target point
        //         up = glm::normalize(glm::cross(right, newLookDir)); // Update the up vector
        //     }
        // }

        // Update the view matrix using the new eye, center, and up vectors
        m_view = glm::lookAt(eye, center, up);

        // Request a redraw to reflect the updated camera orientation
        update();
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    // Calculate delta time
    int elapsedms = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f; // Convert to seconds
    m_elapsedTimer.restart();

    // Movement speed
    float moveSpeed = 2.0f; // Adjust as needed for speed

    // Compute the camera directions
    glm::vec3 look = glm::normalize(center - eye); // Forward direction
    glm::vec3 right = glm::normalize(glm::cross(look, up)); // Right direction

    // Initialize movement direction
    glm::vec3 moveDirection(0.0f);

    // Handle movement keys
    if (m_keyMap[Qt::Key_W]) {
        moveDirection += look; // Move forward
    }
    if (m_keyMap[Qt::Key_S]) {
        moveDirection -= look; // Move backward
    }
    if (m_keyMap[Qt::Key_A]) {
        moveDirection -= right; // Move left
    }
    if (m_keyMap[Qt::Key_D]) {
        moveDirection += right; // Move right
    }
    if (m_keyMap[Qt::Key_Space]) {
        moveDirection += up;   // Move up
    }
    if (m_keyMap[Qt::Key_Control]) {
        moveDirection -= up;   // Move down
    }

    // Normalize and scale movement
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection) * moveSpeed * deltaTime;
        eye += moveDirection;  // Update camera position
        center += moveDirection; // Keep the target moving with the camera
    }

    // Update the view matrix
    m_view = glm::lookAt(eye, center, up);

    // Trigger a redraw
    update(); // Ask for a PaintGL() call
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
