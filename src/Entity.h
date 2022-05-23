#ifndef ENTITY_H
#define ENTITY_H

#include "utils.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"

class Entity
{
public:
    //some attributes 
    std::string name;
    Matrix44 model;
    Matrix44 global_model;
    BoundingBox aabb;

    //methods overwritten by derived classes 
    virtual void render() = 0;
    virtual void update(float dt) = 0;

    //some useful methods...
    Vector3 getPosition();

    //pointer to my parent entity 
    Entity* parent;

    //pointers to my children
    std::vector<Entity*> children;

    //methods
    void addChild(Entity* ent);
    void removeChild(Entity* ent);

    Matrix44 getGlobalMatrix(); //returns transform in world coordinates
};

class EntityMesh : public Entity
{
public:
    //Attributes of this class 
    int primitive;
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
    int tiling = 1.0f;

    EntityMesh(int prim, Matrix44 new_model, Mesh* mes, Texture* tex, Shader* shad) {
        primitive = prim;
        model = new_model;
        mesh = mes;
        texture = tex;
        shader = shad;
    }

    //methods overwritten 
    void render();
    void update(float dt);
};

class EntityMeshStatic : public EntityMesh
{
public:

};

class EntityMeshDynamic : public EntityMesh
{
public:

};

class EntitySound : public Entity
{
public:

};

struct sPlayer {
    Vector3 pos;
    float jaw;
    float pitch;
};

class EntityPlayer : public Entity
{
public:
    EntityMesh* mesh;
    sPlayer mov;
    bool firstPerson;

    EntityPlayer() {
        Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
        Matrix44 playerModel;
        playerModel.translate(mov.pos.x, mov.pos.y, mov.pos.z);
        playerModel.rotate(mov.jaw * DEG2RAD, Vector3(0, 1, 0));

        Texture* playerTex = Texture::Get("data/PolygonMinis_Texture_01_A.png");
        Mesh* playerMesh = Mesh::Get("data/skeleton_back.obj");

        mesh = new EntityMesh(GL_TRIANGLES, playerModel, playerMesh, playerTex, shader);

        mov.pos.x = 0;
        mov.pos.y = 0;
        mov.jaw = 0;

        firstPerson = false;

    }

    //methods overwritten 
    void render();
    void update(float dt);
};

class EntityEnemies : public Entity
{
public:
    EntityMesh mesh;

};


class EntityCamera : public Entity
{
public:

};

#endif 