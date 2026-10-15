/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

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

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture StarWarsTexture;
Texture avionTexture;

// Añadir Texturas del dado y del holocron


Model Blackhawk_M;
Model Rover_M; //Sustituir por sus propios Rovers Con jerarquía
//Añadir modelo del avión.
//Model Avion_M;
Model avion;
Model cuerpo_rover;
Model brazo_rover;
Model pata_der_frontal;
Model pata_izq_frontal;
Model pata_der_mitad;
Model pata_izq_mitad;
Model pata_der_trasera;
Model pata_izq_trasera;



Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
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
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		1.0f, 0.0f, -1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		1.0f, 0.0f, -1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		1.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		1.0f, 0.0f, -1.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,


	};
	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	MeshModel *obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado() //sustituir con su propio dado, para que tenga la textura de la práctica 6
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.0f,  0.75f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.25f,	0.75f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.25f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.25f,  0.75f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.5f,	0.75f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.5f,	1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.25f,	1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		//x		y		z		S		T
		-0.5f, -0.5f, -0.5f,	0.5f,  0.75f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.75f,	0.75f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.75f,	1.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.5f,	1.0f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.75f,  0.75f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,	0.75f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.75f,	1.0f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,  0.5f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.25f,	0.5f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.25f,	0.75f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.75f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.25f,  0.5f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.5f,	0.5f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.5f,	0.75f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.25f,	0.75f,		0.0f,	-1.0f,	0.0f,

	};

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(dado);

}


void CrearHolocron()
{
	GLfloat holocron_vertices[] = {

		// --- PARTE CARAS CUADRADAS ---

		// x         y         z           S        T        NX      NY      NZ
		-2.500f,  2.500f,  0.000f,    0.0f, 0.875f,   0.0f,   -1.0f,   0.0f,
		 0.000f,  2.500f,  2.500f,    0.125f, 0.75f,   0.0f,   -1.0f,   0.0f,
		 2.500f,  2.500f,  0.000f,    0.25f, 0.875f,   0.0f,   -1.0f,   0.0f,
		 0.000f,  2.500f, -2.500f,    0.125f, 1.0f,   0.0f,   -1.0f,   0.0f,

		-2.500f, -2.500f,  0.000f,    0.25f, 0.875f,   0.0f,  1.0f,   0.0f,
		 0.000f, -2.500f, -2.500f,    0.375f, 0.75f,   0.0f,  1.0f,   0.0f,
		 2.500f, -2.500f,  0.000f,    0.5f, 0.875f,   0.0f,  1.0f,   0.0f,
		 0.000f, -2.500f,  2.500f,    0.375f, 1.0f,   0.0f,  1.0f,   0.0f,



		-2.500f,  0.000f,  2.500f,    0.5, 0.875f,   0.0f,   0.0f,   -1.0f,
		 0.000f, -2.500f,  2.500f,    0.625f, 0.75f,   0.0f,   0.0f,   -1.0f,
		 2.500f,  0.000f,  2.500f,    0.75f, 0.875f,   0.0f,   0.0f,   -1.0f,
		 0.000f,  2.500f,  2.500f,    0.625f, 1.0f,   0.0f,   0.0f,   -1.0f,

		 2.500f,  0.000f, -2.500f,    0.75f, 0.875f,   0.0f,   0.0f,  1.0f,
		 0.000f, -2.500f, -2.500f,    0.875f, 0.75f,   0.0f,   0.0f,  1.0f,
		-2.500f,  0.000f, -2.500f,    1.0f, 0.875f,   0.0f,   0.0f,  1.0f,
		 0.000f,  2.500f, -2.500f,    0.875f, 1.0f,   0.0f,   0.0f,  1.0f,

		 2.500f, -2.500f,  0.000f,    0.0f, 0.625f,   -1.0f,   0.0f,   0.0f,
		 2.500f,  0.000f,  2.500f,    0.125f, 0.5f,   -1.0f,   0.0f,   0.0f,
		 2.500f,  2.500f,  0.000f,    0.25f, 0.625f,  -1.0f,   0.0f,   0.0f,
		 2.500f,  0.000f, -2.500f,    0.125f, 0.75f,   -1.0f,   0.0f,   0.0f,


		-2.500f, -2.500f,  0.000f,    0.25f, 0.625f,  1.0f,   0.0f,   0.0f,
		-2.500f,  0.000f, -2.500f,    0.375f, 0.5f,  1.0f,   0.0f,   0.0f,
		-2.500f,  2.500f,  0.000f,    0.5f, 0.625f,  1.0f,   0.0f,   0.0f,
		-2.500f,  0.000f,  2.500f,    0.375f, 0.75f,  1.0f,   0.0f,   0.0f,

		// --- PARTE DE CARAS TRIANGULARES ---

	     2.500f,  2.500f,  0.000f,    0.75f, 0.5f,   -0.577f, -0.577f, -0.577f,
	     2.500f,  0.000f,  2.500f,    0.5f, 0.5f,   -0.577f, -0.577f, -0.577f,
	     0.000f,  2.500f,  2.500f,    0.625f, 0.8f,   -0.577f, -0.577f, -0.577f,
		
	    -2.500f,  0.000f,  2.500f,    1.0f, 0.5f,  0.577f, -0.577f, -0.577f,
	    -2.500f,  2.500f,  0.000f,    0.75f, 0.5f,  0.577f, -0.577f, -0.577f,
	     0.000f,  2.500f,  2.500f,    0.875f, 0.75f,  0.577f, -0.577f, -0.577f,

	     0.000f, -2.500f,  2.500f,    0.25f, 0.25f,   -0.577f, 0.577f, -0.577f,
	     2.500f, -2.500f,  0.000f,    0.0f, 0.25f,   -0.577f, 0.577f, -0.577f,
	     2.500f,  0.000f,  2.500f,    0.125f, 0.55f,   -0.577f, 0.577f, -0.577f,

	    -2.500f, -2.500f,  0.000f,    0.5f, 0.25f,  0.577f,0.577f, -0.577f,
	     0.000f, -2.500f,  2.500f,    0.25f, 0.25f,  0.577f,0.577f, -0.577f,
	    -2.500f,  0.000f,  2.500f,    0.375f, 0.55f,  0.577f,0.577f, -0.577f,

	     2.500f,  0.000f, -2.500f,    1.0f, 0.25f,   -0.577f, -0.577f,0.577f,
	     2.500f,  2.500f,  0.000f,    0.75f, 0.25f,   -0.577f, -0.577f,0.577f,
	     0.000f,  2.500f, -2.500f,    0.875f, 0.55f,   -0.577f, -0.577f,0.577f,



	    -2.500f,  2.500f,  0.000f,    0.75f, 0.25f,  0.577f, -0.577f,0.577f,
	    -2.500f,  0.000f, -2.500f,    0.5f, 0.25f,  0.577f, -0.577f,0.577f,
	     0.000f,  2.500f, -2.500f,    0.625f, 0.55f,  0.577f, -0.577f,0.577f,

	     2.500f, -2.500f,  0.000f,    0.25f, 0.0f,   -0.577f,0.577f,0.577f,
	     0.000f, -2.500f, -2.500f,    0.0f, 0.0f,   -0.577f,0.577f,0.577f,
	     2.500f,  0.000f, -2.500f,    0.125f, 0.3f,   -0.577f,0.577f,0.577f,

	     0.000f, -2.500f, -2.500f,    0.5f, 0.0f,  0.577f,0.577f,0.577f,
	    -2.500f, -2.500f,  0.000f,    0.25f, 0.0f,  0.577f,0.577f,0.577f,
	    -2.500f,  0.000f, -2.500f,    0.375f, 0.3f,  0.577f,0.577f,0.577f
	};

	unsigned int holocron_indices[] = {
		// Cuadrados 
		0,  1,  2,   2,  3,  0,
		4,  5,  6,   6,  7,  4,
		8,  9,  10,  10, 11, 8,
		12, 13, 14,  14, 15, 12,
		16, 17, 18,  18, 19, 16,
		20, 21, 22,  22, 23, 20,

		// Triángulos 
		24, 25, 26,
		27, 28, 29,
		30, 31, 32,
		33, 34, 35,
		36, 37, 38,
		39, 40, 41,
		42, 43, 44,
		45, 46, 47
	};

	MeshModel* holocron = new MeshModel();
	holocron->CreateMeshModel(holocron_vertices, holocron_indices, 384, 60);
	meshListModel.push_back(holocron);
} 



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects(); 
	CrearDado();
	CrearHolocron();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

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
	StarWarsTexture = Texture("Textures/starwars_texturas_4x4.tga");
	StarWarsTexture.LoadTextureA();
	avionTexture = Texture("Textures/avion_texturas_escalado.tga");
	avionTexture.LoadTexture();

	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	
	Rover_M = Model();
	Rover_M.LoadModel("Models/rovermodificado.obj");

	//Carga de modelo de avion texturizado 
	avion = Model();
	avion.LoadModel("Models/avion_texturizado.obj");

	cuerpo_rover = Model();
	cuerpo_rover.LoadModel("Models/cuerpo_rover.obj");

	brazo_rover = Model();
	brazo_rover.LoadModel("Models/brazo_rover.obj");

	pata_der_frontal= Model();
	pata_der_frontal.LoadModel("Models/pata_der_frontal.obj");

	pata_izq_frontal = Model();
	pata_izq_frontal.LoadModel("Models/pata_izq_frontal.obj");

	pata_der_mitad = Model();
	pata_der_mitad.LoadModel("Models/pata_der_mitad.obj");

	pata_izq_mitad = Model();
	pata_izq_mitad.LoadModel("Models/pata_izq_mitad.obj");

	pata_der_trasera = Model();
	pata_der_trasera.LoadModel("Models/pata_der_trasera.obj");

	pata_izq_trasera = Model();
	pata_izq_trasera.LoadModel("Models/pata_izq_trasera.obj");
	//Añadir modelo del avión
	//Avion_M = Model();
	//Avion_M.LoadModel("Models/");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	const glm::vec3 AVANCE = glm::vec3(1.0f, 0.0f, 0.0f);   
	glm::vec3 avionBase = glm::vec3(5.0f, 10.0f, -22.0f);    

	const glm::vec3 ROVER_AVANCE = glm::vec3(1.0f, 0.0f, 0.0f);  
	glm::vec3 roverBase = glm::vec3(5.0f, 2.7f, 3.5f);          



	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>LUCES>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


	//luz direccional, sólo 1 y siempre debe de existir

	//>>>>>>>>>>>>>>>>>>>> LUZ DEL SOL <<<<<<<<<<<<<<<<<<<<<<
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.6f,
		1.0f, -1.0f, -1.0f)	;
	//contador de luces puntuales
	unsigned int pointLightCount = 0;



	//>>>>>>>>>>>>>>>>>>>> LUZ PUNTUAL 1 (LUZ ROJA) <<<<<<<<<<<<<<<<<<<<<<
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f); //no pueen ser los tres 0 (0f,0f,0f) debido a que puede causar indeterminacion
	pointLightCount++;


	//>>>>>>>>>>>>>>>>>>>> LUZ SPOT 1 (LINTERNA DE LA CAMARA) <<<<<<<<<<<<<<<<<<<<<<
	unsigned int spotLightCount = 0;
	spotLights[0] = SpotLight(0.5f, 0.5f, 0.5f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//>>>>>>>>>>>>>>>>>>>> LUZ SPOT 2 (FARO DE FRONTAL DEL AVION)  <<<<<<<<<<<<<<<<<<<<<<
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		0.0f, 4.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		45.0f);
	spotLightCount++;

	//>>>>>>>>>>>>>>>>>>>> LUZ SPOT 3 (FARO DEL AVION BAJO) <<<<<<<<<<<<<<<<<<<<<<
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f,    
		0.0f, 3.0f,                                
		0.0f, 0.0f, 0.0f,                         
		0.0f, -1.0f, 0.0f,                        
		1.0f, 0.0f, 0.0f,                       
		25.0f);                                   
	spotLightCount++;

	//>>>>>>>>>>>>>>>>>>>> LUZ SPOT 3 (FARO DE FRONTAL DEL ROVER) <<<<<<<<<<<<<<<<<<<<<<
	spotLights[3] = SpotLight(1.0f, 0.0f, 1.0f,
		0.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		45.0f);
	spotLightCount++;   




	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

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

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
			

		// >>>>>>>>> Jerarquía del avión <<<<<<<<<<<
		GLfloat altura = mainWindow.getavionAltura();

		glm::vec3 avionPos = avionBase
			+ AVANCE * mainWindow.getavionAvance()
			+ glm::vec3(0.0f, altura, 0.0f);


		glm::mat4 modelAvion = glm::mat4(1.0f);
		modelAvion = glm::translate(modelAvion, avionPos);
		glm::mat4 modelAvionAux = modelAvion;                        
		modelAvion = glm::scale(modelAvion, glm::vec3(1.5f, 1.5f, 1.5f));

		// Faro 1: bajo
		glm::vec3 posAbajo = glm::vec3(modelAvionAux * glm::vec4(0.0f, -0.3f, 0.0f, 1.0f));
		glm::vec3 dirAbajo = glm::normalize(glm::vec3(modelAvionAux * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)));
		spotLights[2].SetFlash(posAbajo, dirAbajo);

		// Faro 2: delantero 
		glm::vec3 AVANCE = glm::vec3(-1.0f, 0.0f, 0.0f);
		glm::vec3 posFrente = glm::vec3(modelAvionAux * glm::vec4(AVANCE * 1.0f, 1.0f));
		glm::vec3 dirFrente = glm::normalize(glm::vec3(modelAvionAux * glm::vec4(AVANCE, 0.0f)));
		spotLights[3].SetFlash(posFrente, dirFrente);


		// >>>>>>>>> Jerarquía del rover <<<<<<<<<<<
		glm::vec3 roverPos = roverBase + ROVER_AVANCE * mainWindow.getmuevex();

		glm::mat4 modelRoverAux = glm::mat4(1.0f);
		modelRoverAux = glm::translate(modelRoverAux, roverPos);
		modelRoverAux = glm::rotate(modelRoverAux, glm::radians(180.0f),
						glm::vec3(0.0f,1.0f,0.0f));

		// Faro delantero
		glm::vec3 posFaroRover = glm::vec3(modelRoverAux * glm::vec4(4.0f, 1.0f, 0.0f, 1.0f));
		glm::vec3 dirFaroRover = glm::normalize(glm::vec3(modelRoverAux * glm::vec4(ROVER_AVANCE, 0.0f)));
		spotLights[1].SetFlash(posFaroRover, dirFaroRover);


		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[2]->RenderMeshModel();
	

		//Agave ¿qué sucede si lo renderizan antes del helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[3]->RenderMeshModel();
		glDisable(GL_BLEND);


		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>>>Cubo con texturizado de logos de StarWars por codigo>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-8.5f, 0.42f, -3.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		StarWarsTexture.UseTexture();
		meshListModel[4]->RenderMeshModel();

		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>Holocron con texturizado de logos de StarWars por codigo>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-14.0f, 0.6f, -3.0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		StarWarsTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess); 
		meshListModel[5]->RenderMeshModel();


		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>> Rover modelo jerarquico >>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		// Cuerpo del rover (parte central), la mas importante por jerarquia 
		model = modelRoverAux;
		modelaux = model;
		color = glm::vec3(0.7f, 0.7f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cuerpo_rover.RenderModel();


		// rover brazo
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.9f, 2.2f, -1.1f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazo_rover.RenderModel();

		//	rover llanta derecha frontal
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5f, -0.5f, 2.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pata_der_frontal.RenderModel();

		//	rover llanta izquierda frontal
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5f, -0.5f, -2.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pata_izq_frontal.RenderModel();

		//	rover llanta derecho mitad
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.5f, -0.5f, 2.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pata_der_mitad.RenderModel();

		//	rover llanta izquierda mitad
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.5f, -0.5f, -2.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pata_izq_mitad.RenderModel();

		//	rover llanta derecho atras
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.5f, -0.5f, 2.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pata_der_trasera.RenderModel();

		//	rover llanta izquierda atras
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.5f, -0.5f, -2.0f));
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pata_der_trasera.RenderModel();

		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		//>>>>>>>>>>>>>Modelo de Avion del universo de cars/planes>>>>>>>>>>>
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelAvionAux;
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		avionTexture.UseTexture();
		avion.RenderModel();




		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
