#pragma once
#include <vector>
#include <map>
#include "Group.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Material.h"

using namespace std;
using namespace glm;

class Mesh
{

private:
    vector<Group*> groups;
    vector<vec3*> vertices;
    vector<vec3*> normais;
    vector<vec2*> textures;
    map<string, Material*> materials;

public:
    Mesh();
    ~Mesh();
    vec3* vertice(int id);
    vec3* normal(int id);
    vec2* texture(int id);
    int addVertice(vec3*);
    int addNormal(vec3*);
    int addTexture(vec2*);
    int addGroup(Group*);
    int addMaterial(string name, Material* material);
    void translateModel(vec3 initialTrans);
    glm::mat4 model;
    
    vector<Group*> getGroups() {
        return groups;
    }

    Material* getMaterial(string name) {
        return materials[name];
    }
};

