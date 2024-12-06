#version 330 core

// Task 4: declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.
layout(location = 0) in vec3 objectSpacePosition;
layout(location = 1) in vec3 objectSpaceNormal;
layout(location = 2) in vec2 uv;        // UV coordinates

// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
out vec3 worldSpacePosition;
out vec3 worldSpaceNormal;
out vec2 TexCoords; // Pass UV coordinates to fragment shader
out vec4 fragPosLightSpace; // Add an output for the light space position

// Task 6: declare a uniform mat4 to store model matrix
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

// Task 7: declare uniform mat4's for the view and projection matrix
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

// Add a uniform for the light space transformation matrix
uniform mat4 lightSpaceMatrix;

void main() {
    TexCoords = uv; // Pass UV to fragment shader
    // Task 8: compute the world-space position and normal, then pass them to
    //         the fragment shader using the variables created in task 5
    worldSpacePosition = vec3(modelMatrix * vec4(objectSpacePosition, 1.f)) ;

    worldSpaceNormal = normalMatrix * objectSpaceNormal;

    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.

    // Task 9: set gl_Position to the object space position transformed to clip space
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(objectSpacePosition, 1.0);

    // Transform to light space
    fragPosLightSpace = lightSpaceMatrix * modelMatrix * vec4(objectSpacePosition, 1.0);
}
