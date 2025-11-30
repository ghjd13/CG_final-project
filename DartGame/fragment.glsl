#version 330 core
in vec3 FragPos; 
in vec2 TexCoord;
in mat3 TBN;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

// 텍스처 샘플러
uniform sampler2D texture_diffuse; // slot 0
uniform sampler2D texture_normal;  // slot 1

// Tint: baseColor와 objectColor를 mix함
uniform vec3 objectColor;
uniform float tintFactor; // 0.0 = 원래 텍스처, 1.0 = objectColor 완전 대체

void main ()
{
    // 노말 맵 읽기 및 변환
    vec3 normal = texture(texture_normal, TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    // 색상 & tint
    vec3 baseColor = texture(texture_diffuse, TexCoord).rgb;
    vec3 color = mix(baseColor, objectColor, clamp(tintFactor, 0.0, 1.0));

    // 조명 계산
    vec3 ambient = 0.1 * color;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * color;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * lightColor * 0.5;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}