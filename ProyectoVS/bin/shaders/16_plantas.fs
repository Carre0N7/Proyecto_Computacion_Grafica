#version 330 core
out vec4 FragColor;

// --- Entradas (Vienen del Vertex Shader) ---
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// --- Estructura de Luz (Debe ser IDÉNTICA a la de tu C++) ---
// Asumo esta estructura basándome en tus llamadas SetLightUniform...
struct Light {
    vec3 Position;
    vec3 Direction;
    vec4 Color;     // Color base de la luz
    vec4 Power;     // (Intensidad Ambient, Diffuse, Specular, Shininess)
    int alphaIndex; // Para spotlight
    float distance; // Para atenuación (radio de la luz)
};

// --- Uniforms (Variables globales de C++) ---
uniform Light allLights[10]; // Asegúrate que '10' sea el tamaño correcto
uniform int numLights;

// Materiales (pasados por C++)
uniform vec4 MaterialAmbientColor;
uniform vec4 MaterialDiffuseColor;  // No lo usaremos, tomaremos el de la textura
uniform vec4 MaterialSpecularColor; // Color del brillo especular

uniform vec3 eye; // Posición de la cámara

// ¡¡LA TEXTURA DE LA PLANTA!!
uniform sampler2D texture_diffuse1; 

void main()
{
    // ===================================================================
    //  PASO 1: OBTENER EL COLOR DE LA TEXTURA Y HACER EL ALPHA TEST
    // ===================================================================

    // 1. Obtenemos el color de la textura (con su canal alfa)
    vec4 texColor = texture(texture_diffuse1, TexCoords);

    // 2. ¡EL RECORTE MÁGICO! (Alpha Testing)
    // Si el canal alfa (transparencia) es menor a un umbral (ej: 0.1),
    // este shader simplemente deja de ejecutarse para este píxel.
    if(texColor.a < 0.1)
        discard;

    // Si el código llega aquí, significa que el píxel SÍ es visible.

    // ===================================================================
    //  PASO 2: CALCULAR LA ILUMINACIÓN (Tu Phong)
    // ===================================================================

    // El 'Albedo' (color difuso base) ahora viene de la textura
    vec3 albedo = texColor.rgb;

    // Normalizamos los vectores necesarios
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(eye - FragPos);

    // El resultado final de la luz se acumulará aquí
    // Empezamos con la luz ambiental base del material (multiplicada por la textura)
    vec3 result = vec3(MaterialAmbientColor) * albedo;
    
    // --- Bucle de luces (Iteramos sobre todas las luces activas) ---
    for(int i = 0; i < numLights; i++)
    {
        // Vector de dirección de la luz (asumimos luz puntual)
        vec3 lightDir = normalize(allLights[i].Position - FragPos);

        // --- Atenuación ---
        // Calculamos la atenuación basada en la 'distance' (radio) de la luz
        // Esto hace que la luz se desvanezca suavemente al llegar a su radio máximo
        float realDistance = length(allLights[i].Position - FragPos);
        float attenuation = 1.0;
        if(allLights[i].distance > 0.0) { // Si la distancia es 0, la luz es infinita
             attenuation = 1.0 - smoothstep(allLights[i].distance * 0.8, allLights[i].distance, realDistance);
        }

        // --- Componente Ambiental de la luz ---
        // (La mayoría de las veces Power.x es 0.1 o 0.2)
        vec3 ambient = vec3(allLights[i].Power.x) * vec3(allLights[i].Color) * albedo;

        // --- Componente Difusa ---
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = vec3(allLights[i].Power.y) * diff * vec3(allLights[i].Color) * albedo;

        // --- Componente Especular (Blinn-Phong) ---
        vec3 halfwayDir = normalize(lightDir + viewDir);
        // Usamos Power.w como el 'shininess' (brillo)
        float spec = pow(max(dot(norm, halfwayDir), 0.0), allLights[i].Power.w); 
        // El color especular usa el color del *material* (MaterialSpecularColor)
        vec3 specular = vec3(allLights[i].Power.z) * spec * vec3(allLights[i].Color) * vec3(MaterialSpecularColor);
        
        // (Nota: No estoy implementando spotlight (Direction, alphaIndex) para mantenerlo simple)

        // Sumamos todo al resultado, multiplicado por la atenuación
        result += (ambient + diffuse + specular) * attenuation;
    }

    // El color final es el resultado de la iluminación.
    // El alfa es 1.0 (totalmente opaco) porque ya descartamos los transparentes.
    FragColor = vec4(result, 1.0);
}