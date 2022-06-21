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

enum ENTITY_ID : uint8 {
    NOTHING,
    WALL_ID,
    BOX_ID,
};

class EntityMesh : public Entity
{
public:
    //Attributes of this class 
    int primitive;
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
    float tiling;
    ENTITY_ID id;
    bool coverFlag;
    bool collision;

    EntityMesh(int prim, Matrix44 new_model, Mesh* mes, Texture* tex, Shader* shad, float til = 1.0f, ENTITY_ID e_id = ENTITY_ID::NOTHING) {
        primitive = prim;
        model = new_model;
        mesh = mes;
        texture = tex;
        shader = shad;
        tiling = til;
        id = e_id;
        coverFlag = false;
        collision = false;
    }

    //methods overwritten 
    void render();
    void update(float dt);
};

enum ANIM_ID: uint8 {
    IDLE,
    WALK,
    RUN,
    JUMP,
    ATTACK
};

enum ITEM_ID : uint8 {
    SWORD,
    WALL_HAND,
    NONE
};

class EntityPlayer : public Entity
{
public:
    EntityMesh* mesh;
    std::vector<EntityMesh*> inventory;
    Matrix44 swordModel;
    ITEM_ID currentItem;

    std::vector<Animation*> animations;
    ANIM_ID currentAnim;
    bool firstPerson;
    bool cameraLocked;
    bool isGrounded;

    bool walkingBackwards;

    Vector3 pos;
    Vector3 vel;
    float jumpLock;
    float jaw;
    float pitch;

    int hearts;
    float hitTimer;

    Matrix44 visualModel;
    Skeleton resultSk;

    EntityPlayer() {
        Shader* shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
        Matrix44 playerModel;
        playerModel.translate(pos.x, pos.y, pos.z);
        playerModel.rotate(jaw * DEG2RAD, Vector3(0, 1, 0));

        currentItem = ITEM_ID::NONE;

        inventory.reserve(2);
        EntityMesh* entityBox = new EntityMesh(GL_TRIANGLES, Matrix44(), Mesh::Get("data/sword.obj"), Texture::Get("data/color-atlas.png"), shader);
        //EntityMesh* entityWall = new EntityMesh(GL_TRIANGLES, Matrix44(), Mesh::Get("data/wall.obj"), Texture::Get("data/color-atlas.png"), shader);
        inventory.push_back(entityBox);
        //inventory.push_back(entityWall);
        
        animations.reserve(5);
        animations.push_back(Animation::Get("data/sword_idle.skanim"));
        animations.push_back(Animation::Get("data/walk.skanim"));
        animations.push_back(Animation::Get("data/run.skanim"));
        animations.push_back(Animation::Get("data/jump.skanim"));
        animations.push_back(Animation::Get("data/attack.skanim"));
        
        currentAnim = ANIM_ID::IDLE;

        Texture* playerTex = Texture::Get("data/PolygonMinis_Texture_01_A.png");
        Mesh* playerMesh = Mesh::Get("data/skeleton.mesh");

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

        hearts = 3;

        hitTimer = 0.0f;

        firstPerson = false;
        cameraLocked = true;
        isGrounded = true;
        walkingBackwards = false;
    }

    //methods overwritten 
    void checkIsGrounded();
    void render();
    void update(float dt);
};

class EntityEnemy : public Entity
{
public:
    EntityMesh* mesh;
    std::vector<Animation*> animations;
    ANIM_ID currentAnim;

    Vector3 pos;
    Vector3 vel;

    Vector3 spawnPos;

    float jaw;

    Matrix44 visualModel;
    Matrix44 swordModel;
    Skeleton resultSk;

    EntityMesh* sword;

    bool markedTarget;
    int hearts;
    float hitTimer;

    EntityEnemy(Matrix44 model, Mesh* n_mesh, Texture* tex) {
        Shader* shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
        
        animations.reserve(4);
        animations.push_back(Animation::Get("data/sword_idle.skanim"));
        animations.push_back(Animation::Get("data/attack.skanim"));
        animations.push_back(Animation::Get("data/walk.skanim"));
        animations.push_back(Animation::Get("data/run.skanim"));

        currentAnim = ANIM_ID::IDLE;

        Texture* playerTex = Texture::Get("data/PolygonMinis_Texture_01_A.png");
        Mesh* playerMesh = Mesh::Get("data/skeleton.mesh");
        //Mesh* playerMesh = Mesh::Get("data/skeleton.obj");

        mesh = new EntityMesh(GL_TRIANGLES, model, n_mesh, tex, shader);
        sword = new EntityMesh(GL_TRIANGLES, Matrix44(), Mesh::Get("data/sword.obj"), Texture::Get("data/color-atlas.png"), shader);

        jaw = 180;
        markedTarget = false;
        hearts = 3;
        hitTimer = 0.0f;
    }
    void render();
    void update(float dt);
};

class EntityChest : public Entity
{
public:
    EntityMesh* mesh;
    EntityMesh* content;

    EntityChest(Matrix44 model, int actualLevel) {
        mesh = new EntityMesh(GL_TRIANGLES, model, Mesh::Get("data/chest.obj"), Texture::Get("data/color-atlas.png"), Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs"));
        mesh->model = model;
        if (actualLevel == 1)
        {
            content = new EntityMesh(GL_TRIANGLES, Matrix44(), Mesh::Get("data/sword.obj"), Texture::Get("data/color-atlas.png"), Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs"));
        }
    }

    //methods overwritten 
    void render();
    void update(float dt);
};

class EntityCamera : public Entity
{
public:

};

class EntitySound : public Entity
{
public:

};

void renderGUI(float x, float y, float w, float h, Texture* tex, bool flipYV = false);

#endif 