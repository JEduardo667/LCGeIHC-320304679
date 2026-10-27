/*
Practica 9: Animacion compleja
- Nave de carreras que recorre la pista (rectas, curvas, rampas de subida y bajada)
  siguiendo la trayectoria de la pista
- Satelite con trayectoria no rectilinea y articulaciones automaticas (sin teclado)
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <string>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminacion
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animacion
bool avanza = true;

float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;
float movCoche = 0.0f;
float movOffset = 0.4f;
float rotLlanta = 0.0f;
float rotLlantaIncrementa = 5.0f;
float anguloHelice = 0.0f;
float velHelice = 20.0f;

//Limites del recorrido del rover
float limiteRoverMax = 292.0f;
float limiteRoverMin = -292.0f;

//Variables para la animacion del avion
const float PI_F = 3.14159265f;
float velAvion = 0.6f;
float radioGiro = 40.0f;
float longitudRecta = 100.0f;
float alturaAvion = 20.0f;
float orientacionModeloAvion = 90.0f;
int faseAvion = 0;
float progresoFase = 0.0f;
float inicioAvionX = 0.0f;
float inicioAvionZ = 40.0f;
float yawInicio = 0.0f;
float avionX = 0.0f;
float avionZ = 40.0f;
float avionYaw = 0.0f;


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>> Variables del satelite >>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//Trayectoria: recta desde X=-SAT_LIMITE, una vuelta de 360 grados al centro de la escena, recta hasta X=+SAT_LIMITE
//y regreso por la misma trayectoria (con un giro de 180 grados en cada extremo).
const float SAT_VELOCIDAD = 0.8f;       //unidades por deltaTime (igual que velAvion)
const float SAT_LIMITE = 290.0f;        //extremos del recorrido en X (el piso va de -300 a 300)
const float SAT_RADIO = 120.0f;         //radio de la vuelta de 360 grados (circulo centrado en el centro del piso)
const float SAT_ALTURA = 60.0f;         //altura de vuelo (por arriba de la pista y del avion)
const float SAT_TIEMPO_GIRO = 90.0f;    //duracion del giro de 180 grados en los extremos (en unidades de deltaTime)
const float SAT_YAW_MODELO = 0.0f;      //correccion en radianes si el modelo no mira hacia +X
float satDistancia = 0.0f;              //distancia recorrida sobre la trayectoria
int satFase = 0;                        //0 ida, 1 giro, 2 regreso, 3 giro
float satProgresoGiro = 0.0f;
float satX = -SAT_LIMITE;
float satZ = SAT_RADIO;
float satYaw = 0.0f;

//Articulaciones automaticas del satelite
const float SAT_SUP_VELOCIDAD = 1.5f;   //cuerpo superior: giro continuo sobre Y (grados por deltaTime)
const float SAT_ALA_MIN = -45.0f;       //paneles solares: vaiven sobre X entre MIN y MAX (grados)
const float SAT_ALA_MAX = 45.0f;
const float SAT_ALA_VELOCIDAD = 0.8f;   //grados por deltaTime
float satAngSup = 0.0f;
float satAngAla = 0.0f;
float satSentidoAla = 1.0f;

Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>> Declaracion de texturas >>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;

Texture naveTexture;
Texture fuegoTexture;

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>> Declaracion de modelos >>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

Model Rover_M;

// Rover por partes: cuerpo y brazo
Model rover_cuerpo;
Model rover_brazo_base;
Model rover_brazo_bajo;
Model rover_brazo_superior;
Model rover_brazo_pinza;
Model rover_pinza;

// Rover por partes: patas
Model rover_pata_der_frontal;
Model rover_pata_izq_frontal;
Model rover_pata_der_medio;
Model rover_pata_izq_medio;
Model rover_pata_der_trasera;
Model rover_pata_izq_trasera;

// Rover por partes: llantas
Model rover_llanta_der_frontal;
Model rover_llanta_izq_frontal;
Model rover_llanta_der_medio;
Model rover_llanta_izq_medio;
Model rover_llanta_der_trasera;
Model rover_llanta_izq_trasera;

// Avion
Model avion_texturizado;
Model helice_avion;


// Nave de carreras, pista y satelite
Model nave_espacial;
Model pista_carreras;
Model satelite_inf;
Model satelite_sup;
Model satelite_ala1;
Model satelite_ala2;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>> Luces >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

DirectionalLight mainLight;
//Indices de las luces puntuales (todas ligadas a la nave)
enum { PUNTUAL_NAVE = 0, PUNTUAL_MOTOR1 = 1, PUNTUAL_MOTOR2 = 2, NUM_LUCES_PUNTUALES = 3 };
//Indices de las luces spot
enum { SPOT_LINTERNA = 0, SPOT_FARO_BAJO_AVION = 1, SPOT_FARO_FRENTE_AVION = 2, SPOT_FARO_ROVER = 3, SPOT_FARO_NAVE = 4, NUM_LUCES_SPOT = 5 };
static_assert(MAX_POINT_LIGHTS >= NUM_LUCES_PUNTUALES, "MAX_POINT_LIGHTS es menor que las luces puntuales usadas");
static_assert(MAX_SPOT_LIGHTS >= NUM_LUCES_SPOT, "MAX_SPOT_LIGHTS es menor que las luces spot usadas");
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight pointLightsActivas[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLightsActivas[MAX_SPOT_LIGHTS];
unsigned int pointLightCountActivas = 0;
unsigned int spotLightCountActivas = 0;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//---------------- Faro bajo del avi�n ----------------
const glm::vec3 FARO_BAJO_AVION_POS = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 FARO_BAJO_AVION_DIR = glm::vec3(0.0f, -1.0f, 0.0f);
const glm::vec3 COLORES_FARO[6] = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 0.0f, 1.0f)
};
int indiceColorFaroAplicado = 0;

//---------------- Helice y faro frontal del avi�n ----------------
const glm::vec3 HELICE_AVION_POS = glm::vec3(-5.5f, -0.0f, 0.0f);
const glm::vec3 FARO_FRENTE_AVION_POS = HELICE_AVION_POS + glm::vec3(-1.5f, 0.0f, 0.0f);
const glm::vec3 FARO_FRENTE_AVION_DIR = glm::vec3(-1.0f, 0.0f, 0.0f);

//---------------- Faro frontal del rover ----------------
const glm::vec3 FARO_FRENTE_ROVER_POS = glm::vec3(5.5f, 2.0f, -1.0f);
const glm::vec3 FARO_FRENTE_ROVER_DIR = glm::vec3(1.0f, 0.0f, 0.0f);

//---------------- Variables de las jerarqu�as ----------------
glm::vec3 lowerLight = glm::vec3(0.0f);
GLfloat altura = 0.0f;
glm::vec3 avionPos = glm::vec3(0.0f);
glm::mat4 modelAvion = glm::mat4(1.0f);
glm::mat4 modelAvionAux = glm::mat4(1.0f);
glm::vec3 posBajoAvion = glm::vec3(0.0f);
glm::vec3 dirBajoAvion = glm::vec3(0.0f);
glm::vec3 posFrenteAvion = glm::vec3(0.0f);
glm::vec3 dirFrenteAvion = glm::vec3(0.0f);
glm::vec3 posFrenteRover = glm::vec3(0.0f);
glm::vec3 dirFrenteRover = glm::vec3(0.0f);


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>> Pista de carreras >>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
const char* PISTA_MODELO = "Models/pista_carreras.obj";
//La pista mide ~900 x 1100 (X x Z) y el piso 600 x 600.
const float PISTA_ESCALA = 0.5f;
const float PISTA_CENTRO_X = -130.0f;   //centro de la pista en las coordenadas del modelo
const float PISTA_CENTRO_Z = 30.0f;
const float PISO_Y = -2.0f;             //altura del piso (la misma de la traslacion del piso)
//Traslacion para centrar la pista en el piso y apoyar su base (grosor 2 * escala) sobre el piso
const glm::vec3 PISTA_OFFSET = glm::vec3(-PISTA_ESCALA * PISTA_CENTRO_X,
	PISO_Y + PISTA_ESCALA * 2.0f,
	-PISTA_ESCALA * PISTA_CENTRO_Z);

//---------------- Nave espacial ----------------
const glm::vec3 NAVE_ESCALA = glm::vec3(1.0f, 1.0f, 1.0f);
//Hacia donde mira la nariz de la nave en su espacio local: +1.0 = +X, -1.0 = -X.
//El fuego sale por -X, asi que la nariz mira a +X.
const float NAVE_NARIZ_SIGNO = 1.0f;
const float NAVE_VELOCIDAD = 1.2f;            //unidades de mundo por deltaTime
const float NAVE_ALTURA_FLOTACION = 3.0f;     //altura sobre la superficie de la pista
const float NAVE_SEMILARGO = 13.0f;           //media longitud de la nave (los motores estan en x=+-13): se usa para inclinarla y librar las rampas
const glm::vec3 NAVE_LUZ_PUNTUAL_POS = glm::vec3(0.0f, 2.7f, 0.0f);
const glm::vec3 NAVE_FARO_POS = glm::vec3(0.0f, -0.5f, 0.0f);
const glm::vec3 NAVE_FARO_DIR = glm::vec3(0.0f, -1.0f, 0.0f);

//---------------- Motores y fuego de la nave ----------------
const glm::vec3 MOTOR1_POS = glm::vec3(13.0f, 0.0f, 4.0f);
const glm::vec3 MOTOR2_POS = glm::vec3(13.0f, 0.0f, -4.0f);
const glm::vec3 FUEGO_DIR = glm::vec3(-1.0f, 0.0f, 0.0f);
const glm::vec3 FUEGO_ESCALA = glm::vec3(4.0f, 4.0f, 5.4f);
const glm::vec3 FUEGO_BRILLO = glm::vec3(2.0f, 2.0f, 2.0f);
MeshModel* meshFuego = nullptr;

//Jerarquia de la nave
float naveS = 0.0f;                     //distancia recorrida sobre la pista (unidades de mundo)
glm::vec3 navePos = glm::vec3(0.0f);
glm::vec3 naveAdelante = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 naveArriba = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 naveDerecha = glm::vec3(0.0f, 0.0f, -1.0f);
glm::mat4 modelNaveAux = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
glm::mat4 modelNave = glm::mat4(1.0f);
glm::vec3 posLuzNave = glm::vec3(0.0f);
glm::vec3 posFaroNave = glm::vec3(0.0f);
glm::vec3 dirFaroNave = glm::vec3(0.0f);
glm::vec3 posLuzMotor1 = glm::vec3(0.0f);
glm::vec3 posLuzMotor2 = glm::vec3(0.0f);

//---------------- Variables de dibujo ----------------
std::vector<std::string> skyboxFaces;
GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
GLuint uniformColor = 0;
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 modelaux = glm::mat4(1.0f);
glm::mat4 modelRoverAux = glm::mat4(1.0f);
glm::mat4 modelBrazo = glm::mat4(1.0f);
glm::mat4 modelPata = glm::mat4(1.0f);
glm::mat4 modelHelice = glm::mat4(1.0f);
glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
GLfloat now = 0.0f;
int contadorI = 0;
int contadorJ = 0;

//calculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	MeshModel* obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los numeros

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un numero

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//Trayectoria recta del avion
void trayectoriaRecta(float distancia)
{
	avionX = inicioAvionX + sin(yawInicio) * distancia;
	avionZ = inicioAvionZ + cos(yawInicio) * distancia;
	avionYaw = yawInicio;
}

//Trayectoria curva del avion
void trayectoriaCurva(float angulo)
{
	avionYaw = yawInicio + angulo;
	avionX = inicioAvionX + radioGiro * (cos(yawInicio) - cos(avionYaw));
	avionZ = inicioAvionZ + radioGiro * (sin(avionYaw) - sin(yawInicio));
}

//Actualizacion del recorrido del avion
void actualizaAvion(float dt)
{
	float avance = velAvion * dt;
	float meta;
	bool esGiro = (faseAvion % 2 == 1);

	if (!esGiro)
	{
		progresoFase += avance;
		meta = longitudRecta;
	}
	else
	{
		progresoFase += avance / radioGiro;
		meta = (faseAvion == 1 || faseAvion == 5) ? 2.0f * PI_F : PI_F;
	}

	if (progresoFase > meta)
		progresoFase = meta;

	if (!esGiro)
		trayectoriaRecta(progresoFase);
	else
		trayectoriaCurva(progresoFase);

	if (progresoFase >= meta)
	{
		inicioAvionX = avionX;
		inicioAvionZ = avionZ;
		yawInicio = avionYaw;
		progresoFase = 0.0f;
		faseAvion = (faseAvion + 1) % 8;

		if (faseAvion == 0)
		{
			inicioAvionX = 0.0f;
			inicioAvionZ = 40.0f;
			yawInicio = 0.0f;
		}
	}
}

//---------------- Luz para faro bajo del avi�n ----------------
SpotLight crearFaroBajoAvion(const glm::vec3& c)
{
	return SpotLight(c.r, c.g, c.b,
		0.7f, 3.0f,
		FARO_BAJO_AVION_POS.x, FARO_BAJO_AVION_POS.y, FARO_BAJO_AVION_POS.z,
		FARO_BAJO_AVION_DIR.x, FARO_BAJO_AVION_DIR.y, FARO_BAJO_AVION_DIR.z,
		0.3f, 0.01f, 0.003f,
		25.0f);
}


//---------------- Luz puntual de la nave ----------------
PointLight crearLuzNave(const glm::vec3& pos)
{
	return PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 3.0f,
		pos.x, pos.y, pos.z,
		1.0f, 0.045f, 0.0075f);
}

//---------------- Luz puntual de los motores ----------------
PointLight crearLuzMotor(const glm::vec3& pos)
{
	return PointLight(0.1f, 0.3f, 1.0f,
		0.0f, 4.0f,
		pos.x, pos.y, pos.z,
		1.0f, 0.09f, 0.032f);
}

//---------------- Faro de la nave ----------------
SpotLight crearFaroNave()
{
	return SpotLight(0.5f, 0.8f, 0.0f,
		0.0f, 3.0f,
		NAVE_FARO_POS.x, NAVE_FARO_POS.y, NAVE_FARO_POS.z,
		NAVE_FARO_DIR.x, NAVE_FARO_DIR.y, NAVE_FARO_DIR.z,
		1.0f, 0.045f, 0.0075f,
		30.0f);
}

//---------------- Malla del fuego ----------------
void CrearFuego()
{
	unsigned int fuego_indices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7
	};

	GLfloat fuego_vertices[] = {
		//x		y		z		S		T		NX		NY		NZ
		-0.5f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,
		0.5f,	0.0f,	0.0f,	1.0f,	1.0f,	0.0f,	1.0f,	0.0f,
		0.5f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f,
		-0.5f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,

		0.0f,	-0.5f,	0.0f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,
		0.0f,	0.5f,	0.0f,	1.0f,	1.0f,	1.0f,	0.0f,	0.0f,
		0.0f,	0.5f,	1.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,
		0.0f,	-0.5f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f
	};

	meshFuego = new MeshModel();
	meshFuego->CreateMeshModel(fuego_vertices, fuego_indices, 64, 12);
}

//---------------- Orientacion del fuego ----------------
glm::mat4 orientarFuego(const glm::vec3& dir)
{
	glm::vec3 d = glm::normalize(dir);
	glm::vec3 eje = glm::vec3(0.0f, 0.0f, 1.0f);
	float c = glm::dot(eje, d);
	if (c > 0.9999f) return glm::mat4(1.0f);
	if (c < -0.9999f) return glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	return glm::rotate(glm::mat4(1.0f), std::acos(c), glm::normalize(glm::cross(eje, d)));
}

//---------------- Centro del fuego de un motor ----------------
glm::vec3 centroFuego(const glm::vec3& motorPos)
{
	glm::vec3 centro = motorPos + glm::normalize(FUEGO_DIR) * (FUEGO_ESCALA.z * 0.5f);
	return glm::vec3(modelNave * glm::vec4(centro, 1.0f));
}

//---------------- Dibujo del fuego de un motor ----------------
void dibujarFuego(const glm::vec3& motorPos)
{
	model = glm::translate(modelNave, motorPos);
	model = model * orientarFuego(FUEGO_DIR);
	model = glm::scale(model, FUEGO_ESCALA);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	meshFuego->RenderMeshModel();
}


//---------------- Recorrido parametrico de la pista----------------
// Tramos obtenidos del trazado de la pista original de blender: rectas, rampas y curvas.
// Las coordenadas son las del OBJ ANTES de aplicar PISTA_ESCALA y PISTA_OFFSET.
// Cada tramo tiene distancia inicial s, longitud, extremos y rumbo horizontal inicial.
// En las curvas se utiliza x=R*sin(angulo) y z=R*(1-cos(angulo)),
// con radio 38 y giro de 90 grados. En rectas y rampas se interpolan extremos.
enum TipoTramoNave { RECTA, CURVA };
struct TramoNave {
	float inicio, longitud;
	glm::vec3 desde, hasta;
	glm::vec2 rumbo;
	int sentido; // +1 izquierda matematica en XZ; -1 derecha; 0 recta
	TipoTramoNave tipo;
};
const float RADIO_CURVA_PISTA = 38.0f;
const float LONGITUD_CIRCUITO = 4238.34f;
const TramoNave TRAMOS_NAVE[] = {
	{ 0.000f, 40.000f, glm::vec3(0.00000f, 0.00000f, 0.00000f), glm::vec3(-40.00000f, 0.00000f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg01_START
	{ 40.000f, 40.000f, glm::vec3(-40.00000f, 0.00000f, 0.00000f), glm::vec3(-80.00000f, 0.00000f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg02_Recta
	{ 80.000f, 40.000f, glm::vec3(-80.00000f, 0.00000f, 0.00000f), glm::vec3(-108.28430f, 28.28430f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg03_Rampa_Sube_1
	{ 120.000f, 40.000f, glm::vec3(-108.28430f, 28.28430f, 0.00000f), glm::vec3(-148.28430f, 28.28430f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg04_Meseta_1
	{ 160.000f, 40.000f, glm::vec3(-148.28430f, 28.28430f, 0.00000f), glm::vec3(-176.56850f, 56.56850f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg05_Rampa_Sube_2
	{ 200.000f, 80.000f, glm::vec3(-176.56850f, 56.56850f, 0.00000f), glm::vec3(-256.56850f, 56.56850f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg06_Meseta_Alta
	{ 280.000f, 40.000f, glm::vec3(-256.56850f, 56.56850f, 0.00000f), glm::vec3(-284.85280f, 28.28430f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg07_Rampa_Baja_1
	{ 320.000f, 40.000f, glm::vec3(-284.85280f, 28.28430f, 0.00000f), glm::vec3(-324.85280f, 28.28430f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg08_Meseta_2
	{ 360.000f, 40.000f, glm::vec3(-324.85280f, 28.28430f, 0.00000f), glm::vec3(-353.13710f, 0.00000f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg09_Rampa_Baja_2
	{ 400.000f, 168.860f, glm::vec3(-353.13710f, 0.00000f, 0.00000f), glm::vec3(-521.99701f, 0.00000f, 0.00731f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg10_Recta
	{ 568.860f, 59.690f, glm::vec3(-521.99701f, 0.00000f, 0.00731f), glm::vec3(-559.99710f, 0.00000f, 37.99680f), glm::vec2(-0.999692f, 0.024835f), -1, CURVA }, // Seg11_Curva_Izq_1
	{ 628.550f, 84.000f, glm::vec3(-559.99710f, 0.00000f, 37.99680f), glm::vec3(-559.99244f, 0.00000f, 121.99672f), glm::vec2(-0.003276f, 0.999995f), 0, RECTA }, // Seg12_Recta_B
	{ 712.550f, 59.690f, glm::vec3(-559.99244f, 0.00000f, 121.99672f), glm::vec3(-522.00340f, 0.00000f, 159.99930f), glm::vec2(0.025701f, 0.999670f), -1, CURVA }, // Seg13_Curva_Izq_2
	{ 772.240f, 44.000f, glm::vec3(-522.00340f, 0.00000f, 159.99930f), glm::vec3(-478.00348f, 0.00000f, 160.00487f), glm::vec2(0.999997f, 0.002436f), 0, RECTA }, // Seg14_Recta_C
	{ 816.240f, 59.690f, glm::vec3(-478.00348f, 0.00000f, 160.00487f), glm::vec3(-440.00172f, 0.00000f, 197.99633f), glm::vec2(0.999711f, 0.024036f), 1, CURVA }, // Seg15_Curva_Der_1
	{ 875.930f, 104.000f, glm::vec3(-440.00172f, 0.00000f, 197.99633f), glm::vec3(-440.00010f, 0.00000f, 301.99630f), glm::vec2(0.000000f, 1.000000f), 0, RECTA }, // Seg16_Recta_D
	{ 979.930f, 59.690f, glm::vec3(-440.00010f, 0.00000f, 301.99630f), glm::vec3(-477.99609f, 0.00000f, 339.99341f), glm::vec2(-0.020949f, 0.999781f), 1, CURVA }, // Seg17_Curva_Der_2
	{ 1039.620f, 44.000f, glm::vec3(-477.99609f, 0.00000f, 339.99341f), glm::vec3(-521.99600f, 0.00000f, 340.00154f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg18_Recta_E
	{ 1083.620f, 59.690f, glm::vec3(-521.99600f, 0.00000f, 340.00154f), glm::vec3(-559.99217f, 0.00000f, 377.99587f), glm::vec2(-0.999781f, 0.020949f), -1, CURVA }, // Seg19_Curva_Izq_3
	{ 1143.310f, 144.000f, glm::vec3(-559.99217f, 0.00000f, 377.99587f), glm::vec3(-559.99594f, 0.00000f, 521.99573f), glm::vec2(0.000000f, 1.000000f), 0, RECTA }, // Seg20_Recta_F
	{ 1287.310f, 59.690f, glm::vec3(-559.99594f, 0.00000f, 521.99573f), glm::vec3(-522.00445f, 0.00000f, 559.99335f), glm::vec2(0.020943f, 0.999781f), -1, CURVA }, // Seg21_Curva_Izq_4
	{ 1347.000f, 84.000f, glm::vec3(-522.00445f, 0.00000f, 559.99335f), glm::vec3(-438.00455f, 0.00000f, 559.99350f), glm::vec2(1.000000f, 0.000000f), 0, RECTA }, // Seg22_Recta_G
	{ 1431.000f, 59.690f, glm::vec3(-438.00455f, 0.00000f, 559.99350f), glm::vec3(-400.00413f, 0.00000f, 522.00470f), glm::vec2(0.999722f, -0.023572f), -1, CURVA }, // Seg23_Curva_Izq_5
	{ 1490.690f, 24.000f, glm::vec3(-400.00413f, 0.00000f, 522.00470f), glm::vec3(-399.99227f, 0.00000f, 498.00480f), glm::vec2(0.002833f, -0.999996f), 0, RECTA }, // Seg24_Recta_H
	{ 1514.690f, 59.690f, glm::vec3(-399.99227f, 0.00000f, 498.00480f), glm::vec3(-362.00500f, 0.00000f, 460.00162f), glm::vec2(0.025586f, -0.999673f), 1, CURVA }, // Seg25_Curva_Der_3
	{ 1574.380f, 107.430f, glm::vec3(-362.00500f, 0.00000f, 460.00162f), glm::vec3(-254.57500f, 0.00000f, 459.99964f), glm::vec2(0.999995f, -0.003140f), 0, RECTA }, // Seg26_Recta_I
	{ 1681.820f, 59.690f, glm::vec3(-254.56500f, 0.00000f, 459.99964f), glm::vec3(-216.57583f, 0.00000f, 421.99660f), glm::vec2(0.999776f, -0.021142f), -1, CURVA }, // Seg27_Curva_Izq_6
	{ 1741.510f, 54.000f, glm::vec3(-216.57583f, 0.00000f, 421.99660f), glm::vec3(-216.56608f, 0.00000f, 367.99670f), glm::vec2(0.000000f, -1.000000f), 0, RECTA }, // Seg28_Recta_J
	{ 1795.510f, 59.690f, glm::vec3(-216.56608f, 0.00000f, 367.99670f), glm::vec3(-178.56542f, 0.00000f, 330.00752f), glm::vec2(0.021135f, -0.999777f), 1, CURVA }, // Seg29_Curva_Der_4
	{ 1855.200f, 24.000f, glm::vec3(-178.56542f, 0.00000f, 330.00752f), glm::vec3(-154.56556f, 0.00000f, 329.99490f), glm::vec2(1.000000f, 0.000000f), 0, RECTA }, // Seg30_Recta_K
	{ 1879.200f, 59.690f, glm::vec3(-154.56556f, 0.00000f, 329.99490f), glm::vec3(-116.57427f, 0.00000f, 291.99720f), glm::vec2(0.999777f, -0.021130f), -1, CURVA }, // Seg31_Curva_Izq_7
	{ 1938.890f, 24.000f, glm::vec3(-116.57427f, 0.00000f, 291.99720f), glm::vec3(-116.57575f, 0.00000f, 267.99729f), glm::vec2(0.001284f, -0.999999f), 0, RECTA }, // Seg32_Recta_L
	{ 1962.890f, 59.690f, glm::vec3(-116.57575f, 0.00000f, 267.99729f), glm::vec3(-154.57107f, 0.00000f, 230.00312f), glm::vec2(-0.024743f, -0.999694f), -1, CURVA }, // Seg33_Curva_Izq_8
	{ 2022.580f, 24.000f, glm::vec3(-154.57107f, 0.00000f, 230.00312f), glm::vec3(-178.57091f, 0.00000f, 229.99229f), glm::vec2(-0.999995f, -0.003192f), 0, RECTA }, // Seg34_Recta_M
	{ 2046.580f, 59.690f, glm::vec3(-178.57091f, 0.00000f, 229.99229f), glm::vec3(-216.56772f, 0.00000f, 191.99780f), glm::vec2(-0.999666f, -0.025831f), 1, CURVA }, // Seg35_Curva_Der_5
	{ 2106.270f, 132.000f, glm::vec3(-216.56772f, 0.00000f, 191.99780f), glm::vec3(-216.56850f, 0.00000f, 59.99780f), glm::vec2(-0.002573f, -0.999997f), 0, RECTA }, // Seg36_Recta_Pre_Cruce
	{ 2238.270f, 120.000f, glm::vec3(-216.56850f, 0.00000f, 59.99780f), glm::vec3(-216.56850f, 0.00000f, -60.00220f), glm::vec2(0.000000f, -1.000000f), 0, RECTA }, // Seg37_Paso_Inferior
	{ 2358.270f, 132.000f, glm::vec3(-216.56850f, 0.00000f, -60.00220f), glm::vec3(-216.57383f, 0.00000f, -192.00212f), glm::vec2(0.000000f, -1.000000f), 0, RECTA }, // Seg38_Recta_Post_Cruce
	{ 2490.270f, 59.690f, glm::vec3(-216.57383f, 0.00000f, -192.00212f), glm::vec3(-254.57052f, 0.00000f, -229.99899f), glm::vec2(-0.024386f, -0.999703f), -1, CURVA }, // Seg39_Curva_Izq_9
	{ 2549.960f, 24.000f, glm::vec3(-254.57052f, 0.00000f, -229.99899f), glm::vec3(-278.57050f, 0.00000f, -230.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg40_Recta_N
	{ 2573.960f, 59.690f, glm::vec3(-278.57050f, 0.00000f, -230.00000f), glm::vec3(-316.56225f, 0.00000f, -268.00179f), glm::vec2(-0.999777f, -0.021099f), 1, CURVA }, // Seg41_Curva_Der_6
	{ 2633.650f, 24.000f, glm::vec3(-316.56225f, 0.00000f, -268.00179f), glm::vec3(-316.56718f, 0.00000f, -292.00170f), glm::vec2(0.000000f, -1.000000f), 0, RECTA }, // Seg42_Recta_O
	{ 2657.650f, 59.690f, glm::vec3(-316.56718f, 0.00000f, -292.00170f), glm::vec3(-278.56700f, 0.00000f, -329.99221f), glm::vec2(0.021098f, -0.999777f), 1, CURVA }, // Seg43_Curva_Der_7
	{ 2717.340f, 24.000f, glm::vec3(-278.56700f, 0.00000f, -329.99221f), glm::vec3(-254.56710f, 0.00000f, -330.00389f), glm::vec2(1.000000f, 0.000000f), 0, RECTA }, // Seg44_Recta_P
	{ 2741.340f, 59.690f, glm::vec3(-254.56710f, 0.00000f, -330.00389f), glm::vec3(-216.57529f, 0.00000f, -368.00125f), glm::vec2(0.999778f, -0.021092f), -1, CURVA }, // Seg45_Curva_Izq_10
	{ 2801.030f, 94.000f, glm::vec3(-216.57529f, 0.00000f, -368.00125f), glm::vec3(-216.56211f, 0.00000f, -462.00115f), glm::vec2(0.000000f, -1.000000f), 0, RECTA }, // Seg46_Recta_Q
	{ 2895.030f, 59.690f, glm::vec3(-216.56211f, 0.00000f, -462.00115f), glm::vec3(-178.56760f, 0.00000f, -499.99565f), glm::vec2(0.023388f, -0.999726f), 1, CURVA }, // Seg47_Curva_Der_8
	{ 2954.720f, 100.570f, glm::vec3(-178.56760f, 0.00000f, -499.99565f), glm::vec3(-77.99760f, 0.00000f, -499.99568f), glm::vec2(0.999996f, -0.002649f), 0, RECTA }, // Seg48_Recta_R
	{ 3055.290f, 59.690f, glm::vec3(-77.99760f, 0.00000f, -499.99568f), glm::vec3(-40.00643f, 0.00000f, -461.99785f), glm::vec2(0.999777f, 0.021117f), 1, CURVA }, // Seg49_Curva_Der_9
	{ 3114.980f, 24.000f, glm::vec3(-40.00643f, 0.00000f, -461.99785f), glm::vec3(-39.99324f, 0.00000f, -437.99795f), glm::vec2(0.000257f, 1.000000f), 0, RECTA }, // Seg50_Recta_S
	{ 3138.980f, 59.690f, glm::vec3(-39.99324f, 0.00000f, -437.99795f), glm::vec3(-1.99810f, 0.00000f, -400.00392f), glm::vec2(0.023942f, 0.999713f), -1, CURVA }, // Seg51_Curva_Izq_11
	{ 3198.670f, 40.000f, glm::vec3(-1.99810f, 0.00000f, -400.00392f), glm::vec3(37.87122f, 0.31550f, -400.00000f), glm::vec2(0.999996f, 0.002901f), 0, RECTA }, // Seg52_Recta
	{ 3238.670f, 40.000f, glm::vec3(37.87122f, 0.31550f, -400.00000f), glm::vec3(66.41627f, 27.97009f, -400.00000f), glm::vec2(1.000000f, 0.000000f), 0, RECTA }, // Seg53_Rampa_Sube_3
	{ 3278.670f, 40.000f, glm::vec3(66.41627f, 27.97009f, -400.00000f), glm::vec3(106.15552f, 28.59980f, -400.00000f), glm::vec2(1.000000f, 0.000000f), 0, RECTA }, // Seg54_Meseta_3
	{ 3318.670f, 40.000f, glm::vec3(106.15552f, 28.59980f, -400.00000f), glm::vec3(134.70054f, 56.25435f, -400.00000f), glm::vec2(1.000000f, 0.000000f), 0, RECTA }, // Seg55_Rampa_Sube_4
	{ 3358.670f, 40.000f, glm::vec3(134.70054f, 56.25435f, -400.00000f), glm::vec3(174.43972f, 56.88400f, -400.00000f), glm::vec2(1.000000f, 0.000000f), 0, RECTA }, // Seg56_Meseta_4
	{ 3398.670f, 40.000f, glm::vec3(174.43972f, 56.88400f, -400.00000f), glm::vec3(202.98484f, 84.53865f, -400.00000f), glm::vec2(1.000000f, 0.000000f), 0, RECTA }, // Seg57_Rampa_Sube_5
	{ 3438.670f, 59.150f, glm::vec3(202.98484f, 84.53865f, -400.00000f), glm::vec3(262.00470f, 84.85280f, -399.99955f), glm::vec2(1.000000f, 0.000000f), 0, RECTA }, // Seg58_Meseta_Cima_1
	{ 3497.820f, 59.690f, glm::vec3(262.00470f, 84.85280f, -399.99955f), glm::vec3(299.99270f, 84.85280f, -361.99550f), glm::vec2(0.999776f, 0.021172f), 1, CURVA }, // Seg59_Curva_Der_10
	{ 3557.510f, 40.000f, glm::vec3(299.99270f, 84.85280f, -361.99550f), glm::vec3(300.00000f, 84.58885f, -322.10492f), glm::vec2(0.000000f, 1.000000f), 0, RECTA }, // Seg60_Meseta_Cima_2
	{ 3597.510f, 40.000f, glm::vec3(300.00000f, 84.58885f, -322.10492f), glm::vec3(300.00000f, 56.82933f, -293.60325f), glm::vec2(0.000000f, 1.000000f), 0, RECTA }, // Seg61_Rampa_Baja_3
	{ 3637.510f, 40.000f, glm::vec3(300.00000f, 56.82933f, -293.60325f), glm::vec3(300.00000f, 56.30455f, -253.82062f), glm::vec2(0.000000f, 1.000000f), 0, RECTA }, // Seg62_Meseta_5
	{ 3677.510f, 40.000f, glm::vec3(300.00000f, 56.30455f, -253.82062f), glm::vec3(300.00000f, 28.54513f, -225.31898f), glm::vec2(0.000000f, 1.000000f), 0, RECTA }, // Seg63_Rampa_Baja_4
	{ 3717.510f, 80.000f, glm::vec3(300.00000f, 28.54513f, -225.31898f), glm::vec3(300.00000f, 28.02035f, -145.53632f), glm::vec2(0.000000f, 1.000000f), 0, RECTA }, // Seg64_Meseta_6
	{ 3797.510f, 40.000f, glm::vec3(300.00000f, 28.02035f, -145.53632f), glm::vec3(300.00000f, 0.26083f, -117.03475f), glm::vec2(0.000000f, 1.000000f), 0, RECTA }, // Seg65_Rampa_Baja_5
	{ 3837.510f, 79.150f, glm::vec3(300.00000f, 0.26083f, -117.03475f), glm::vec3(299.99211f, 0.00000f, -37.99287f), glm::vec2(0.000000f, 1.000000f), 0, RECTA }, // Seg66_Recta
	{ 3916.650f, 59.690f, glm::vec3(299.99223f, 0.00000f, -38.00286f), glm::vec3(262.00300f, 0.00000f, -0.00124f), glm::vec2(-0.025704f, 0.999670f), 1, CURVA }, // Seg67_Curva_Der_11
	{ 3976.340f, 182.000f, glm::vec3(262.00300f, 0.00000f, -0.00124f), glm::vec3(80.00300f, 0.00000f, 0.00000f), glm::vec2(-0.999996f, 0.002980f), 0, RECTA }, // Seg68_Recta_T
	{ 4158.340f, 40.000f, glm::vec3(80.00300f, 0.00000f, 0.00000f), glm::vec3(40.00300f, 0.00000f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg69_FINISH
	{ 4198.340f, 40.000f, glm::vec3(40.00300f, 0.00000f, 0.00000f), glm::vec3(0.00000f, 0.00000f, 0.00000f), glm::vec2(-1.000000f, 0.000000f), 0, RECTA }, // Seg70_Recta_Previa
};
const int NUM_TRAMOS_NAVE = sizeof(TRAMOS_NAVE) / sizeof(TRAMOS_NAVE[0]);

// Obtiene la posicion en la pista mediante sus funciones por tramos.
glm::vec3 puntoPista(float distanciaMundo)
{
	float s = std::fmod(distanciaMundo / PISTA_ESCALA, LONGITUD_CIRCUITO);
	if (s < 0.0f) s += LONGITUD_CIRCUITO;

	const TramoNave* tramo = &TRAMOS_NAVE[NUM_TRAMOS_NAVE - 1];
	for (int i = 0; i < NUM_TRAMOS_NAVE; ++i)
	{
		if (s < TRAMOS_NAVE[i].inicio + TRAMOS_NAVE[i].longitud)
		{
			tramo = &TRAMOS_NAVE[i];
			break;
		}
	}

	float d = glm::clamp(s - tramo->inicio, 0.0f, tramo->longitud);
	float u = d / tramo->longitud;
	glm::vec3 local;
	if (tramo->tipo == RECTA)
	{
		// Recta horizontal o rampa: incrementos proporcionales al avance.
		local = glm::mix(tramo->desde, tramo->hasta, u);
	}
	else
	{

		glm::vec2 a(tramo->desde.x, tramo->desde.z);
		glm::vec2 b(tramo->hasta.x, tramo->hasta.z);
		glm::vec2 cuerda = b - a;
		float signo = static_cast<float>(tramo->sentido);
		glm::vec2 centro = (a + b) * 0.5f
			+ signo * glm::vec2(-cuerda.y, cuerda.x) * 0.5f;
		glm::vec2 radioInicial = a - centro;
		float angulo = signo * u * (PI_F * 0.5f);
		glm::vec2 radial(
			radioInicial.x * std::cos(angulo) - radioInicial.y * std::sin(angulo),
			radioInicial.x * std::sin(angulo) + radioInicial.y * std::cos(angulo));
		glm::vec2 horizontal = centro + radial;
		local = glm::vec3(horizontal.x,
			glm::mix(tramo->desde.y, tramo->hasta.y, u), horizontal.y);
	}
	return PISTA_OFFSET + PISTA_ESCALA * local;
}

// Actualiza posicion, inclinacion y giro de TODA la jerarquia de la nave.
void actualizaNave(float dt)
{
	float longitudMundo = LONGITUD_CIRCUITO * PISTA_ESCALA;
	naveS = std::fmod(naveS + NAVE_VELOCIDAD * dt, longitudMundo);
	if (naveS < 0.0f) naveS += longitudMundo;

	glm::vec3 centro = puntoPista(naveS);
	// Diferencia centrada: suaviza el cambio de orientacion en las rampas.
	glm::vec3 frente = puntoPista(naveS + NAVE_SEMILARGO);
	glm::vec3 atras = puntoPista(naveS - NAVE_SEMILARGO);
	naveAdelante = glm::normalize(frente - atras);
	naveDerecha = glm::normalize(glm::cross(naveAdelante, glm::vec3(0.0f, 1.0f, 0.0f)));
	naveArriba = glm::normalize(glm::cross(naveDerecha, naveAdelante));

	float holgura = 0.0f;
	for (int k = -3; k <= 3; ++k)
	{
		float distancia = NAVE_SEMILARGO * k / 3.0f;
		float elevacion = glm::dot(puntoPista(naveS + distancia) - centro, naveArriba);
		if (elevacion > holgura) holgura = elevacion;
	}
	navePos = centro + naveArriba * (NAVE_ALTURA_FLOTACION + holgura);

	// Matriz unica: orienta la nariz y arrastra los motores, el fuego y las luces.
	// El OBJ de esta practica mira al +X local (NAVE_NARIZ_SIGNO = +1).
	modelNaveAux = glm::mat4(
		glm::vec4(NAVE_NARIZ_SIGNO * naveAdelante, 0.0f),
		glm::vec4(naveArriba, 0.0f),
		glm::vec4(NAVE_NARIZ_SIGNO * naveDerecha, 0.0f),
		glm::vec4(navePos, 1.0f));
	modelNaveAux = glm::rotate(
		modelNaveAux,
		glm::radians(180.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	modelNave = glm::scale(modelNaveAux, NAVE_ESCALA);
}

//---------------- Luces ligadas a la nave (siguen su jerarquia) ----------------
void actualizaLucesNave()
{
	// Luz puntual de la nave
	posLuzNave = glm::vec3(modelNave * glm::vec4(NAVE_LUZ_PUNTUAL_POS, 1.0f));
	pointLights[PUNTUAL_NAVE] = crearLuzNave(posLuzNave);

	// Faro de la nave
	posFaroNave = glm::vec3(modelNave * glm::vec4(NAVE_FARO_POS, 1.0f));
	dirFaroNave = glm::normalize(glm::vec3(modelNaveAux * glm::vec4(NAVE_FARO_DIR, 0.0f)));
	spotLights[SPOT_FARO_NAVE].SetFlash(posFaroNave, dirFaroNave);

	// Luces de los motores
	posLuzMotor1 = centroFuego(MOTOR1_POS);
	posLuzMotor2 = centroFuego(MOTOR2_POS);
	pointLights[PUNTUAL_MOTOR1] = crearLuzMotor(posLuzMotor1);
	pointLights[PUNTUAL_MOTOR2] = crearLuzMotor(posLuzMotor2);
}

//---------------- Luces activas segun las teclas ----------------
void armaLucesActivas()
{
	pointLightCountActivas = 0;
	if (mainWindow.getLuzNaveEncendida()) pointLightsActivas[pointLightCountActivas++] = pointLights[PUNTUAL_NAVE];
	if (mainWindow.getMotor1Encendido()) pointLightsActivas[pointLightCountActivas++] = pointLights[PUNTUAL_MOTOR1];
	if (mainWindow.getMotor2Encendido()) pointLightsActivas[pointLightCountActivas++] = pointLights[PUNTUAL_MOTOR2];

	spotLightCountActivas = 0;
	spotLightsActivas[spotLightCountActivas++] = spotLights[SPOT_LINTERNA];
	spotLightsActivas[spotLightCountActivas++] = spotLights[SPOT_FARO_BAJO_AVION];
	spotLightsActivas[spotLightCountActivas++] = spotLights[SPOT_FARO_FRENTE_AVION];
	spotLightsActivas[spotLightCountActivas++] = spotLights[SPOT_FARO_ROVER];
	if (mainWindow.getLuzNaveEncendida()) spotLightsActivas[spotLightCountActivas++] = spotLights[SPOT_FARO_NAVE];
}

//---------------- Creacion de todas las luces ----------------
void CreaLuces()
{
	//luz direccional, solo 1 y siempre debe de existir
	mainLight = DirectionalLight(0.5f, 0.5f, 0.5f,
		0.3f, 0.6f,
		1.0f, -1.0f, -1.0f);

	//>>>>>>>>>> Luces puntuales (nave) <<<<<<<<<<
	pointLights[PUNTUAL_NAVE] = crearLuzNave(NAVE_LUZ_PUNTUAL_POS);
	pointLights[PUNTUAL_MOTOR1] = crearLuzMotor(MOTOR1_POS);
	pointLights[PUNTUAL_MOTOR2] = crearLuzMotor(MOTOR2_POS);

	//>>>>>>>>>> Spot: linterna de la camara <<<<<<<<<<
	spotLights[SPOT_LINTERNA] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);

	//>>>>>>>>>> Spot: faro bajo del avion <<<<<<<<<<
	indiceColorFaroAplicado = mainWindow.getIndiceColorFaro();
	spotLights[SPOT_FARO_BAJO_AVION] = crearFaroBajoAvion(COLORES_FARO[indiceColorFaroAplicado]);

	//>>>>>>>>>> Spot: faro frontal del avion <<<<<<<<<<
	spotLights[SPOT_FARO_FRENTE_AVION] = SpotLight(1.0f, 0.0f, 1.0f,
		0.7f, 3.0f,
		FARO_FRENTE_AVION_POS.x, FARO_FRENTE_AVION_POS.y, FARO_FRENTE_AVION_POS.z,
		FARO_FRENTE_AVION_DIR.x, FARO_FRENTE_AVION_DIR.y, FARO_FRENTE_AVION_DIR.z,
		0.3f, 0.01f, 0.003f,
		45.0f);

	//>>>>>>>>>> Spot: faro frontal del rover <<<<<<<<<<
	spotLights[SPOT_FARO_ROVER] = SpotLight(1.0f, 0.0f, 0.0f,
		0.7f, 3.0f,
		FARO_FRENTE_ROVER_POS.x, FARO_FRENTE_ROVER_POS.y, FARO_FRENTE_ROVER_POS.z,
		FARO_FRENTE_ROVER_DIR.x, FARO_FRENTE_ROVER_DIR.y, FARO_FRENTE_ROVER_DIR.z,
		0.3f, 0.01f, 0.003f,
		50.0f);

	//>>>>>>>>>> Spot: faro de la nave <<<<<<<<<<
	spotLights[SPOT_FARO_NAVE] = crearFaroNave();
}


//---------------- Trayectoria del satelite segun la distancia recorrida d ----------------
//Recta (+X) -> vuelta de 360 grados centrada en el centro de la escena -> recta (+X)
void trayectoriaSatelite(float d)
{
	float largoRecta = SAT_LIMITE;
	float largoVuelta = 2.0f * PI_F * SAT_RADIO;

	if (d < largoRecta)
	{
		satX = -SAT_LIMITE + d;
		satZ = SAT_RADIO;
		satYaw = 0.0f;
	}
	else if (d < largoRecta + largoVuelta)
	{
		float a = (d - largoRecta) / SAT_RADIO;
		satX = SAT_RADIO * sin(a);
		satZ = SAT_RADIO * cos(a);
		satYaw = a;
	}
	else
	{
		satX = d - largoRecta - largoVuelta;
		satZ = SAT_RADIO;
		satYaw = 0.0f;
	}
}

//---------------- Actualizacion del satelite: recorrido y articulaciones automaticas ----------------
void actualizaSatelite(float dt)
{
	float largoTotal = 2.0f * SAT_LIMITE + 2.0f * PI_F * SAT_RADIO;

	switch (satFase)
	{
	case 0: //ida
		satDistancia += SAT_VELOCIDAD * dt;
		if (satDistancia >= largoTotal)
		{
			satDistancia = largoTotal;
			satFase = 1;
			satProgresoGiro = 0.0f;
		}
		break;
	case 1: //giro de 180 grados en el extremo +X
		satProgresoGiro += dt / SAT_TIEMPO_GIRO;
		if (satProgresoGiro >= 1.0f)
		{
			satProgresoGiro = 1.0f;
			satFase = 2;
		}
		break;
	case 2: //regreso por la misma trayectoria
		satDistancia -= SAT_VELOCIDAD * dt;
		if (satDistancia <= 0.0f)
		{
			satDistancia = 0.0f;
			satFase = 3;
			satProgresoGiro = 0.0f;
		}
		break;
	default: //giro de 180 grados en el extremo -X
		satProgresoGiro += dt / SAT_TIEMPO_GIRO;
		if (satProgresoGiro >= 1.0f)
		{
			satProgresoGiro = 1.0f;
			satFase = 0;
		}
		break;
	}

	trayectoriaSatelite(satDistancia);

	//el cuerpo mira hacia donde avanza (de regreso mira hacia atras)
	float suave = satProgresoGiro * satProgresoGiro * (3.0f - 2.0f * satProgresoGiro);
	if (satFase == 1) satYaw += PI_F * suave;
	else if (satFase == 2) satYaw += PI_F;
	else if (satFase == 3) satYaw += PI_F + PI_F * suave;

	//Articulaciones automaticas
	satAngSup += SAT_SUP_VELOCIDAD * dt;
	if (satAngSup > 360.0f)
		satAngSup -= 360.0f;

	satAngAla += satSentidoAla * SAT_ALA_VELOCIDAD * dt;
	if (satAngAla >= SAT_ALA_MAX)
	{
		satAngAla = SAT_ALA_MAX;
		satSentidoAla = -1.0f;
	}
	else if (satAngAla <= SAT_ALA_MIN)
	{
		satAngAla = SAT_ALA_MIN;
		satSentidoAla = 1.0f;
	}
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearFuego();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);



	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//>>>>>>>>>>>>>>>>>>>>>> Carga de texturas >>>>>>>>>>>>>>>>>>>>
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();

	naveTexture = Texture("Textures/nave_espacial_texturas/Material.001_Base_color.jpg");
	naveTexture.LoadTexture();
	fuegoTexture = Texture("Textures/fuego.png");
	fuegoTexture.LoadTextureA();


	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//>>>>>>>>>>>>>>>>>>>>>> Carga de modelos >>>>>>>>>>>>>>>>>>>>
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//Carga de rover completo
	Rover_M = Model();
	Rover_M.LoadModel("Models/RoverModificado.obj");

	//Carga de rover por partes
	rover_cuerpo = Model();
	rover_cuerpo.LoadModel("Models/rover_cuerpo.obj");

	rover_brazo_base = Model();
	rover_brazo_base.LoadModel("Models/rover_brazo_base.obj");

	rover_brazo_bajo = Model();
	rover_brazo_bajo.LoadModel("Models/rover_brazo_bajo.obj");

	rover_brazo_superior = Model();
	rover_brazo_superior.LoadModel("Models/rover_brazo_superior.obj");

	rover_brazo_pinza = Model();
	rover_brazo_pinza.LoadModel("Models/rover_brazo_pinza.obj");

	rover_pinza = Model();
	rover_pinza.LoadModel("Models/rover_pinza.obj");

	rover_pata_der_frontal = Model();
	rover_pata_der_frontal.LoadModel("Models/rover_pata_der_frontal.obj");

	rover_pata_izq_frontal = Model();
	rover_pata_izq_frontal.LoadModel("Models/rover_pata_izq_frontal.obj");

	rover_pata_der_medio = Model();
	rover_pata_der_medio.LoadModel("Models/rover_pata_der_medio.obj");

	rover_pata_izq_medio = Model();
	rover_pata_izq_medio.LoadModel("Models/rover_pata_izq_medio.obj");

	rover_pata_der_trasera = Model();
	rover_pata_der_trasera.LoadModel("Models/rover_pata_der_trasera.obj");

	rover_pata_izq_trasera = Model();
	rover_pata_izq_trasera.LoadModel("Models/rover_pata_izq_trasera.obj");

	rover_llanta_der_frontal = Model();
	rover_llanta_der_frontal.LoadModel("Models/rover_llanta_der_frontal.obj");

	rover_llanta_izq_frontal = Model();
	rover_llanta_izq_frontal.LoadModel("Models/rover_llanta_izq_frontal.obj");

	rover_llanta_der_medio = Model();
	rover_llanta_der_medio.LoadModel("Models/rover_llanta_der_medio.obj");

	rover_llanta_izq_medio = Model();
	rover_llanta_izq_medio.LoadModel("Models/rover_llanta_izq_medio.obj");

	rover_llanta_der_trasera = Model();
	rover_llanta_der_trasera.LoadModel("Models/rover_llanta_der_trasera.obj");

	rover_llanta_izq_trasera = Model();
	rover_llanta_izq_trasera.LoadModel("Models/rover_llanta_izq_trasera.obj");

	//Carga de avion
	avion_texturizado = Model();
	avion_texturizado.LoadModel("Models/avion_texturizado.obj");

	//Carga de helice del avion
	helice_avion = Model();
	helice_avion.LoadModel("Models/avion_helice.obj");


	//Carga de la nave espacial
	nave_espacial = Model();
	nave_espacial.LoadModel("Models/nave_espacial.obj");

	//Carga de la pista de carreras
	pista_carreras = Model();
	pista_carreras.LoadModel(PISTA_MODELO);

	//Carga del satelite (jerarquia: cuerpo inferior > cuerpo superior y paneles)
	satelite_inf = Model();
	satelite_inf.LoadModel("Models/satelite_inf.obj");

	satelite_sup = Model();
	satelite_sup.LoadModel("Models/satelite_sup.obj");

	satelite_ala1 = Model();
	satelite_ala1.LoadModel("Models/panel_ala1.obj");

	satelite_ala2 = Model();
	satelite_ala2.LoadModel("Models/panel_ala2.obj");


	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);



	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//>>>>>>>>>>>>>>>>>>>>>> Luces / ilumninacion >>>>>>>>>>>>>>>>>>>>
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	CreaLuces();

	projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		now = glfwGetTime();

		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;

		//Movimiento del rover (ida y vuelta)
		if (avanza)
		{
			if (movCoche < limiteRoverMax)
			{
				movCoche += movOffset * deltaTime;
				rotLlanta -= rotLlantaIncrementa * deltaTime;
			}
			else
			{
				avanza = false;
			}
		}
		else
		{
			if (movCoche > limiteRoverMin)
			{
				movCoche -= movOffset * deltaTime;
				rotLlanta += rotLlantaIncrementa * deltaTime;
			}
			else
			{
				avanza = true;
			}
		}

		//Movimiento del avion
		actualizaAvion(deltaTime);

		//Giro de la helice
		anguloHelice += velHelice * deltaTime;
		if (anguloHelice > 360.0f)
			anguloHelice -= 360.0f;

		//Movimiento de la nave sobre la pista
		actualizaNave(deltaTime);

		//Movimiento y articulaciones del satelite
		actualizaSatelite(deltaTime);


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//informacion en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la camara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[SPOT_LINTERNA].SetFlash(lowerLight, camera.getCameraDirection());

		//>>>>>>>>>>>>>>>>>>>>>> Luces ligadas a los modelos >>>>>>>>>>>>>>>>>>>>

		// Ciclo de color del faro bajo
		if (mainWindow.getIndiceColorFaro() != indiceColorFaroAplicado)
		{
			indiceColorFaroAplicado = mainWindow.getIndiceColorFaro();
			spotLights[SPOT_FARO_BAJO_AVION] = crearFaroBajoAvion(COLORES_FARO[indiceColorFaroAplicado]);
		}

		// Jerarquia del avion
		modelAvion = glm::mat4(1.0f);
		modelAvion = glm::translate(modelAvion, glm::vec3(avionX, alturaAvion, avionZ));
		modelAvion = glm::rotate(modelAvion, avionYaw + orientacionModeloAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		modelAvionAux = modelAvion;

		// Helice (hija del avion)
		modelHelice = modelAvionAux;
		modelHelice = glm::translate(modelHelice, HELICE_AVION_POS);
		modelHelice = glm::rotate(modelHelice, anguloHelice * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		// Faro 1: bajo
		posBajoAvion = glm::vec3(modelAvionAux * glm::vec4(FARO_BAJO_AVION_POS, 1.0f));
		dirBajoAvion = glm::normalize(glm::vec3(modelAvionAux * glm::vec4(FARO_BAJO_AVION_DIR, 0.0f)));
		spotLights[SPOT_FARO_BAJO_AVION].SetFlash(posBajoAvion, dirBajoAvion);

		// Faro 2: frontal (delante de la helice)
		posFrenteAvion = glm::vec3(modelAvionAux * glm::vec4(FARO_FRENTE_AVION_POS, 1.0f));
		dirFrenteAvion = glm::normalize(glm::vec3(modelAvionAux * glm::vec4(FARO_FRENTE_AVION_DIR, 0.0f)));
		spotLights[SPOT_FARO_FRENTE_AVION].SetFlash(posFrenteAvion, dirFrenteAvion);

		// Jerarquia del rover (cuerpo como raiz)
		modelRoverAux = glm::mat4(1.0f);
		modelRoverAux = glm::translate(modelRoverAux, glm::vec3(movCoche, 0.97f, 10.0f));

		// Faro 3: frontal del rover
		posFrenteRover = glm::vec3(modelRoverAux * glm::vec4(FARO_FRENTE_ROVER_POS, 1.0f));
		dirFrenteRover = glm::normalize(glm::vec3(modelRoverAux * glm::vec4(FARO_FRENTE_ROVER_DIR, 0.0f)));
		spotLights[SPOT_FARO_ROVER].SetFlash(posFrenteRover, dirFrenteRover);

		// Luces de la nave (puntual, faro y motores)
		actualizaLucesNave();

		// Luces activas segun las teclas
		armaLucesActivas();

		//informacion al shader de fuentes de iluminacion
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLightsActivas, pointLightCountActivas);
		shaderList[0].SetSpotLights(spotLightsActivas, spotLightCountActivas);


		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);


		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMeshModel();


		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>> Rover modelo jerarquico >>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		//---------------- Cuerpo (raiz de la jerarquia) ----------------
		model = modelRoverAux;
		modelaux = model;
		color = glm::vec3(0.7f, 0.7f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_cuerpo.RenderModel();

		//>>>>>>>>>>>>>>>>>>>>>> Brazo: base > bajo > superior > pinzas

		//---------------- Brazo base (hijo de cuerpo) ----------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.0f, 1.7f, -1.1f));
		modelBrazo = model;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_brazo_base.RenderModel();

		//---------------- Brazo bajo (hijo de brazo base) ----------------
		model = modelBrazo;
		model = glm::translate(model, glm::vec3(0.7f, 1.09f, 0.0f));
		modelBrazo = model;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_brazo_bajo.RenderModel();

		//---------------- Brazo superior (hijo de brazo bajo) ----------------
		model = modelBrazo;
		model = glm::translate(model, glm::vec3(2.45f, 2.5f, 0.09f));
		modelBrazo = model;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_brazo_superior.RenderModel();



		//---------------- Pinza 1 (hija de brazo superior) ----------------
		model = modelBrazo;
		model = glm::translate(model, glm::vec3(-2.5f, 3.12f, -0.04f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_pinza.RenderModel();

		//>>>>>>>>>>>>>>>>>>>>>> Patas y llantas: pata > llanta

		//---------------- Pata derecha frontal (hija de cuerpo) ----------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, 0.97f, 3.1f));
		modelPata = model;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_pata_der_frontal.RenderModel();

		//---------------- Llanta derecha frontal (hija de pata derecha frontal) ----------------
		model = modelPata;
		model = glm::translate(model, glm::vec3(3.55f, -2.7f, 0.0f));
		model = glm::rotate(model, rotLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_llanta_der_frontal.RenderModel();

		//---------------- Pata izquierda frontal (hija de cuerpo) ----------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, 0.97f, -3.1f));
		modelPata = model;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_pata_izq_frontal.RenderModel();

		//---------------- Llanta izquierda frontal (hija de pata izquierda frontal) ----------------
		model = modelPata;
		model = glm::translate(model, glm::vec3(3.55f, -2.7f, 0.0f));
		model = glm::rotate(model, rotLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_llanta_izq_frontal.RenderModel();

		//---------------- Pata derecha medio (hija de cuerpo) ----------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.97f, -2.85f));
		modelPata = model;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_pata_der_medio.RenderModel();

		//---------------- Llanta derecha medio (hija de pata derecha medio) ----------------
		model = modelPata;
		model = glm::translate(model, glm::vec3(2.0f, -2.14f, -1.5f));
		model = glm::rotate(model, rotLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_llanta_der_medio.RenderModel();

		//---------------- Pata izquierda medio (hija de cuerpo) ----------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 0.97f, 2.85f));
		modelPata = model;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_pata_izq_medio.RenderModel();

		//---------------- Llanta izquierda medio (hija de pata izquierda medio) ----------------
		model = modelPata;
		model = glm::translate(model, glm::vec3(2.0f, -2.14f, 1.5f));
		model = glm::rotate(model, rotLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_llanta_izq_medio.RenderModel();

		////---------------- Pata derecha trasera (hija de cuerpo) ----------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.0f, 0.97f, -2.85f));
		modelPata = model;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_pata_der_trasera.RenderModel();

		//---------------- Llanta derecha trasera (hija de pata derecha trasera) ----------------
		model = modelPata;
		model = glm::translate(model, glm::vec3(-2.0f, -2.14f, -1.5f));
		model = glm::rotate(model, rotLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_llanta_der_trasera.RenderModel();

		//---------------- Pata izquierda trasera (hija de cuerpo) ----------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.0f, 0.97f, 2.85f));
		modelPata = model;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_pata_izq_trasera.RenderModel();

		//---------------- Llanta izquierda trasera (hija de pata izquierda trasera) ----------------
		model = modelPata;
		model = glm::translate(model, glm::vec3(-2.0f, -2.14f, 1.5f));
		model = glm::rotate(model, rotLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rover_llanta_izq_trasera.RenderModel();


		//Instancia del rover
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche + .0f, -1.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Rover_M.RenderModel();


		//Agave que sucede si lo renderizan antes del Rover?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMeshModel();

		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMeshModel();

		//plano con todos los numeros
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		//numero 1
		//toffsetnumerou = 0.0;
		//toffsetnumerov = 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		//glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		for (contadorI = 1; contadorI < 4; contadorI++)
		{
			//numeros 2-4
			toffsetnumerou += 0.25;
			toffsetnumerov = 0.0;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (contadorI * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();

		}

		for (contadorJ = 1; contadorJ < 5; contadorJ++)
		{
			//numeros 5-8
			toffsetnumerou += 0.25;
			toffsetnumerov = -0.33;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (contadorJ * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();
		}


		//numero cambiante 
		/*
		Como hacer para que sea a una velocidad visible?
		*/
		toffsetnumerocambiau += 0.25;
		if (toffsetnumerocambiau > 1.0)
			toffsetnumerocambiau = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		//cambiar automaticamente entre textura numero 1 y numero 2
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Numero1Texture.UseTexture();
		//if
		//Numero1Texture.UseTexture();
		//Numero2Texture.UseTexture();

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		glDisable(GL_BLEND);

		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>> Avion >>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



		//---------------- Avion completo ----------------
		model = modelAvionAux;
		//model = glm::translate(model, glm::vec3(avionX, alturaAvion, avionZ));
		//model = glm::rotate(model, avionYaw + orientacionModeloAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		toffset = glm::vec2(0.0f, 0.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		avion_texturizado.RenderModel();

		//---------------- Helice (hija del avion, gira sobre su eje) ----------------
		model = modelHelice;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		helice_avion.RenderModel();



		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>> Pista de carreras >>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(glm::mat4(1.0f), PISTA_OFFSET);
		model = glm::scale(model, glm::vec3(PISTA_ESCALA, PISTA_ESCALA, PISTA_ESCALA));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		pista_carreras.RenderModel();

		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>> Nave espacial sobre la pista >>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelNave;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		naveTexture.UseTexture();
		nave_espacial.RenderModel();

		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>> Satelite modelo jerarquico >>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		//Satelite cuerpo inferior (raiz de la jerarquia)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(satX, SAT_ALTURA, satZ));
		model = glm::rotate(model, satYaw + SAT_YAW_MODELO, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		color = glm::vec3(0.7f, 0.7f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		satelite_inf.RenderModel();

		//Satelite cuerpo superior (articulacion automatica sobre Y)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.28f, 0.0f));
		model = glm::rotate(model, satAngSup * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		satelite_sup.RenderModel();

		//Satelite panel 1 (articulacion automatica sobre X)
		model = modelaux;
		model = glm::translate(model, glm::vec3(5.9f, 0.5f, 0.0f));
		model = glm::rotate(model, satAngAla * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.7f, 0.7f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		satelite_ala1.RenderModel();

		//Satelite panel 2 (articulacion automatica sobre X)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-5.8f, 0.5f, 0.0f));
		model = glm::rotate(model, satAngAla * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.7f, 0.7f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		satelite_ala2.RenderModel();

		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>> Fuego de los motores de la nave >>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		color = FUEGO_BRILLO;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		fuegoTexture.UseTexture();
		if (mainWindow.getMotor1Encendido()) dibujarFuego(MOTOR1_POS);
		if (mainWindow.getMotor2Encendido()) dibujarFuego(MOTOR2_POS);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}