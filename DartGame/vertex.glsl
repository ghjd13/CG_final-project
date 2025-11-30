#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent; // [New] Tangent 입력

out vec3 FragPos; 
out vec2 TexCoord;
out mat3 TBN; // [New] 프래그먼트 쉐이더로 보낼 TBN 행렬

uniform mat4 model;      
uniform mat4 view;       
uniform mat4 projection; 

void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    FragPos = vec3(model * vec4(vPos, 1.0));
    TexCoord = vTexCoord;

    // [New] 노말 맵핑을 위한 TBN 행렬 구성
    // 1. Tangent와 Normal을 월드 공간으로 변환
    vec3 T = normalize(vec3(model * vec4(vTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(vNormal, 0.0)));
    
    // 2. Gram-Schmidt process (T와 N을 수직으로 재정렬)
    T = normalize(T - dot(T, N) * N);
    
    // 3. Bitangent 계산 (N과 T의 외적)
    vec3 B = cross(N, T);
    
    // 4. TBN 행렬 완성
    TBN = mat3(T, B, N);
}