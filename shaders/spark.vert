#version 330 core

layout(location = 0) in vec3  aPos;
layout(location = 1) in float aAge;   // 0=fresh, 1=dying

uniform mat4  uMVP;
uniform float uPointScale;   // screen_h / (2*tan(fov/2))

out vec4 vColor;

void main() {
    gl_Position  = uMVP * vec4(aPos, 1.0);

    // Small, sharp, shrink as they age
    float size   = mix(0.04, 0.018, aAge);
    gl_PointSize = max(size * uPointScale / gl_Position.w, 1.0);

    // White-hot -> orange -> dark
    vec3 color;
    if (aAge < 0.3)
        color = mix(vec3(1.0, 1.0, 0.9), vec3(1.0, 0.6, 0.1), aAge / 0.3);
    else
        color = mix(vec3(1.0, 0.6, 0.1), vec3(0.5, 0.05, 0.0), (aAge - 0.3) / 0.7);

    float alpha = (1.0 - aAge) * (1.0 - aAge);
    vColor = vec4(color * alpha, alpha);
}
