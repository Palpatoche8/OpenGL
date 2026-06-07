#version 330 core

layout(location = 0) in vec3  aPos;
layout(location = 1) in float aAge;   // 0=fresh, 1=dying

uniform mat4  uMVP;
uniform float uPointScale;   // screen_h / (2*tan(fov/2))

out vec4 vColor;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);

    float size   = mix(0.28, 0.05, aAge);
    gl_PointSize = max(size * uPointScale / gl_Position.w, 1.0);

    vec3 color;
    if (aAge < 0.25)
        color = mix(vec3(1.0, 1.0, 0.9),  vec3(1.0, 0.95, 0.2),  aAge / 0.25);
    else if (aAge < 0.55)
        color = mix(vec3(1.0, 0.95, 0.2), vec3(1.0, 0.35, 0.02), (aAge - 0.25) / 0.30);
    else if (aAge < 0.80)
        color = mix(vec3(1.0, 0.35, 0.02),vec3(0.6, 0.04, 0.0),  (aAge - 0.55) / 0.25);
    else
        color = mix(vec3(0.6, 0.04, 0.0), vec3(0.0, 0.0, 0.0),   (aAge - 0.80) / 0.20);

    float alpha = (1.0 - aAge) * (1.0 - aAge);
    vColor = vec4(color * alpha, alpha);
}
