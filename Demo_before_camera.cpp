#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


#include"EBO.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"

const unsigned int width = 800;
const unsigned int height = 800;

// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      / 
	-0.5f, 0.0f,  0.5f,     0.0f, 0.0f, 1.0f,
	-0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.f,
	 0.5f, 0.0f, -0.5f,     1.0f, 0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.21f
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};

int main() {

	// Crear la ventana
	glfwInit();

	// Versiones de GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// widht, heigh, name, full_scree? , NO_IMPORTANT
	GLFWwindow* window = glfwCreateWindow(width, height, "Puppet", NULL, NULL);
	if (window == nullptr) {
		std::cout << "failed to create the window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Indicarle que esta ventana es la actual
	glfwMakeContextCurrent(window);


	gladLoadGL();
	glViewport(0, 0, width, height);

	Shader shaderProgram("default.vert", "default.frag");

	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	//Los grados que se va a ir moviendo la piramide y contador de tiempo
	float rotation =  0.0f;
	double prevTime = glfwGetTime();
	//Desactiva el buffer de profundidad
	glEnable(GL_DEPTH_TEST);

	int i = 0;

	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();


		// Simple timer
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60)
		{
			rotation += 0.01f;
			prevTime = crntTime;
		}

		// Initializes matrices so they are not the null matrix
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		// donde voy a guarda la matriz de rotacion, radianes que quiero rotar, sobre que eje
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		// Movemos el mundo entero en la direccion del segundo aprametro
		view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));

		// campo de vision, las medidas de la pantalla y la minima y maxima distancia a la que podemos ver
		proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		// Outputs the matrices into the Vertex Shader
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		// Location, 1, GL_FALSE para que no la haga transpuesta, 
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 0.5f);
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
