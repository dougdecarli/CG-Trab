#include "ObjReader.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

Mesh* ObjReader::read(string filename)
{
    Mesh* mesh = new Mesh();

    int firstGroup = 1;
    Group* group = new Group();
    group->setName("default");

    ifstream arq(filename);

    if (!arq) {
        exit(EXIT_FAILURE);
    }

    while (!arq.eof()) {
        string line;
        getline(arq, line);

        stringstream sline(line);

        string temp;
        sline >> temp;

        if (temp.empty() || temp == "#") {
            continue;
        }

        if (temp == "mtllib") {
            material(mesh, sline);
        } else if (temp == "v") {
            vertice(mesh, sline);
        }
        else if (temp == "vn") {
            normal(mesh, sline);
        }
        else if (temp == "vt") {
            texture(mesh, sline);
        }
        else if (temp == "f") {
            face(group, sline);
        }
        else if (temp == "g") {
            if (firstGroup == 1) {
                firstGroup = 0;
            }
            else {
                mesh->addGroup(group);
                group = new Group();
            }

            string name;
            sline >> name;
            group->setName(name);
        }
        else if (temp == "usemtl") {

        }

    }

    arq.close();

    mesh->addGroup(group);
    return mesh;
}

void ObjReader::material(Mesh* mesh, stringstream& sline) {
    string name;
    sline >> name;
    mesh->setMaterial(name);
}

void ObjReader::vertice(Mesh* mesh, stringstream &sline) {
    float x, y, z;
    sline >> x >> y >> z;
    mesh->addVertice(new glm::vec3(x, y, z));
}

void ObjReader::normal(Mesh* mesh, stringstream& sline) {
    float x, y, z;
    sline >> x >> y >> z;
    mesh->addNormal(new glm::vec3(x, y, z));
}

void ObjReader::texture(Mesh* mesh, stringstream& sline) {
    float x, y;
    sline >> x >> y;
    mesh->addTexture(new glm::vec2(x, y));
}

int getNextTokenValue(stringstream& stoken) {
    string temp;
    getline(stoken, temp, '/');
    return temp.empty() ? 0 : stoi(temp);
}

void ObjReader::face(Group* group, stringstream& sline) {
    Face* face = new Face();

    string token;
    while (getline(sline, token, ' ')) {

        if (token.empty()) {
            continue;
        }

        stringstream stoken(token);

        int v = getNextTokenValue(stoken);
        int t = getNextTokenValue(stoken);
        int n = getNextTokenValue(stoken);

        face->push(v, n, t);
    }

    group->addFace(face);
}

void ObjReader::usemtl(Group* group, stringstream& sline) {
    string name;
    sline >> name;
    group->setMaterial(name);
}
