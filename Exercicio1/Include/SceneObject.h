#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <map>

using namespace std;
using namespace glm;

class SceneObject
{
public:
    SceneObject();
    ~SceneObject();
    string path;
    int objId;
    vec3* initalRot, initalEscale, initalTrans;
};
