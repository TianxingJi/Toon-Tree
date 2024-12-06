#version 330 core

out vec4 FragColor;

uniform float nearPlane;
uniform float u_time;

vec3 hsv2rgb(vec3 c) {
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0,4,2),
                             6.0)-3.0)-1.0,
                      0.0,
                      1.0);
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix(vec3(1.0), rgb, c.y);
}

void main() {
    if (nearPlane == 1.0) {
        float hue = fract(u_time * 0.1);
        vec3 color = hsv2rgb(vec3(hue, 1.0, 1.0));
        FragColor = vec4(color, 0.9);
    } else {
        FragColor = vec4(1.0, 1.0, 1.0, 0.9);
    }
}
