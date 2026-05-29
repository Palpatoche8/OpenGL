#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in float aAge;  
uniform vec2 uResolution;

out float vAge;
out vec4  vColor;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);

    float size = mix(28.0, 4.0, aAge) * (uResolution.y / 600.0);
    gl_PointSize = max(size, 1.0);


    vec3 color;                                                                              // White
    if (aAge < 0.25) {
        color = mix(vec3(1.0, 1.0, 0.9), vec3(1.0, 0.95, 0.2), aAge / 0.25);                 // Yellow
    } else if (aAge < 0.55) {                                                           
        color = mix(vec3(1.0, 0.95, 0.2), vec3(1.0, 0.35, 0.02), (aAge - 0.25) / 0.30);     // orange
    } else if (aAge < 0.80) {
        color = mix(vec3(1.0, 0.35, 0.02), vec3(0.6, 0.04, 0.0), (aAge - 0.55) / 0.25);      // red
    } else {
        color = mix(vec3(0.6, 0.04, 0.0), vec3(0.0, 0.0, 0.0), (aAge - 0.80) / 0.20);         // black
    }

    float alpha = (1.0 - aAge) * (1.0 - aAge);

    vColor = vec4(color * alpha, alpha);
    vAge   = aAge;
}
