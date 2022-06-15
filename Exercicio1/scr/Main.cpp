#include "System.h"
#include "SceneReader.h"
#include <string>

int main() {
    System system;
    
    if ( system.GLFWInit() != 0 ){
        return EXIT_FAILURE;
    }
    if ( system.OpenGLSetup() != 0 ){
        return EXIT_FAILURE;
    }
    if ( system.SystemSetup() != 0 ){
        return EXIT_FAILURE;
    }
    
    vector<Mesh*> meshs;
    
    SceneReader* sceneReader = new SceneReader();
    sceneReader->read("/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Objects/Scene.txt");

    for(SceneObject* sceneObj : sceneReader->sceneObjects) {
        string meshPath = sceneObj->path;
        Mesh* mesh = ObjReader::read(meshPath);
        mesh->translateModel(sceneObj->initalTrans);
        meshs.push_back(mesh);
    }
    
    std::shared_ptr<Camera> cam;
    cam = std::make_shared<Camera>();
    Camera::bind_instance (cam);
    cam->cameraPos.x = sceneReader->cameraInicialX;
    cam->cameraPos.y = sceneReader->cameraInicialY;
    cam->cameraPos.z = sceneReader->cameraInicialZ;
    
    system.Run(meshs, cam);
    
    system.Finish();
    
    return 0;

}
