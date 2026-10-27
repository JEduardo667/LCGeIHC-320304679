#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getavionAvance() { return avionAvance; }
	GLfloat getavionAltura() { return avionAltura; }
	GLint getIndiceColorFaro() { return indiceColorFaro; }
	bool getLuzHolocronEncendida() { return luzHolocronEncendida; }
	bool getLuzPinzaEncendida() { return luzPinzaEncendida; }
	bool getLuzNaveEncendida() { return luzNaveEncendida; }
	bool getMotor1Encendido() { return motor1Encendido; }
	bool getMotor2Encendido() { return motor2Encendido; }



	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);
	}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	~Window();
private:
	GLFWwindow* mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat avionAvance;
	GLfloat avionAltura;
	GLint indiceColorFaro;
	bool luzHolocronEncendida;
	bool luzPinzaEncendida;
	bool luzNaveEncendida;
	bool motor1Encendido;
	bool motor2Encendido;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};