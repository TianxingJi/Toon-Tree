#include "realtime.h"
#include "shapes/vbogenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Realtime::generateShapeData(){
    m_shapeData.clear(); // Clear any existing shape data

    for (const RenderShapeData& shape : sceneLoader.getShapes()) {
        m_data.clear(); // clear m_data to store vboData
        ShapeData shapeData;

        int adjustedParam1 = settings.shapeParameter1;
        int adjustedParam2 = settings.shapeParameter2;

        if (shape.primitive.type == PrimitiveType::PRIMITIVE_MESH) {
            m_data.insert(m_data.end(), shape.vboData.begin(), shape.vboData.end());
        } else {
            generateVBOBasedOnType(adjustedParam1, adjustedParam2, m_data, shape.primitive.type);
        }

        // Generate and bind VBO
        glGenBuffers(1, &shapeData.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo);

        // Send Data to VBO
        glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(GLfloat), m_data.data(), GL_STATIC_DRAW);

        // Generate and bind VAO
        glGenVertexArrays(1, &shapeData.vao);
        glBindVertexArray(shapeData.vao);

        // Enable and define attribute 0 to store vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

        // Enable and define attribute 2 to store UV coordinates
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(6 * sizeof(GLfloat)));

        // Store other relevant data
        shapeData.vertexCount = m_data.size() / 8;
        shapeData.modelMatrix = shape.ctm;
        shapeData.ambient = shape.primitive.material.cAmbient;
        shapeData.diffuse = shape.primitive.material.cDiffuse;
        shapeData.specular = shape.primitive.material.cSpecular;
        shapeData.shininess = shape.primitive.material.shininess;

        if(shape.primitive.material.textureMap.isUsed){ // if there is texture
            shapeData.textureUsed = true;
            shapeData.blend = shape.primitive.material.blend;
            shapeData.repeatU = shape.primitive.material.textureMap.repeatU;
            shapeData.repeatV = shape.primitive.material.textureMap.repeatV;

            // Below is how we handle texture
            // Prepare filepath
            QString textureFilePath = QString::fromStdString(shape.primitive.material.textureMap.filename);

            // Task 1: Obtain image from filepath
            m_image = QImage(textureFilePath);

            // Task 2: Format image to fit OpenGL
            m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();

            // Task 3: Generate kitten texture
            glGenTextures(1, &shapeData.diffuseTexture);

            // Task 9: Set the active texture slot to texture slot 0
            glActiveTexture(GL_TEXTURE0);

            // Task 4: Bind kitten texture
            glBindTexture(GL_TEXTURE_2D, shapeData.diffuseTexture);

            // Task 5: Load image into kitten texture
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());

            // Task 6: Set min and mag filters' interpolation mode to linear
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Task 7: Unbind kitten texture
            glBindTexture(GL_TEXTURE_2D, 0);

            // Clear m_image;
            m_image = QImage();
        }

        // Unbind VAO and VBO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Add to shape data collection
        m_shapeData.push_back(shapeData);
    }
}

void Realtime::paintGeometry(){
    // The below code will be great if I can split it into antoher function called paintGeometry()
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);  // Use the compiled shader program

    // Set view and projection matrices (assuming they are available in `m_viewMatrix` and `m_projMatrix`)
    // Below is the information that need to be changed as key pressed and mouse wheel moved
    // 1. Get the information of updated parameters and then convey them to the camera to update view and projMatrix
    // 2. Store them into the sceneLoader
    // 3. update
    GLint viewLoc = glGetUniformLocation(m_shader, "viewMatrix");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &sceneLoader.getViewMatrix()[0][0]);

    GLint projLoc = glGetUniformLocation(m_shader, "projMatrix");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &sceneLoader.getProjMatrix()[0][0]);

    GLint cameraLocation = glGetUniformLocation(m_shader, "cameraPosition");
    glUniform4fv(cameraLocation, 1, &sceneLoader.getCamera().pos[0]);

    // Below is the information that will never be changed as the key and mouse events

    // Task 12: pass m_ka into the fragment shader as a uniform
    GLint kaLocation = glGetUniformLocation(m_shader, "ka");
    glUniform1f(kaLocation, sceneLoader.sceneGlobalData.ka);

    // Task 13: pass light position and m_kd into the fragment shader as a uniform
    GLint kdLocation = glGetUniformLocation(m_shader, "kd");
    glUniform1f(kdLocation, sceneLoader.sceneGlobalData.kd);

    // Task 14: pass shininess, m_ks, and world-space camera position
    GLint ksLocation = glGetUniformLocation(m_shader, "ks");
    glUniform1f(ksLocation, sceneLoader.sceneGlobalData.ks);

    // Pass light data
    const std::vector<SceneLightData>& lights = sceneLoader.getLights();
    int numLights = std::min(static_cast<int>(lights.size()), 8); // Max 8 lights
    glUniform1i(glGetUniformLocation(m_shader, "numLights"), numLights);

    for (int i = 0; i < numLights; ++i) {
        const SceneLightData& light = lights[i];

        // Set light color
        std::string baseName = "lights[" + std::to_string(i) + "]";
        glUniform4fv(glGetUniformLocation(m_shader, (baseName + ".color").c_str()), 1, &light.color[0]);

        // Set light attenuation function
        glUniform3fv(glGetUniformLocation(m_shader, (baseName + ".function").c_str()), 1, &light.function[0]);

        // Set position and direction
        glUniform4fv(glGetUniformLocation(m_shader, (baseName + ".position").c_str()), 1, &light.pos[0]);
        glUniform4fv(glGetUniformLocation(m_shader, (baseName + ".direction").c_str()), 1, &light.dir[0]);

        // Set type
        int type;
        switch (light.type) {
        case LightType::LIGHT_POINT: type = 0; break;
        case LightType::LIGHT_DIRECTIONAL: type = 1; break;
        case LightType::LIGHT_SPOT: type = 2; break;
        default: type = 3; break;
        }
        glUniform1i(glGetUniformLocation(m_shader, (baseName + ".type").c_str()), type);

        // Set spotlight parameters if applicable
        glUniform1f(glGetUniformLocation(m_shader, (baseName + ".penumbra").c_str()), light.penumbra);
        glUniform1f(glGetUniformLocation(m_shader, (baseName + ".angle").c_str()), light.angle);
    }

    for (const ShapeData& shapeData : m_shapeData) {
        glBindVertexArray(shapeData.vao);

        // Set model matrix uniform
        GLint modelLoc = glGetUniformLocation(m_shader, "modelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &shapeData.modelMatrix[0][0]);

        glm::mat3 normalMatrix = glm::inverse(glm::transpose(shapeData.modelMatrix));
        GLint normalMatrixLocation = glGetUniformLocation(m_shader, "normalMatrix");
        glUniformMatrix3fv(normalMatrixLocation, 1, GL_FALSE, &normalMatrix[0][0]);

        // Set material properties
        GLint ambientLoc = glGetUniformLocation(m_shader, "material.ambient");
        glUniform4fv(ambientLoc, 1, &shapeData.ambient[0]);

        GLint diffuseLoc = glGetUniformLocation(m_shader, "material.diffuse");
        glUniform4fv(diffuseLoc, 1, &shapeData.diffuse[0]);

        GLint specularLoc = glGetUniformLocation(m_shader, "material.specular");
        glUniform4fv(specularLoc, 1, &shapeData.specular[0]);

        GLint shininessLoc = glGetUniformLocation(m_shader, "material.shininess");
        glUniform1f(shininessLoc, shapeData.shininess);

        if (shapeData.textureUsed) {
            // Pass the textureUsed uniform
            GLint textureUsedLocation = glGetUniformLocation(m_shader, "textureUsed");
            glUniform1i(textureUsedLocation, true); // Set to true

            // Pass the texture to shader (already bound to slot 1 as per your setup)
            GLint textureLocation = glGetUniformLocation(m_shader, "Texture");
            glActiveTexture(GL_TEXTURE1); // Set the active texture slot
            glBindTexture(GL_TEXTURE_2D, shapeData.diffuseTexture);
            glUniform1i(textureLocation, 1); // Slot 1

            // Pass blend value
            GLint blendLocation = glGetUniformLocation(m_shader, "blend");
            glUniform1f(blendLocation, shapeData.blend);

            // Pass repeatU and repeatV values
            GLint repeatULocation = glGetUniformLocation(m_shader, "repeatU");
            glUniform1f(repeatULocation, shapeData.repeatU);

            GLint repeatVLocation = glGetUniformLocation(m_shader, "repeatV");
            glUniform1f(repeatVLocation, shapeData.repeatV);
        }

        // Draw the shape
        glDrawArrays(GL_TRIANGLES, 0, shapeData.vertexCount);

        // Unbind VAO (optional for good practice)
        glBindVertexArray(0);
    }

    glUseProgram(0);  // Unbind the shader program

    // The above should be split into a seprated function to simplify the code
}
