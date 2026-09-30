/*
Práctica 5: Optimización y Carga de Modelos
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
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList; //solo recibe xyz
std::vector<MeshColor*> meshListColor; // recibe xyz rgb
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;
//Lista de Modelos a importar
Model Rover_M;
Model holocron_c;
Model holocron_s1;
Model holocron_s2;
Model holocron_s3; 
Model holocron_s4; 
Model holocron_i1; 
Model holocron_i2; 
Model holocron_i3; 
Model holocron_i4;
Model satelite_ala1;
Model satelite_ala2;
Model satelite_sup;
Model satelite_inf;

//Lista de Skybox a crear
Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";


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

	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);



	//>>>>>Cargar modelos<<<<<<<

	//Centro de de holocron

/*	Rover_M = Model();
	Rover_M.LoadModel("Models/holocron_inf1.obj");*/

	holocron_c = Model();
	holocron_c.LoadModel("Models/holocron_centro.obj");

	holocron_s1 = Model();
	holocron_s1.LoadModel("Models/holocron_sup1.obj");

	holocron_s2 = Model();
	holocron_s2.LoadModel("Models/holocron_sup2.obj");

	holocron_s3 = Model();
	holocron_s3.LoadModel("Models/holocron_sup3.obj");

	holocron_s4 = Model();
	holocron_s4.LoadModel("Models/holocron_sup4.obj");

	holocron_i1 = Model();
	holocron_i1.LoadModel("Models/holocron_inf1.obj");

	holocron_i2 = Model();
	holocron_i2.LoadModel("Models/holocron_inf2.obj");

	holocron_i3 = Model();
	holocron_i3.LoadModel("Models/holocron_inf3.obj");

	holocron_i4 = Model();
	holocron_i4.LoadModel("Models/holocron_inf4.obj");

	satelite_ala1 = Model();
	satelite_ala1.LoadModel("Models/panel_ala1.obj");

	satelite_ala2 = Model();
	satelite_ala2.LoadModel("Models/panel_ala2.obj");

	satelite_inf = Model();
	satelite_inf.LoadModel("Models/satelite_inf.obj");

	satelite_sup = Model();
	satelite_sup.LoadModel("Models/satelite_sup.obj");

	//Crear Skybox con sus 6 texturas
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	////Loop mientras no se cierra la ventana
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
		//Se dibuja el Skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// INICIA DIBUJO DEL PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f); //piso de color gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[2]->RenderMeshModel();



		//---------------------------------------
		//------------*HOLOCRON------------------
		//---------------------------------------
		

		//Holocron centro
		color = glm::vec3(0.0f, 0.0f, 1.0f); //modelo de color azul
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 3.5f, 0.0f));
		model = glm::rotate(model, glm::radians(45.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));//rotar alrededor de z 90°
		modelaux = model;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocron_c.RenderModel();//modificar por el modelo de solo cuerpo del Rover, para que se pueda separar el brazo y las llantas
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		//En sesión se separara una parte del modelo y se unirá por jeraquía al cuerpo	
		
		//Holocron esquina sup 1
        
		color = glm::vec3(1.0f, 0.0f, 0.0f); //modelo de color azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.9f, 4.0f, -3.9f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()),
			glm::vec3(0.50f, 0.50f, -0.50f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocron_s1.RenderModel();
		color = glm::vec3(1.0f, 0.0f, 0.0f);

		//Holocron esquina sup 2

		color = glm::vec3(1.0f, 0.0f, 0.0f); //modelo de color azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.9f, 4.0f, -3.9f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()),
			glm::vec3(-0.50f, 0.50f, -0.50f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocron_s2.RenderModel();
		color = glm::vec3(1.0f, 0.0f, 0.0f);


		//Holocron esquina sup 3

		color = glm::vec3(1.0f, 0.0f, 0.0f); //modelo de color azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.0f, 3.8f, 4.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()),
			glm::vec3(-0.50f, 0.50f, 0.50f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocron_s3.RenderModel();
		color = glm::vec3(1.0f, 0.0f, 0.0f);


		//Holocron esquina sup 4

		color = glm::vec3(1.0f, 0.0f, 0.0f); //modelo de color azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.0f, 3.8f, 4.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()),
			glm::vec3(0.50f, 0.50f, 0.50f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocron_s4.RenderModel();
		color = glm::vec3(1.0f, 0.0f, 0.0f);


		//Holocron esquina inf 1

		color = glm::vec3(1.0f, 0.0f, 0.0f); //modelo de color azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.9f, -4.0f, -3.9f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()),
			glm::vec3(0.50f, -0.50f, -0.50f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocron_i1.RenderModel();
		color = glm::vec3(1.0f, 0.0f, 0.0f);

		//Holocron esquina inf 2

		color = glm::vec3(1.0f, 0.0f, 0.0f); //modelo de color azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.9f, -4.0f, -3.9f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()),
			glm::vec3(-0.50f, -0.50f, -0.50f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocron_i2.RenderModel();
		color = glm::vec3(1.0f, 0.0f, 0.0f);

		//Holocron esquina inf 3

		color = glm::vec3(1.0f, 0.0f, 0.0f); //modelo de color azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.0f, -3.8f, 4.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()),
			glm::vec3(-0.50f, -0.50f, 0.50f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocron_i3.RenderModel();
		color = glm::vec3(1.0f, 0.0f, 0.0f);

		//Holocron esquina inf 4

		color = glm::vec3(1.0f, 0.0f, 0.0f); //modelo de color azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.0f, -3.8f, 4.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()),
			glm::vec3(0.50f, -0.50f, 0.50f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocron_i4.RenderModel();
		color = glm::vec3(1.0f, 0.0f, 0.0f);


		//---------------------------------------
		//-------------SATELITE------------------
		//---------------------------------------



	    //Satelite cuerpo inferior
		model = glm::mat4(1.0);
		color = glm::vec3(0.7f, 0.7f, 0.7f);
		model = glm::translate(model, glm::vec3(24.0f, 3.8f, 4.0f));
		//parte de traslacion en x, y, z para el satelite junto con los paneles solares y la punta que estan unidas
		//mediante jerarquia
		model = glm::translate(model, glm::vec3(mainWindow.getmovx(), mainWindow.getmovy(), mainWindow.getmovz()));
		modelaux = model; 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		satelite_inf.RenderModel();
		color = glm::vec3(0.7f, 0.7f, 0.7f);


		//Satelite cuerpo sup

		model = modelaux;
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		model = glm::translate(model, glm::vec3(0.0f, 1.28f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()),
			glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		satelite_sup.RenderModel();
		color = glm::vec3(0.3f, 0.3f, 0.3f);


		//Satelite ala 1
		model = modelaux;
		color = glm::vec3(0.7f, 0.7f, 1.0f);
		model = glm::translate(model, glm::vec3(5.9f, 0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion10()),
		glm::vec3(1.0f, 0.0f, 0.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		satelite_ala1.RenderModel();
		color = glm::vec3(0.7f, 0.7f, 1.0f);

		//Satelite ala 2

		model = modelaux;
		color = glm::vec3(0.7f, 0.7f, 1.0f);
		model = glm::translate(model, glm::vec3(-5.8f, 0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion11()),
		glm::vec3(1.0f, 0.0f, 0.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		satelite_ala2.RenderModel();
		color = glm::vec3(0.7f, 0.7f, 1.0f);


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
