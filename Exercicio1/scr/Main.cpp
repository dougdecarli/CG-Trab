#include "System.h"


int main() {
	map<string, Mesh*> meshs;
	meshs["obj1"] = ObjReader::read("/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Objects/Pikachu.obj");
    meshs["obj2"] = ObjReader::read("/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Objects/Pikachu.obj");
    
	map<string, char*> textures;
	textures["obj1"] = "/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Objects/PikachuDh.png";
    textures["obj2"] = "/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Objects/PikachuDh.png";

	System system;

	if (system.GLFWInit() != 0){
		return EXIT_FAILURE;
	}
	if (system.OpenGLSetup() != 0){
		return EXIT_FAILURE;
	}
	if (system.SystemSetup() != 0){
		return EXIT_FAILURE;
	}

	system.Run(meshs, textures, "obj1", "obj2");

	system.Finish();

	return 0;
}
