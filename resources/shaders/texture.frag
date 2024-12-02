#version 330 core

// Task 16: Create a UV coordinate in variable
in vec2 TexCoords;

// Task 8: Add a sampler2D uniform
uniform sampler2D Texture;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool enablePerPixelFilter;
uniform bool enableKernelFilter;
uniform bool enablePerPixelFilter2;
uniform bool enableKernelFilter2;

uniform vec2 texelSize;

out vec4 fragColor;

void main()
{
    // Start with the original texture color
    vec4 baseColor = texture(Texture, TexCoords);

    // Apply Per-pixel filters first (modify `baseColor`)
    if (enablePerPixelFilter) {
        baseColor.rgb = vec3(1.0) - baseColor.rgb; // Invert colors
    }

    // After applying Per-pixel filters, use `baseColor` for Kernel-based filters
    vec3 result = baseColor.rgb;

    // Apply Kernel-based sharpen filter
    if (enableKernelFilter) {
        vec3 sharpenResult = vec3(0.0);

        // Sharpen kernel
        float kernel[9] = float[](
            -1, -1, -1,
            -1, 17, -1,
            -1, -1, -1
        );

        // Define offsets for sampling neighboring pixels
        vec2 offsets[9] = vec2[9](
            vec2(-1.0,  1.0) * texelSize, // Top-left
            vec2( 0.0,  1.0) * texelSize, // Top-center
            vec2( 1.0,  1.0) * texelSize, // Top-right
            vec2(-1.0,  0.0) * texelSize, // Middle-left
            vec2( 0.0,  0.0) * texelSize, // Center
            vec2( 1.0,  0.0) * texelSize, // Middle-right
            vec2(-1.0, -1.0) * texelSize, // Bottom-left
            vec2( 0.0, -1.0) * texelSize, // Bottom-center
            vec2( 1.0, -1.0) * texelSize  // Bottom-right
        );

        // Perform convolution with the kernel
        for (int i = 0; i < 9; ++i) {
            vec3 sample = texture(Texture, TexCoords + offsets[i]).rgb; // Sample original texture
            sharpenResult += sample * kernel[i];
        }

        // Normalize and clamp the result
        sharpenResult /= 9.0;
        result = clamp(sharpenResult, 0.0, 1.0); // Combine the results

        // Apply Per-pixel filters first (modify `baseColor`)
        if (enablePerPixelFilter) {
            result.rgb = vec3(1.0) - result.rgb; // Invert colors
        }
    }

    // Final output color
    fragColor = vec4(result, baseColor.a);
}
