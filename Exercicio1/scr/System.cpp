#include "System.h"
#include "Mesh.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

System::System() {}

System::~System() {}

int System::GLFWInit()
{
    glfwInit();
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow( WIDTH, HEIGHT, "Sabertooth", nullptr, nullptr );

	glfwGetFramebufferSize( window, &screenWidth, &screenHeight );

	if ( window == nullptr ) {
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent( window );

	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	glewExperimental = GL_TRUE;

	if ( glewInit() != GLEW_OK ) {
		std::cout << "Failed no init GLEW." << std::endl;
		return EXIT_FAILURE;
	}

	glViewport( 0, 0, screenWidth, screenHeight );

	return EXIT_SUCCESS;

}

int System::OpenGLSetup()
{

	glEnable( GL_BLEND );	// Enables blending ( glBlendFunc )
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glEnable( GL_DEPTH_TEST );

	return EXIT_SUCCESS;
}

int System::SystemSetup()
{

	coreShader = Shader( "/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Shaders/core.vert", "/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Shaders/core.frag" );
	coreShader.Use();

	return EXIT_SUCCESS;
}

void System::Run(map<string, Mesh*> meshs, map<string, char*> textures, string initial)
{
	Mesh* mesh = meshs.find(initial)->second;
	char* texturePath = textures.find(initial)->second;
    
    std::shared_ptr<Camera> cam;
    cam = std::make_shared<Camera>();
    Camera::bind_instance (cam);
    
    glfwSetCursorPosCallback (window, Camera::mouse_callback);
    glfwSetScrollCallback (window, Camera::scroll_callback);

	coreShader.Use();
	coreShader.LoadTexture(texturePath, "texture1", "woodTexture");

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

	coreShader.setMatrix4fv("projection", proj);

	for (Group* group : mesh->getGroups()) {
		vector<float> vertices;
		vector<float> normais;
		vector<float> textures;

		for (Face* face : group->getFaces()) {
			for (int verticeID : face->getVertices()) {
				glm::vec3* vertice = mesh->vertice(verticeID - 1);
				vertices.push_back(vertice->x);
				vertices.push_back(vertice->y);
				vertices.push_back(vertice->z);

				group->increaseNumVertices();
			}

			for (int normalID : face->getNormais()) {
				glm::vec3* normal = mesh->normal(normalID - 1);
				normais.push_back(normal->x);
				normais.push_back(normal->y);
				normais.push_back(normal->z);
			}

			for (int textureID : face->getTextures()) {
				glm::vec2* texture = mesh->texture(textureID - 1);
				textures.push_back(texture->x);
				textures.push_back(texture->y);
			}
		}

		GLuint VBOvertices, VBOnormais, VBOtextures, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBOvertices);
		glGenBuffers(1, &VBOnormais);
		glGenBuffers(1, &VBOtextures);

		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		// Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VBOvertices);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
		glEnableVertexAttribArray(0);

		// Normais
		glBindBuffer(GL_ARRAY_BUFFER, VBOnormais);
		glBufferData(GL_ARRAY_BUFFER, normais.size() * sizeof(float), normais.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
		glEnableVertexAttribArray(1);

		// Textures
		glBindBuffer(GL_ARRAY_BUFFER, VBOtextures);
		glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float), textures.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*) 0);
		glEnableVertexAttribArray(2);
		
		group->setVAO(&VAO);
	}

	float camX = 5.0f;
	float camY = 0.5f;
	float camZ = 5.0f;

	float angle = 20.0f;
	float translateX = 0.0f;
	float translateY = 0.0f;
	float translateZ = 0.0f;
	
	while ( !glfwWindowShouldClose( window ) ) {

		glfwPollEvents();

		if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
			glfwSetWindowShouldClose( window, GLFW_TRUE );
		}
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cam->cameraPos += cam->cameraSpeed * cam->cameraFront;
            cam->cameraUpdated = true;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cam->cameraPos -= cam->cameraSpeed * cam->cameraFront;
            cam->cameraUpdated = true;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cam->cameraPos -= glm::normalize(glm::cross(cam->cameraFront, cam->cameraUp)) * cam->cameraSpeed;
            cam->cameraUpdated = true;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cam->cameraPos += glm::normalize(glm::cross(cam->cameraFront, cam->cameraUp)) * cam->cameraSpeed;
            cam->cameraUpdated = true;
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            translateX += 0.09f;
        }

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            translateZ += 0.09f;
        }

        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
            translateY += 0.09f;
        }

		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			cout << "X: " << translateX << " - Y: " << translateY << " - Z: " << translateZ << endl;
		}

		glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		coreShader.Use();

		glm::mat4 view(1.0f);
		view = glm::lookAt (cam->cameraPos, cam->cameraPos+cam->cameraFront, cam->cameraUp);

		coreShader.setMatrix4fv("view", view);

		coreShader.setVec3("lightColor", vec3(1.0f, 1.0f, 1.0f));
		coreShader.setVec3("lightPos", vec3(-5.0f, -5.0f, -10.0f));
		coreShader.setVec3("viewPos", vec3(camX, camY, camZ));

		glm::mat4 model(1.0f);
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));

		coreShader.setMatrix4fv("model", model);
		coreShader.UseTexture("woodTexture");

		for (Group* group : mesh->getGroups()) {
			glBindVertexArray(group->getVAO());
			glDrawArrays(GL_TRIANGLES, 0, group->getNumVertices());
		}

		glfwSwapBuffers(window);
	}
}

void System::Finish()
{
	coreShader.Delete();

	glfwTerminate();
}
