#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;

uniform vec3 uLightPos;
uniform vec3 uColor;

out vec4 fragColor;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vWorldPos);

    float diffuse = max(dot(N, L), 0.0);

    vec3 ambient = 0.25 * uColor;
    vec3 color = ambient + diffuse * uColor;

    fragColor = vec4(color, 1.0);
}