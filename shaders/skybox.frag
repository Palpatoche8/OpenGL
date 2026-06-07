#version 330 core
in  vec3 vDir;
out vec4 fragColor;
float hash(vec3 p) {
    p = fract(p * vec3(443.897, 397.297, 491.187));
    p += dot(p.zxy, p.yxz + 19.19);
    return fract(p.x * p.y * p.z);
}
float starField(vec3 dir) {
    vec3 p = normalize(dir) * 120.0;
    vec3 i = floor(p);
    float h = hash(i);
    if (h > 0.992) {
        vec3  f = fract(p);
        float d = length(f - 0.5) * 2.0;
        return (1.0 - smoothstep(0.0, 1.0, d)) * ((h - 0.992) / 0.008) * 2.5;
    }
    return 0.0;
}
void main() {
    vec3 dir = normalize(vDir);
    float horizon = clamp(dir.y * 1.8 + 0.05, 0.0, 1.0);
    vec3 sky = mix(vec3(0.02, 0.05, 0.12), vec3(0.00, 0.01, 0.03), horizon);
    float s = (dir.y > 0.0) ? starField(dir) : 0.0;
    sky += vec3(0.9, 0.95, 1.0) * s;
    float fireGlow = max(0.0, 0.18 - dir.y * 0.6) * (1.0 - abs(dir.x) * 0.5);
    sky += vec3(0.8, 0.25, 0.02) * fireGlow * 1.2;
    fragColor = vec4(sky, 1.0);
}
