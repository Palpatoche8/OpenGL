#version 330 core
in  vec4 vColor;
out vec4 fragColor;
void main() {
    vec2  uv   = gl_PointCoord * 2.0 - 1.0;
    float dist = length(uv);
    if (dist > 1.0) discard;
    float soft = 1.0 - smoothstep(0.0, 1.0, dist);
    soft = soft * soft;
    fragColor = vec4(vColor.rgb, vColor.a * soft);
}
