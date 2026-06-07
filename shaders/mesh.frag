#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

uniform vec3  uLightPos;
uniform vec3  uLightColor;
uniform float uLightIntensity;
uniform vec3  uCameraPos;
uniform vec3  uColor;

out vec4 fragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = uLightPos - vWorldPos;
    float dist = length(L);
    L = normalize(L);

    // Inverse-square attenuation
    float atten = uLightIntensity / (dist * dist + 0.5);

    float diff = max(dot(N, L), 0.0);

    // Specular (Blinn-Phong)
    vec3 V   = normalize(uCameraPos - vWorldPos);
    vec3 H   = normalize(L + V);
    float sp = pow(max(dot(N, H), 0.0), 32.0) * atten;

    vec3 ambient  = uColor * 0.04;
    vec3 diffuse  = uColor * diff * uLightColor * atten;
    vec3 specular = uLightColor * sp * 0.3;

    fragColor = vec4(ambient + diffuse + specular, 1.0);
}
