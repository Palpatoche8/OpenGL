#version 330 core

layout(location = 0) in vec3  aPos;
layout(location = 1) in float aAge;   // 0=fresh, 1=dying

uniform mat4  uMVP;
uniform float uPointScale;

out vec4 vColor;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);

    // Smoke puffs grow as they rise
    float size   = mix(0.35, 1.4, aAge);
    gl_PointSize = max(size * uPointScale / gl_Position.w, 1.0);

    // Dark grey, mostly transparent, fades out near death
    float alpha = (1.0 - aAge) * 0.18;
    float grey  = mix(0.25, 0.12, aAge);
    vColor = vec4(grey, grey, grey, alpha);
}
