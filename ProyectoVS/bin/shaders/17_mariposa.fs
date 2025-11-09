// shaders/butterfly.fs
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 vertexPosition_cameraspace;
in vec3 Normal_cameraspace;

// --- Uniforms (¡Idénticos a 11_PhongShaderMultLights.fs!) ---
uniform mat4 view;
uniform sampler2D texture_diffuse1;

uniform vec4 MaterialAmbientColor;
uniform vec4 MaterialDiffuseColor;
uniform vec4 MaterialSpecularColor;
uniform float transparency;

#define MAX_LIGHTS 10
uniform int numLights;
uniform struct Light {
   vec3  Position;
   vec3  Direction;
   vec4  Color;
   vec4  Power;
   int   alphaIndex;
   float distance;
} allLights[MAX_LIGHTS];

uniform vec3 eye;

// --- Lógica de Iluminación (Idéntica a 11_PhongShaderMultLights.fs) ---
vec4 ApplyLight(Light light, vec3 N, vec3 L, vec3 E) {
    vec4 K_a = MaterialAmbientColor * light.Color;
    float cosTheta = clamp( dot( N,L ), 0,1 );
    vec4 K_d = MaterialDiffuseColor * light.Color * cosTheta;
    vec3 R = reflect(-L,N);
    float cosAlpha = clamp( dot( E,R ), 0,1 );
    vec4 K_s = MaterialSpecularColor * light.Color * pow(cosAlpha,light.alphaIndex);
    
    // Tu lógica de atenuación corregida
    vec4 attenuation = light.Power / (light.distance * light.distance);
    vec4 l_contribution = K_a + (K_d * attenuation) + (K_s * attenuation);
    return l_contribution;
}

void main()
{    
    // --- 1. OBTENER TEXTURA Y HACER ALPHA TEST (de 16_plantas.fs) ---
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if(texColor.a < 0.1)
        discard; // ¡Recorta los píxeles transparentes!

    // --- 2. CALCULAR ILUMINACIÓN (de 11_PhongShaderMultLights.fs) ---
    vec3 n = normalize( Normal_cameraspace );
    vec4 ex_color = vec4(0.0f);

    for(int i = 0; i < numLights; ++i){
        vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
        vec3 LightPosition_cameraspace = ( view * vec4(allLights[i].Position,1)).xyz;
        vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
        vec3 e = normalize(EyeDirection_cameraspace);
        vec3 l = normalize( LightDirection_cameraspace );
        ex_color += ApplyLight(allLights[i], n, l, e);
    }
           
    ex_color.a = transparency;
    
    // Usamos texColor (que ya calculamos) en lugar de muestrear de nuevo
    FragColor = texColor * ex_color;
}