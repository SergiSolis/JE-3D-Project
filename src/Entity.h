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

enum ANIM_ID: uint8 {
    IDLE,
    WALK,
    RUN,
    JUMP
};

class EntityPlayer : public Entity
{
public:
    EntityMesh* mesh;
    //Animation* idle;
    //Animation* walk;
    //Animation* run;

    std::vector<Animation*> animations;
    ANIM_ID currentAnim;
    bool firstPerson;
    bool cameraLocked;
    bool isGrounded;

    Vector3 pos;
    Vector3 vel;
    float jumpLock;
    float jaw;
    float pitch;

    bool objectSelected;


    Matrix44 visualModel;
    Skeleton resultSk;

    EntityPlayer() {
        Shader* shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
        Matrix44 playerModel;
        playerModel.translate(pos.x, pos.y, pos.z);
        playerModel.rotate(jaw * DEG2RAD, Vector3(0, 1, 0));


        animations.reserve(4);
        animations.push_back(Animation::Get("data/idle.skanim"));
        animations.push_back(Animation::Get("data/walk.skanim"));
        animations.push_back(Animation::Get("data/run.skanim"));
        animations.push_back(Animation::Get("data/jump.skanim"));
        
        currentAnim = ANIM_ID::IDLE;

        Texture* playerTex = Texture::Get("data/PolygonMinis_Texture_01_A.png");
        Mesh* playerMesh = Mesh::Get("data/skeleton.mesh");
        //Mesh* playerMesh = Mesh::Get("data/skeleton.obj");

        playerModel.scale(0.01f, 0.01f, 0.01f);
        mesh = new EntityMesh(GL_TRIANGLES, playerModel, playerMesh, playerTex, shader);

        pos.x = 10;
        pos.y = 0;
        pos.z = 10;
        vel.x = 0;
        vel.y = 0;
        vel.z = 0;
        jaw = 0;
        jumpLock = 0.0f;

        firstPerson = false;
        cameraLocked = true;
        isGrounded = true;
        objectSelected = false;
    }

    //methods overwritten 
    void checkIsGrounded();
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

class StaticEntities : public Entity
{
public:
    std::vector<EntityMesh*> entities;
};

class EntityMeshDynamic : public EntityMesh
{
public:

};

class EntitySound : public Entity
{
public:

};

void renderMesh(int primitive, Matrix44& model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam, float tiling = 1.0f);
void renderMeshAnim(int primitive, Matrix44& model, Mesh* a_mesh, Animation* anim, Texture* tex, Shader* a_shader, Camera* cam, float tiling = 1.0f);
#endif 