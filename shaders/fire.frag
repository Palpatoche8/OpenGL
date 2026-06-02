#version 330 core

uniform sampler2D texture_sampler;

in float vAge;
in vec4  vColor;

out vec4 fragColor;

void main() {
    vec2 uv = gl_PointCoord * 2.0 - 1.0;
    vec4 c1 = texture2D(texture_sampler, gl_PointCoord);
    float dist = length(uv);

    if (dist > 1.0) discard;

    float softness = 1.0 - smoothstep(0.0, 1.0, dist);
    softness = pow(softness, 0.5);

    // fragColor = vColor * softness;
    fragColor = c1 * softness * vColor * softness;
}
