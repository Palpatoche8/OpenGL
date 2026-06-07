#version 330 core
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;
uniform sampler2D uTexture;
uniform vec3  uCameraPos;
uniform vec3  uLightPos;
uniform vec3  uLightColor;
uniform float uLightIntensity;
out vec4 fragColor;
void main() {
    vec4 texColor = texture(uTexture, vTexCoord);
    vec3 albedo   = texColor.rgb;
    vec3  N    = normalize(vNormal);
    vec3  L    = uLightPos - vWorldPos;
    float dist = length(L);
    L = normalize(L);
    float atten = uLightIntensity / (1.0 + 0.5 * dist + 0.8 * dist * dist);
    float diff   = max(dot(N, L), 0.0);
    vec3  diffuse = uLightColor * diff * atten;
    vec3 V       = normalize(uCameraPos - vWorldPos);
    vec3 H       = normalize(L + V);
    float spec   = pow(max(dot(N, H), 0.0), 32.0);
    vec3 specular = uLightColor * spec * atten * 0.3;
    vec3 ambient = vec3(0.01, 0.015, 0.03);
    vec3 result = albedo * (ambient + diffuse) + specular;
    result += albedo * uLightColor * uLightIntensity * 0.015;
    fragColor = vec4(result, texColor.a);
}
