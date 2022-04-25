#include "System.h"


int main() {
	map<string, Mesh*> meshs;
	meshs["mesa"] = ObjReader::read("/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Objects/mesa01.obj");
	meshs["trout"] = ObjReader::read("/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Objects/mesa01.obj");
    
	map<string, char*> textures;
	textures["mesa"] = "/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Objects/mesa01.bmp";
	textures["trout"] = "/Users/douglasimmig/Desktop/TrabGA/Exercicio1/Objects/mesa01.bmp";

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

	system.Run(meshs, textures, "mesa", "trout");

	system.Finish();

	return 0;
}
