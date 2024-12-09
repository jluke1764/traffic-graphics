#version 330 core

// Task 16: Create a UV coordinate in variable
in vec2 UV;

// Task 8: Add a sampler2D uniform
uniform sampler2D tex;
// Task 29: Add a bool on whether or not to filter the texture
uniform bool invert;
uniform bool sharpen;
uniform bool grayScale;
uniform bool blur;
uniform bool sepia;
uniform bool edgeDetection;

out vec4 fragColor;

void main()
{
    fragColor = vec4(1);
    // Task 17: Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(tex, UV);

    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
    if (invert) {
        fragColor = vec4(1.0) - fragColor;
    }

    if (grayScale) {
        float avg = fragColor[0]+fragColor[1]+fragColor[2]/3;
        fragColor = vec4(avg, avg, avg, fragColor[3]);
    }
    if (sepia) {
        float red = (fragColor[0] * .393) + (fragColor[1] *.769) + (fragColor[2] * .189);
        float green = (fragColor[0] * .349) + (fragColor[1] *.686) + (fragColor[2] * .168);
        float blue = (fragColor[0] * .272) + (fragColor[1] *.534) + (fragColor[2] * .131);
        fragColor = vec4(red, green, blue, 1.0);
    }
    if (sharpen) {
        mat3 sharpenKernel = mat3(-1.0, -1.0, -1.0,
                                      -1.0,  17.0, -1.0,
                                      -1.0, -1.0, -1.0);
        vec3 color = vec3(0.0);

        // Convert pixel offset to value between 0 and 1 which is what uv needs
        vec2 texOffset = 1.0 / textureSize(tex, 0); //textureSize gets dimensions of texture, 0 because that is mipmap level

        // Loop through kernel
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                // Sample pixels around current pixel
                vec2 sampleCoord = UV + vec2(i, j) * texOffset;

                // Get color of sampled pixel
                vec3 sampleColor = texture(tex, sampleCoord).rgb;

                if (invert) {
                    sampleColor = vec3(1.0) - sampleColor;
                }
                if (grayScale) {
                    float avg = sampleColor[0]+sampleColor[1]+sampleColor[2]/3;
                    sampleColor = vec3(avg, avg, avg);
                }
                if (sepia) {
                    float red = (sampleColor[0] * .393) + (sampleColor[1] *.769) + (sampleColor[2] * .189);
                    float green = (sampleColor[0] * .349) + (sampleColor[1] *.686) + (sampleColor[2] * .168);
                    float blue = (sampleColor[0] * .272) + (sampleColor[1] *.534) + (sampleColor[2] * .131);
                    sampleColor = vec3(red, green, blue);
                }

                // Multiply color by kernel at correct index
                color += sampleColor * sharpenKernel[i + 1][j + 1];
            }
        }

        color /= 9.0;
        fragColor = vec4(color, 1.0);
    }
    if (edgeDetection) {
        mat3 kernelX = mat3(-1.0, 0.0, 1.0,
                            -2.0, 0.0, 2.0,
                            -1.0, 0.0, 1.0);

        mat3 kernelY = mat3(-1.0, -2.0, -1.0,
                            0.0, 0.0, 0.0,
                            1.0, 2.0, 1.0);

        vec3 GX = vec3(0.0);
        vec3 GY = vec3(0.0);

        vec2 texOffset = 1.0 / textureSize(tex, 0);

        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                vec2 sampleCoord = UV + vec2(i, j) * texOffset;
                vec3 sampleColor = texture(tex, sampleCoord).rgb;

                if (invert) {
                    sampleColor = vec3(1.0) - sampleColor;
                }
                if (grayScale) {
                    float avg = sampleColor[0]+sampleColor[1]+sampleColor[2]/3;
                    sampleColor = vec3(avg, avg, avg);
                }
                if (sepia) {
                    float red = (sampleColor[0] * .393) + (sampleColor[1] *.769) + (sampleColor[2] * .189);
                    float green = (sampleColor[0] * .349) + (sampleColor[1] *.686) + (sampleColor[2] * .168);
                    float blue = (sampleColor[0] * .272) + (sampleColor[1] *.534) + (sampleColor[2] * .131);
                    sampleColor = vec3(red, green, blue);
                }

                GX += sampleColor * kernelX[i + 1][j + 1];
                GY += sampleColor * kernelY[i + 1][j + 1];
            }
        }
        float magnitude = sqrt(dot(GX, GX)+dot(GY, GY));
        fragColor = vec4(vec3(magnitude), 1.0);
    }
    if (blur) {
        vec3 color = vec3(0.0);

        vec2 texOffset = 1.0 / textureSize(tex, 0);

        for (int i = -2; i <= 2; ++i) {
            for (int j = -2; j <= 2; ++j) {
                vec2 sampleCoord = UV + vec2(i, j) * texOffset;
                vec3 sampleColor = texture(tex, sampleCoord).rgb;

                if (invert) {
                    sampleColor = vec3(1.0) - sampleColor;
                }
                if (grayScale) {
                    float avg = sampleColor[0]+sampleColor[1]+sampleColor[2]/3;
                    sampleColor = vec3(avg, avg, avg);
                }
                if (sepia) {
                    float red = (sampleColor[0] * .393) + (sampleColor[1] *.769) + (sampleColor[2] * .189);
                    float green = (sampleColor[0] * .349) + (sampleColor[1] *.686) + (sampleColor[2] * .168);
                    float blue = (sampleColor[0] * .272) + (sampleColor[1] *.534) + (sampleColor[2] * .131);
                    sampleColor = vec3(red, green, blue);
                }

                color += sampleColor;
            }
        }
        color /= 25.0;
        fragColor = vec4(color, 1.0);
    }
}

