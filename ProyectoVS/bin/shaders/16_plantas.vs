#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
// Ignoramos tangent y bitangent (loc 3 y 4) porque no los necesitamos

// --- SALIDAS (para el planta.fs) ---
out vec3 FragPos;  // Posición en el Mundo (World Space)
out vec3 Normal;   // Normal en el Mundo (World Space)
out vec2 TexCoords; // Coordenadas UV

// --- Uniforms ---
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 1. Posición final en pantalla (obligatorio)
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // 2. Pasar la Posición en el Mundo (World Space) al Fragment Shader
    FragPos = vec3(model * vec4(aPos, 1.0));

    // 3. Pasar la Normal en el Mundo (World Space) al Fragment Shader
    // Se usa la transpuesta inversa para manejar escalado no uniforme
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // 4. Pasar las coordenadas de textura
    TexCoords = aTexCoords;
}