#version 330 core
struct Material {
    vec4 ambient;  // Ambient reflection coefficient
    vec4 diffuse;  // Diffuse reflection coefficient
    vec4 specular; // Specular reflection coefficient
    float shininess;
};

struct Light {
    int type;         // 0 = Point, 1 = Directional, 2 = Spot
    vec4 color;       // Light color
    vec3 function;    // Attenuation function (only relevant for point and spot lights)
    vec4 position;    // Position (not used for directional lights)
    vec4 direction;   // Direction (only used for directional and spot lights)
    float penumbra;   // Spot light outer cone angle minus inner cone angle in radians
    float angle;      // Spot light outer cone angle in radians
};

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec3 worldSpacePosition;
in vec3 worldSpaceNormal;

// Task 10: declare an out vec4 for your output color
out vec4 fragColor;

uniform Material material;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform float ka;

// Task 13: declare relevant uniform(s) here, for diffuse lighting
uniform float kd;
uniform vec4 lightPosition;

// Task 14: declare relevant uniform(s) here, for specular lighting
uniform float ks;
uniform vec4 cameraPosition;

uniform Light lights[8];   // Array of lights, up to eight lights
uniform int numLights;     // Actual number of active lights

void main() {
    // // Ambient color
    // vec4 ambientColor = ka * material.ambient;
    // fragColor = ambientColor;

    // // Normalize world-space normal
    // vec3 normal = normalize(worldSpaceNormal);

    // // Compute the view direction (from fragment to camera)
    // vec3 viewDir = normalize(vec3(cameraPosition) - worldSpacePosition);

    // // Loop through each light
    // for (int i = 0; i < numLights; i++) {
    //      Light lightData = lights[i];
    //      vec3 lightDir; // Direction to the light
    //      float attenuationFactor = 1.0;

    //      if (lightData.type == 0) { // Point Light

    //          vec3 lightPos = vec3(lightData.position);
    //          lightDir = normalize(lightPos - worldSpacePosition);

    //          // Compute attenuation
    //          float distance = length(lightPos - worldSpacePosition);
    //          attenuationFactor = 1.0 / (lightData.function.x +
    //                                     lightData.function.y * distance +
    //                                     lightData.function.z * distance * distance);

    //      } else if (lightData.type == 1) { // Directional Light
    //          lightDir = normalize(vec3(-lightData.direction)); // Inverse of light direction

    //      } else if (lightData.type == 2) { // Spot Light
    //          vec3 lightPos = vec3(lightData.position);
    //          lightDir = normalize(lightPos - worldSpacePosition);

    //          // Compute attenuation
    //          float distance = length(lightPos - worldSpacePosition);
    //          attenuationFactor = 1.0 / (lightData.function.x +
    //                                     lightData.function.y * distance +
    //                                     lightData.function.z * distance * distance);

    //          // Check spotlight cone
    //          float angleCosine = dot(normalize(-lightDir), normalize(vec3(lightData.direction)));
    //          angleCosine = clamp(angleCosine, -1.0, 1.0);

    //          float currentAngle = acos(angleCosine);
    //          float thetaInner = lightData.angle - lightData.penumbra;
    //          float thetaOuter = lightData.angle;

    //          if (currentAngle > thetaOuter) {
    //          continue; // Outside the spotlight cone
    //          }

    //          if (currentAngle > thetaInner) {
    //          // Compute smooth falloff
    //          float t = (currentAngle - thetaInner) / (thetaOuter - thetaInner);
    //          attenuationFactor *= (1.0 - (3.0 * t * t - 2.0 * t * t * t)); // Smoothstep
    //          }

    //      }

    //      // Diffuse lighting
    //      float diffuseFactor = max(dot(normal, lightDir), 0.0);
    //      vec4 blendedDiffuse =  kd * material.diffuse; // Set up for Texture Mapping

    //      vec4 diffuseColor = blendedDiffuse * diffuseFactor;
    //      fragColor += attenuationFactor * lightData.color * diffuseColor;

    //      // Specular lighting
    //      vec3 reflectedDir = reflect(-lightDir, normal);
    //      float specularFactor = max(dot(normalize(reflectedDir), viewDir), 0.0);

    //      if (material.shininess > 0.0) {
    //          specularFactor = pow(specularFactor, material.shininess);
    //      } else {
    //          specularFactor = 1.0; // Handle shininess = 0 case
    //      }
    //      vec4 specularColor = ks * material.specular * specularFactor;

    //      fragColor += attenuationFactor * lightData.color * specularColor;
    // }

    fragColor = vec4(1.f, 0.f, 0.f, 1.f);

}
