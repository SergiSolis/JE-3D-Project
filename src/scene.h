#ifndef WORLD_H
#define WORLD_H

#include "includes.h"
#include "camera.h"
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
    Entity(); //constructor
    virtual ~Entity(); //destructor

    //some attributes 
    std::string name;
    Matrix44 model;

    //methods overwritten by derived classes 
    virtual void render();
    virtual void update(float elapsed_time);

    //some useful methods...
    Vector3 getPosition();
};

class EntityMesh : public Entity
{
public:
    //Attributes of this class 
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
    Vector4 color;

    //methods overwritten 
    void render();
    void update(float dt);
};


#endif 