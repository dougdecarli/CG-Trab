#include "System.h"
#include "Mesh.h"
#include "camera.h"

enum RotationStatus {
    none, X, Y, Z
};

System::System() {}

System::~System() {}

int System::GLFWInit()
{
    
    glfwInit();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    glfwWindowHint( GLFW_SAMPLES, 4 );
    
    window = glfwCreateWindow( WIDTH, HEIGHT, "LEITOR/VISUALIZADOR DE CENAS 3D COM OPENGL MODERNA", nullptr, nullptr );
    
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
    
    glEnable( GL_BLEND );    // Enables blending ( glBlendFunc )
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    glEnable( GL_DEPTH_TEST );
    
    // glEnable( GL_CULL_FACE );
    // glCullFace( GL_BACK );
    // glFrontFace( GL_CW );
    
    return EXIT_SUCCESS;
}

int System::SystemSetup()
{
    
    coreShader = Shader( "/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Shaders/core.vert",
                        "/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Shaders/core.frag" );
    coreShader.Use();
    
    return EXIT_SUCCESS;
}

void System::Run(vector<Mesh*> meshs, std::shared_ptr<Camera> cam)
{
    
    glfwSetCursorPosCallback (window, Camera::mouse_callback);
    glfwSetScrollCallback (window, Camera::scroll_callback);
    
    coreShader.Use();
    
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
    
    coreShader.setMatrix4fv("projection", proj);
    
    for (Mesh* mesh : meshs) {
        for (Group* group : mesh->getGroups()) {
            
            Material* material = mesh->getMaterial(group->getMaterial());
            coreShader.LoadTexture(strdup(material->getTexture().c_str()), "texture1", group->getName());
            coreShader.setVec3("materialAmbient", vec3(material->getAmbient()->x, material->getAmbient()->y, material->getAmbient()->z));
            coreShader.setVec3("materialDiffuse", vec3(material->getDiffuse()->x, material->getDiffuse()->y, material->getDiffuse()->z));
            coreShader.setVec3("materialSpecular", vec3(material->getSpecular()->x, material->getSpecular()->y, material->getSpecular()->z));
            coreShader.setFloat("materialShininess", material->getShininess());
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
            
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(0);
            
            // Normais
            glBindBuffer(GL_ARRAY_BUFFER, VBOnormais);
            glBufferData(GL_ARRAY_BUFFER, normais.size() * sizeof(float), normais.data(), GL_STATIC_DRAW);
            
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(1);
            
            // Textures
            glBindBuffer(GL_ARRAY_BUFFER, VBOtextures);
            glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float), textures.data(), GL_STATIC_DRAW);
            
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(2);
            
            group->setVAO(&VAO);
            glBindVertexArray(0); // Unbind VAO
        }
    }
    
    float angle = 0.0f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    float translateZ = 0.0f;
    float camX = 1.0f;
    float camY = 0.5f;
    float camZ = 1.0f;
    
    RotationStatus rotationStatus = none;
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
        
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            translateX -= 0.09f;
        }
        
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            translateZ -= 0.09f;
        }
        
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
            translateY -= 0.09f;
        }
        
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            rotationStatus = X;
        }
        
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            rotationStatus = Y;
        }
        
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            rotationStatus = Z;
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
        coreShader.setVec3("lightPos", vec3(-100.0f, 1.0f, 100.0f));
        coreShader.setVec3("viewPos", vec3(camX, camY, camZ));
        
        switch (rotationStatus) {
            case none:
                break;
            case X:
                for (Mesh* mesh : meshs) {
                    mesh->model = glm::rotate(mesh->model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
                }
                angle += 2.0f;
                rotationStatus = none;
                break;
            case Y:
                for (Mesh* mesh : meshs) {
                    mesh->model = glm::rotate(mesh->model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
                }
                angle += 2.0f;
                rotationStatus = none;
                break;
            case Z:
                for (Mesh* mesh : meshs) {
                    mesh->model = glm::rotate(mesh->model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
                }
                angle += 2.0f;
                rotationStatus = none;
                break;
            default:
                break;
        }
        
        for (Mesh* mesh : meshs) {
            mesh->model = glm::translate(mesh->model, glm::vec3(translateX, translateY, translateZ));
        }
        
        for (Mesh* mesh : meshs) {
            coreShader.setMatrix4fv("model", mesh->model);
            
            for (Group* group : mesh->getGroups()) {
                coreShader.UseTexture(group->getName());
                glBindVertexArray(group->getVAO());
                glDrawArrays(GL_TRIANGLES, 0, group->getNumVertices());
                glBindVertexArray(0);
            }
        }
        
        glfwSwapBuffers(window);
    }
}

void System::Finish()
{
    coreShader.Delete();
    
    glfwTerminate();
}
