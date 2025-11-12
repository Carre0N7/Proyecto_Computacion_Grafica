/*
*
* Proyecto Base Galería Estática
*
*/

#include <iostream>
#include <stdlib.h>
#include <vector> // Asegúrate de tener esta (para las luces)
#include <string> // Y esta (para los helpers de luces)
#include <sstream> // Y esta (para los helpers de luces)

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Clases de carga (Headers de tu proyecto)
#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <animatedmodel.h>
#include <material.h>
#include <light.h>
#include <cubemap.h>

// Funciones
bool Start();
bool Update();

// Definición de callbacks
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Globales
GLFWwindow *window;

// Tamaño de la ventana
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 600;

// Definición de cámara (posición inicial en XYZ)
Camera camera(glm::vec3(0.0f, 1.72f, 30.0f));

// Controladores para el movimiento del mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Variables de tiempo
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Variables puertas
float der = 0.0f;
float izq = 0.0f;

// --- AÑADIDO: Constantes de Velocidad ---
const float VELOCIDAD_NORMAL = 5.0f;
const float VELOCIDAD_SPRINT = 20.0f;

// Shaders
Shader *staticLightShader; // Renombrado de mLightsShader para más claridad
Shader *cubemapShader;
Shader* butterflyShader;
//Shader* dynamicShader;

// Modelos
Model *paredes; // Tu modelo de galería estática
Model* piso;
Model *plano;
Model* letras;
Model* masetasinv;
Model* mesassillascafe;
Model* mueblescafe;
Model* electrodomesticos;
Model* adiccafe;
Model* consumibles;
Model* mueblessala;
Model* plantasaxo;
Model* plantasinv;
Model* pinturas;
Model* pinturas2;
Model* esculturas;
Model* lampstecho;
Model* terrario;
Model* puertaprind;
Model* puertaprini;
AnimatedModel* butterfly;
AnimatedModel* colibri;
AnimatedModel* conejo;
//AnimatedModel* iguana;
AnimatedModel* serpiente;

// Cubemap (fondo)
CubeMap *mainCubeMap;

// Luces
std::vector<Light> gLights;

// Materiales (puedes definir más si tu galería usa varios)
Material materialPrincipal;
Material materialPiso;
Material materialMueblesSala;

// --- Funciones de ayuda para luces (Copiadas de tu práctica) ---
void SetLightUniformInt(Shader *shader, const char* propertyName, size_t lightIndex, int value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	shader->setInt(uniformName.c_str(), value);
}
void SetLightUniformFloat(Shader *shader, const char* propertyName, size_t lightIndex, float value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	shader->setFloat(uniformName.c_str(), value);
}
void SetLightUniformVec4(Shader *shader, const char* propertyName, size_t lightIndex, glm::vec4 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	shader->setVec4(uniformName.c_str(), value);
}
void SetLightUniformVec3(Shader *shader, const char* propertyName, size_t lightIndex, glm::vec3 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	shader->setVec3(uniformName.c_str(), value);
}
// --- Fin de funciones de ayuda ---


// Entrada a función principal
int main()
{
	if (!Start())
		return -1;

	/* Loop principal de renderizado */
	while (!glfwWindowShouldClose(window))
	{
		if (!Update())
			break;
	}

	glfwTerminate();
	return 0;
}

bool Start() {
	// Inicialización de GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creación de la ventana
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto InvernArte", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Capturar el cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Carga de GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// Activación de buffer de profundidad
	glEnable(GL_DEPTH_TEST);
	

	// Compilación de shaders (¡Necesitaré estos archivos!)
	staticLightShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");
	cubemapShader = new Shader("shaders/10_vertex_cubemap.vs", "shaders/10_fragment_cubemap.fs");
	//dynamicShader = new Shader("shaders/10_skinning-IT.vs");
	butterflyShader = new Shader("shaders/17_mariposa.vs", "shaders/17_mariposa.fs");
	butterflyShader->setBonesIDs(MAX_RIGGING_BONES); // ¡Importante!


	// Carga del modelo de la galería
	// Asegúrate que la ruta sea correcta dentro de tu carpeta 'bin'
	paredes = new Model("models/Paredes.fbx"); // <-- CAMBIA "galeria.fbx" POR EL NOMBRE DE TU ARCHIVO
	piso = new Model("models/Piso.fbx");
	plano = new Model("models/Plano.fbx");
	letras = new Model("models/Letras.fbx");
	masetasinv = new Model("models/MasetasInv.fbx");
	mesassillascafe = new Model("models/MesasSillasCafe.fbx");
	mueblescafe = new Model("models/MueblesCafe.fbx");
	electrodomesticos = new Model("models/Electrodomesticos.fbx");
	adiccafe = new Model("models/AdicCafe.fbx");
	consumibles = new Model("models/Consumibles.fbx");
	mueblessala = new Model("models/MueblesSala.fbx");
	plantasaxo = new Model("models/PlantasAxolot.fbx");
	plantasinv = new Model("models/PlantasInv.fbx");
	pinturas = new Model("models/Pinturas.fbx");
	pinturas2 = new Model("models/Pinturas2.fbx");
	esculturas = new Model("models/Esculturas.fbx");
	lampstecho = new Model("models/LampsTecho.fbx"); 
	terrario = new Model("models/Terrario.fbx");
	puertaprind = new Model("models/PuertaPrinD.fbx");
	puertaprini = new Model("models/PuertaPrinI.fbx");
	butterfly = new AnimatedModel("models/Mariposa.fbx");
	colibri = new AnimatedModel("models/Colibri.fbx");
	conejo = new AnimatedModel("models/Conejo.fbx");
	//iguana = new AnimatedModel("models/Iguana.fbx");
	serpiente = new AnimatedModel("models/Serpiente.fbx");

	// Carga del Cubemap (fondo)
	vector<std::string> faces
	{
		"textures/cubemap/01/posx.png",
		"textures/cubemap/01/negx.png",
		"textures/cubemap/01/posy.png",
		"textures/cubemap/01/negy.png",
		"textures/cubemap/01/posz.png",
		"textures/cubemap/01/negz.png"
		// Asegúrate que estas texturas existan en 'bin/textures/...'
	};
	mainCubeMap = new CubeMap();
	mainCubeMap->loadCubemap(faces);

	// Configuración de luces (puedes ajustar esto como necesites)
	/*
	Light light01;
	light01.Position = glm::vec3(5.0f, 2.0f, 5.0f);
	light01.Color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	gLights.push_back(light01);
	
	Light light02;
	light02.Position = glm::vec3(-5.0f, 2.0f, 5.0f);
	light02.Color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	gLights.push_back(light02);
	*/

	Light light_sol;
	light_sol.Position = glm::vec3(0.0f, 50.0f, 0.0f);   // Posición MUY ALTA, en el cielo
	light_sol.Direction = glm::vec3(0.0f, -1.0f, 0.0f);  // Apuntando directo hacia abajo
	light_sol.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Un color cálido, ligeramente amarillo
	light_sol.Power = glm::vec4(1.2f, 1.2f, 1.2f, 1.0f); // Un poco más de potencia que los focos
	light_sol.alphaIndex = 32;                           // Brillo especular
	light_sol.distance = 1.0f;                           // Mantenemos la misma atenuación
	gLights.push_back(light_sol);

	//_______LUCES SALA
	// Configuración de luces (puedes ajustar esto como necesites)
	Light light01;				//(X, Y, Z)
	light01.Position = glm::vec3(5.0f, 4.4f, 5.0f);   // Posición
	light01.Direction = glm::vec3(0.0f, -1.0f, 0.0f); // Dirección (apuntando hacia abajo)
	light01.Color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f); // Color (más brillante)
	light01.Power = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f); // Potencia
	light01.alphaIndex = 32;                          // Índice especular
	light01.distance = 2.0f;                         // <-- ARREGLADO
	gLights.push_back(light01);

	Light light02;
	light02.Position = glm::vec3(-5.0f, 4.4f, 5.0f);
	light02.Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	light02.Color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light02.Power = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light02.alphaIndex = 32;
	light02.distance = 2.0f;                       
	gLights.push_back(light02);

	Light light03;
	light03.Position = glm::vec3(5.0f, 4.4f, 15.0f);
	light03.Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	light03.Color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light03.Power = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light03.alphaIndex = 32;
	light03.distance = 2.0f;
	gLights.push_back(light03);

	Light light04;
	light04.Position = glm::vec3(-5.0f, 4.4f, 15.0f);
	light04.Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	light04.Color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light04.Power = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light04.alphaIndex = 32;
	light04.distance = 2.0f;
	gLights.push_back(light04);

	//_________Luces CAfe

	Light light05;
	light05.Position = glm::vec3(-18.0f, 4.4f, -5.0f);
	light05.Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	light05.Color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light05.Power = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light05.alphaIndex = 32;
	light05.distance = 2.0f;
	gLights.push_back(light05);

	Light light06;
	light06.Position = glm::vec3(-18.0f, 4.4f, -10.0f);
	light06.Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	light06.Color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light06.Power = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light06.alphaIndex = 32;
	light06.distance = 2.0f;
	gLights.push_back(light06);

	//---MATERIAL Principal
	materialPrincipal.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	materialPrincipal.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	materialPrincipal.specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	materialPrincipal.transparency = 1.0f;

	//---MATERIAL Piso
	materialPiso.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	materialPiso.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f); // Usa el color claro de tu madera
	materialPiso.specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // ¡CERO BRILLO!
	materialPiso.transparency = 1.0f;

	// --- AÑADIDO: Configurar velocidad inicial de la cámara ---
	camera.MovementSpeed = VELOCIDAD_NORMAL;

	//---MATERIAL Muebles de la sala
	materialMueblesSala.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	materialMueblesSala.diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // Un gris medio para la tela
	materialMueblesSala.specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // ¡CERO BRILLO!
	materialMueblesSala.transparency = 1.0f;

	return true;
}


bool Update() {
	// Cálculo del framerate
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Procesar entrada
	processInput(window);

	// --- AÑADIDO: Actualizar la animación de la mariposa ---
	butterfly->UpdateAnimation(deltaTime);
	colibri->UpdateAnimation(deltaTime);
	conejo->UpdateAnimation(deltaTime); 
	//iguana->UpdateAnimation(deltaTime);
	serpiente->UpdateAnimation(deltaTime);

	// Limpiar buffers
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Matrices de Vista y Proyección (solo cámara en 1ra persona)
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
	glm::mat4 view = camera.GetViewMatrix();

	// 1. Dibujar el Cubemap (fondo)
	{
		mainCubeMap->drawCubeMap(*cubemapShader, projection, view);
	}

	// 2. Dibujar la Galería (Modelo Estático)
	{
		staticLightShader->use();

		// Activar transparencias
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		staticLightShader->setMat4("projection", projection);
		staticLightShader->setMat4("view", view);

		/*
		// Configuración de luces
		staticLightShader->setInt("numLights", (int)gLights.size());
		for (size_t i = 0; i < gLights.size(); ++i) {
			SetLightUniformVec3(staticLightShader, "Position", i, gLights[i].Position);
			SetLightUniformVec4(staticLightShader, "Color", i, gLights[i].Color);
			// ... (puedes añadir más propiedades de luz si tu shader las usa)
		}
		*/
		// Configuración de luces
		staticLightShader->setInt("numLights", (int)gLights.size());
		for (size_t i = 0; i < gLights.size(); ++i) {
			SetLightUniformVec3(staticLightShader, "Position", i, gLights[i].Position);
			SetLightUniformVec3(staticLightShader, "Direction", i, gLights[i].Direction);
			SetLightUniformVec4(staticLightShader, "Color", i, gLights[i].Color);
			SetLightUniformVec4(staticLightShader, "Power", i, gLights[i].Power);
			SetLightUniformInt(staticLightShader, "alphaIndex", i, gLights[i].alphaIndex);
			SetLightUniformFloat(staticLightShader, "distance", i, gLights[i].distance);
		}

		staticLightShader->setVec3("eye", camera.Position);

		// Aplicamos propiedades materiales (Opcional, si el modelo no las tiene)
		staticLightShader->setVec4("MaterialAmbientColor", materialPrincipal.ambient);
		staticLightShader->setVec4("MaterialDiffuseColor", materialPrincipal.diffuse);
		staticLightShader->setVec4("MaterialSpecularColor", materialPrincipal.specular);
		staticLightShader->setFloat("transparency", materialPrincipal.transparency);

		// Modelo Paredes
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			paredes->Draw(*staticLightShader); //¡Dibujamos la galería!

		}
		// Modelo Puerta Derecha
		{
			// ¡¡ Matriz 'model' NUEVA para esta puerta !!
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 19.85f)); // Posición base
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación base
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala base

			// Ahora aplicamos el movimiento dinámico (solo en X)
			model = glm::translate(model, glm::vec3(der, 0.0f, 0.0f));

			staticLightShader->setMat4("model", model);
			puertaprind->Draw(*staticLightShader);
		} 

		// Modelo Puerta Izquierda
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 19.85f)); // Posición base
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación base
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala base

			// Ahora aplicamos el movimiento dinámico (solo en X)
			model = glm::translate(model, glm::vec3(izq, 0.0f, 0.0f));

			staticLightShader->setMat4("model", model);
			puertaprini->Draw(*staticLightShader);
		}
		// Modelo Piso
		{
			staticLightShader->setVec4("MaterialAmbientColor", materialPiso.ambient);
			staticLightShader->setVec4("MaterialDiffuseColor", materialPiso.diffuse);
			staticLightShader->setVec4("MaterialSpecularColor", materialPiso.specular);
			staticLightShader->setFloat("transparency", materialPiso.transparency);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			piso->Draw(*staticLightShader); //¡Dibujamos la galería!
		}

		staticLightShader->setVec4("MaterialAmbientColor", materialPrincipal.ambient);
		staticLightShader->setVec4("MaterialDiffuseColor", materialPrincipal.diffuse);
		staticLightShader->setVec4("MaterialSpecularColor", materialPrincipal.specular);
		staticLightShader->setFloat("transparency", materialPrincipal.transparency);

		//Modelo Plano
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			plano->Draw(*staticLightShader); 
		}
		//Modelo Letras
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			letras->Draw(*staticLightShader);
		}
		//Modelo MasetasInv
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			masetasinv->Draw(*staticLightShader);
		}
		//Modelo MesasSillasCafe
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			mesassillascafe->Draw(*staticLightShader);
		}
		//Modelo Muebles Cafe
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			mueblescafe->Draw(*staticLightShader);
		}
		//Modelo Electrodomesticos
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			electrodomesticos->Draw(*staticLightShader);
		}
		//Modelo Adicionales de la cafe
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			adiccafe->Draw(*staticLightShader);
		}
		//Modelo Consumibles Cafe(donas, vasos)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			consumibles->Draw(*staticLightShader);
		}
		//Modelo Muebles Sala
		{
			staticLightShader->setVec4("MaterialAmbientColor", materialMueblesSala.ambient);
			staticLightShader->setVec4("MaterialDiffuseColor", materialMueblesSala.diffuse);
			staticLightShader->setVec4("MaterialSpecularColor", materialMueblesSala.specular);
			staticLightShader->setFloat("transparency", materialMueblesSala.transparency);
			
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			mueblessala->Draw(*staticLightShader);
		}
		//Modelo Plantas Axolotario
		{
			staticLightShader->setVec4("MaterialAmbientColor", materialPrincipal.ambient);
			staticLightShader->setVec4("MaterialDiffuseColor", materialPrincipal.diffuse);
			staticLightShader->setVec4("MaterialSpecularColor", materialPrincipal.specular);
			staticLightShader->setFloat("transparency", materialPrincipal.transparency);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			plantasaxo->Draw(*staticLightShader);
		}
		//Modelo Plantas Invernadero
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			plantasinv->Draw(*staticLightShader);
		}
		//Modelo Pinturas
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			pinturas->Draw(*staticLightShader);
		}
		//Modelo Pinturas2
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			pinturas2->Draw(*staticLightShader);
		}
		//Modelo Esculturas
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			esculturas->Draw(*staticLightShader);
		}
		//Modelo Lamps Techo
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			lampstecho->Draw(*staticLightShader);
		}
		//Modelo Terrario
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición en el mundo
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// Escala
			staticLightShader->setMat4("model", model);

			terrario->Draw(*staticLightShader);
		}

		
		//Modelo Mariposa Animada (Y COLIBRÍ)
		{
			// ¡Usamos el shader de animación!
			butterflyShader->use();
			glDisable(GL_BLEND);

			// --- ENVIAMOS UNIFORMES COMUNES (para ambos modelos) ---
			// Cámara
			butterflyShader->setMat4("projection", projection);
			butterflyShader->setMat4("view", view);
			butterflyShader->setVec3("eye", camera.Position);

			// Luces
			butterflyShader->setInt("numLights", (int)gLights.size());
			for (size_t i = 0; i < gLights.size(); ++i) {
				SetLightUniformVec3(butterflyShader, "Position", i, gLights[i].Position);
				SetLightUniformVec3(butterflyShader, "Direction", i, gLights[i].Direction);
				SetLightUniformVec4(butterflyShader, "Color", i, gLights[i].Color);
				SetLightUniformVec4(butterflyShader, "Power", i, gLights[i].Power);
				SetLightUniformInt(butterflyShader, "alphaIndex", i, gLights[i].alphaIndex);
				SetLightUniformFloat(butterflyShader, "distance", i, gLights[i].distance);
			}

			// Material (usamos el principal para ambos)
			butterflyShader->setVec4("MaterialAmbientColor", materialPrincipal.ambient);
			butterflyShader->setVec4("MaterialDiffuseColor", materialPrincipal.diffuse);
			butterflyShader->setVec4("MaterialSpecularColor", materialPrincipal.specular);
			butterflyShader->setFloat("transparency", materialPrincipal.transparency);


			// --- DIBUJAR MARIPOSA (Con su movimiento) ---
			{
				// 1. Parámetros del círculo
				float radius = 5.0f;
				float speed = 0.5f;
				glm::vec3 center = glm::vec3(0.0f, 3.0f, -20.0f);

				// 2. Calcular posición basada en el tiempo
				float time = (float)glfwGetTime();
				float angle = time * speed;
				float newX = center.x + radius * cos(angle);
				float newZ = center.z + radius * sin(angle);
				glm::vec3 newPos = glm::vec3(newX, center.y, newZ);

				// 3. Calcular orientación
				float yaw = -angle + glm::radians(0.0f);

				// 4. Construir la matriz de modelo
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, newPos);
				model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
				butterflyShader->setMat4("model", model);

				// 5. Enviar Huesos (Bones)
				butterflyShader->setMat4("gBones", MAX_RIGGING_BONES, butterfly->gBones);

				// 6. ¡Dibujar!
				butterfly->Draw(*butterflyShader);
			}

			// --- AÑADIDO: DIBUJAR COLIBRI (Estático por ahora) ---
			{
				// 1. Construir la matriz de modelo (estática)
				// Usamos las variables globales que ya tenías
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(8.0f, 0.7f, -16.6f)); // <-- Posición estática
				model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f)); // <-- AJUSTA LA ESCALA (quizás es diferente a la mariposa)
				butterflyShader->setMat4("model", model);

				// 2. Enviar Huesos (Bones)
				butterflyShader->setMat4("gBones", MAX_RIGGING_BONES, colibri->gBones);

				// 3. ¡Dibujar!
				colibri->Draw(*butterflyShader);
			}
			// --- AÑADIDO: DIBUJAR Jaguar (Estático por ahora) ---
			{
				// 1. Construir la matriz de modelo (estática)
				// Usamos las variables globales que ya tenías
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-3.5f, 0.25f, -7.0f)); // <-- Posición estática
				model = glm::scale(model, glm::vec3(0.0001f, 0.0001f, 0.0001f)); // <-- AJUSTA LA ESCALA (quizás es diferente a la mariposa)
				model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				butterflyShader->setMat4("model", model);

				// 2. Enviar Huesos (Bones)
				butterflyShader->setMat4("gBones", MAX_RIGGING_BONES, conejo->gBones);

				// 3. ¡Dibujar!
				conejo->Draw(*butterflyShader);
			}
			/*
			// --- AÑADIDO: DIBUJAR Jaguar (Estático por ahora) ---
			{
				// 1. Construir la matriz de modelo (estática)
				// Usamos las variables globales que ya tenías
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f)); // <-- Posición estática
				model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f)); // <-- AJUSTA LA ESCALA (quizás es diferente a la mariposa)
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				butterflyShader->setMat4("model", model);

				// 2. Enviar Huesos (Bones)
				butterflyShader->setMat4("gBones", MAX_RIGGING_BONES, iguana->gBones);

				// 3. ¡Dibujar!
				iguana->Draw(*butterflyShader);
			}*/

			{
				// 1. Construir la matriz de modelo (estática)
				// Usamos las variables globales que ya tenías
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(2.5f, 0.2f, -5.0f)); // <-- Posición estática
				model = glm::scale(model, glm::vec3(0.0001f, 0.0001f, 0.0001f)); // <-- AJUSTA LA ESCALA (quizás es diferente a la mariposa)
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				butterflyShader->setMat4("model", model);

				// 2. Enviar Huesos (Bones)
				butterflyShader->setMat4("gBones", MAX_RIGGING_BONES, serpiente->gBones);

				// 3. ¡Dibujar!
				serpiente->Draw(*butterflyShader);
			}
		}

	}

	glUseProgram(0);

	// Swap buffers y poll events
	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

// Procesamos entradas del teclado
void processInput(GLFWwindow* window)
{
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.MovementSpeed = VELOCIDAD_SPRINT;
	else
		camera.MovementSpeed = VELOCIDAD_NORMAL;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		if (der < 2.0f) {
			der += 0.01f;
			izq -= 0.01f;
		}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		if (der > 0.0f) {
			der -= 0.01f;
			izq += 0.01f;
		}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	// Modos de dibujado
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Relleno
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // Puntos
}

// Callback: Actualizar viewport si la ventana cambia de tamaño
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Callback: Movimiento del mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // Invertido

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// Callback: Scroll del mouse (Zoom)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}