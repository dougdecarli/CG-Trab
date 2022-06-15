#ifndef SYSTEM_H
#define SYSTEM_H

// Internal
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>

// External Libs
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers
#include "Shader.h"
#include "AssetManager.h"
#include "Time.h"
#include "Mesh.h"
#include "TranslatePointsReader.h"
#include "ObjReader.h"
#include "Camera.h"

class System
{
private:
    // Screen
    const GLint WIDTH = 1920, HEIGHT = 1080;
    int screenWidth, screenHeight;

public:
    GLFWwindow* window;
    Shader coreShader;

public:
    System();
    ~System();

    int GLFWInit();
    int OpenGLSetup();
    int SystemSetup();

    void Run(vector<Mesh*> meshs, std::shared_ptr<Camera> cam);

    void Finish();

};

#endif
