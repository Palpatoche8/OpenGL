#version 330 core

in float vAge;
in vec4  vColor;

out vec4 fragColor;

void main() {
    vec2 uv = gl_PointCoord * 2.0 - 1.0;
    float dist = length(uv);

    if (dist > 1.0) discard;

    float softness = 1.0 - smoothstep(0.0, 1.0, dist);
    softness = pow(softness, 1.4);

    fragColor = vColor * softness;
}
