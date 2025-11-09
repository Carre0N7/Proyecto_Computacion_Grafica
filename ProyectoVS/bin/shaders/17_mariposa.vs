// shaders/butterfly.vs
#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
// Entradas de Huesos (Bones)
layout (location = 5) in vec4  bIDs1;
layout (location = 6) in vec4  bIDs2;
layout (location = 7) in vec4  bIDs3;
layout (location = 8) in vec4  bWeights1;
layout (location = 9) in vec4  bWeights2;
layout (location = 10) in vec4 bWeights3;

// Salidas (para el shader de iluminación)
out vec2 TexCoords;
out vec3 vertexPosition_cameraspace;
out vec3 Normal_cameraspace;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[100]; // Matriz de huesos

void main()
{
    // Lógica de Skinning (huesos)
    mat4 BoneTransform = gBones[int(bIDs1[0])] * bWeights1[0];
    BoneTransform += gBones[int(bIDs1[1])] * bWeights1[1];
    BoneTransform += gBones[int(bIDs1[2])] * bWeights1[2];  
    BoneTransform += gBones[int(bIDs1[3])] * bWeights1[3];
    BoneTransform += gBones[int(bIDs2[0])] * bWeights2[0];
    BoneTransform += gBones[int(bIDs2[1])] * bWeights2[1];
    BoneTransform += gBones[int(bIDs2[2])] * bWeights2[2]; 
    BoneTransform += gBones[int(bIDs2[3])] * bWeights2[3];
    BoneTransform += gBones[int(bIDs3[0])] * bWeights3[0];
    BoneTransform += gBones[int(bIDs3[1])] * bWeights3[1];
    BoneTransform += gBones[int(bIDs3[2])] * bWeights3[2]; 
    BoneTransform += gBones[int(bIDs3[3])] * bWeights3[3];

    // Aplicamos la transformación de huesos a la posición y normal
    vec4 PosL = BoneTransform * vec4(aPos, 1.0f);
    vec4 NormL = vec4(mat3(BoneTransform) * aNormal, 0.0);

    // Salidas (igual que tu 11_PhongShaderMultLights.vs)
    gl_Position = projection * view * model * PosL;
    TexCoords = aTexCoords;
    vertexPosition_cameraspace = ( view * model * PosL).xyz;
    Normal_cameraspace = ( view * model * NormL).xyz;
}