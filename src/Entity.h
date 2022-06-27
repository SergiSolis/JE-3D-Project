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

enum ENTITY_ID : uint8 {
    ENTITY_MESH,
    ENTITY_ENEMY,
    ENTITY_CHEST,
};

class Entity
{
public:
    //some attributes 
    std::string name;
    Matrix44 model;
    Matrix44 global_model;
    BoundingBox aabb;
    ENTITY_ID id;

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
    float tiling;
    
    bool coverFlag;
    bool collision;

    EntityMesh(int prim, Matrix44 new_model, Mesh* mes, Texture* tex, Shader* shad, float til = 1.0f, ENTITY_ID e_id = ENTITY_ID::ENTITY_MESH) {
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
    SWORD_IDLE,
    WALK,
    RUN,
    JUMP,
    ATTACK,
    HIT,
    DEAD
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
    float animTimer;
    float animDuration;
    float time;

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
        
        animations.reserve(8);
        animations.push_back(Animation::Get("data/idle.skanim"));
        animations.push_back(Animation::Get("data/sword_idle.skanim"));
        animations.push_back(Animation::Get("data/walk.skanim"));
        animations.push_back(Animation::Get("data/run.skanim"));
        animations.push_back(Animation::Get("data/jump.skanim"));
        animations.push_back(Animation::Get("data/attack.skanim"));
        animations.push_back(Animation::Get("data/hit.skanim"));
        animations.push_back(Animation::Get("data/death.skanim"));
        
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
        time = 0.0f;

        firstPerson = false;
        cameraLocked = true;
        isGrounded = true;
        walkingBackwards = false;
        animDuration = animations[currentAnim]->duration;
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
    float animTimer;
    float animDuration;
    float time;

    EntityEnemy(Matrix44 model, Mesh* n_mesh, Texture* tex, ENTITY_ID e_id = ENTITY_ID::ENTITY_ENEMY) {
        id = e_id;
        Shader* shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
        
        animations.reserve(8);
        animations.push_back(Animation::Get("data/idle.skanim"));
        animations.push_back(Animation::Get("data/sword_idle.skanim"));
        animations.push_back(Animation::Get("data/walk.skanim"));
        animations.push_back(Animation::Get("data/run.skanim"));
        animations.push_back(Animation::Get("data/jump.skanim"));
        animations.push_back(Animation::Get("data/attack.skanim"));
        animations.push_back(Animation::Get("data/hit.skanim"));
        animations.push_back(Animation::Get("data/death.skanim"));

        currentAnim = ANIM_ID::SWORD_IDLE;

        Texture* playerTex = Texture::Get("data/PolygonMinis_Texture_01_A.png");
        Mesh* playerMesh = Mesh::Get("data/skeleton.mesh");
        //Mesh* playerMesh = Mesh::Get("data/skeleton.obj");

        mesh = new EntityMesh(GL_TRIANGLES, model, n_mesh, tex, shader);
        sword = new EntityMesh(GL_TRIANGLES, Matrix44(), Mesh::Get("data/sword.obj"), Texture::Get("data/color-atlas.png"), shader);

        jaw = 180;
        markedTarget = false;
        hearts = 3;
        hitTimer = 0.0f;
        animTimer = 0.0f;
        time = 0.0f;
    }
    void render();
    void update(float dt);
};

class EntityChest : public Entity
{
public:
    EntityMesh* mesh;
    EntityMesh* content;

    EntityChest(Matrix44 model, int actualLevel, ENTITY_ID e_id = ENTITY_ID::ENTITY_CHEST) {
        id = e_id;
        mesh = new EntityMesh(GL_TRIANGLES, model, Mesh::Get("data/chest.obj"), Texture::Get("data/color-atlas.png"), Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs"));
        mesh->model = model;
        mesh->id = e_id;
        if (actualLevel == 1)
        {
            content = new EntityMesh(GL_TRIANGLES, Matrix44(), Mesh::Get("data/sword.obj"), Texture::Get("data/color-atlas.png"), Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs"));
        }
    }

    //methods overwritten 
    void render();
    void update(float dt);
};

struct sBullet {
    Matrix44 model;
    Vector3 last_position;
    float ttl;
    float power;
    int author;
    int type;
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