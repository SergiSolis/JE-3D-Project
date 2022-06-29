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
    Vector4 color;
    int chest_id;

    EntityMesh(int prim, Matrix44 new_model, Mesh* mes, Texture* tex, Shader* shad, Vector4 col = Vector4(1, 1, 1, 1), float til = 1.0f, ENTITY_ID e_id = ENTITY_ID::ENTITY_MESH) {
        primitive = prim;
        model = new_model;
        mesh = mes;
        texture = tex;
        shader = shad;
        color = col;
        tiling = til;
        id = e_id;
        coverFlag = false;
        collision = false;
    }

    //methods overwritten 
    void render();
    void update(float dt);
};

enum PLAYER_ANIM_ID: uint8 {
    PLAYER_IDLE,
    PLAYER_WEAPON_IDLE,
    PLAYER_WALK,
    PLAYER_RUN,
    PLAYER_JUMP,
    PLAYER_ATTACK,
    PLAYER_HIT,
    PLAYER_DEAD
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
    PLAYER_ANIM_ID currentAnim;
    bool firstPerson;
    bool cameraLocked;
    bool isGrounded;
    bool isMoving;

    bool walkingBackwards;

    Vector3 pos;
    Vector3 vel;
    float jumpLock;
    float jaw;
    float pitch;

    
    float hitTimer;
    float animTimer;
    float animDuration;
    float time;

    int hearts;
    int strength;
    int runSpeed;

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
        animations.push_back(Animation::Get("data/warrior_idle.skanim"));
        animations.push_back(Animation::Get("data/walk.skanim"));
        animations.push_back(Animation::Get("data/run.skanim"));
        animations.push_back(Animation::Get("data/jump.skanim"));
        animations.push_back(Animation::Get("data/warrior_attack.skanim"));
        animations.push_back(Animation::Get("data/hit.skanim"));
        animations.push_back(Animation::Get("data/dead.skanim"));
        
        currentAnim = PLAYER_ANIM_ID::PLAYER_IDLE;

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
        strength = 1;
        runSpeed = 15.0f;

        hitTimer = 0.0f;
        time = 0.0f;

        firstPerson = false;
        cameraLocked = true;
        isGrounded = true;
        isMoving = false;
        walkingBackwards = false;
        animDuration = animations[currentAnim]->duration;
    }

    //methods overwritten 
    void checkIsGrounded();
    void render();
    void update(float dt);
};

struct sBullet {
    Matrix44 model;
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
    Vector3 last_position;
    Vector3 velocity;
    float ttl;
    float power;
    int author;
    int type;

    bool isActive() {
        return ttl <= 0.0f;
    }
};

enum ENEMY_ID : uint8 {
    WARRIOR,
    ARCHER,
    BOSS
};
enum ENEMY_ANIM_ID : uint8 {
    ENEMY_IDLE,
    ENEMY_WALK,
    ENEMY_ATTACK,
    ENEMY_HIT,
    ENEMY_DEAD
};

class EntityEnemy : public Entity
{
public:
    EntityMesh* mesh;
    std::vector<Animation*> animations;
    ENEMY_ANIM_ID currentAnim;

    Vector3 pos;
    Vector3 vel;

    Vector3 spawnPos;

    float jaw;

    Matrix44 visualModel;
    Matrix44 weaponModel;
    Skeleton resultSk;

    EntityMesh* weapon;

    bool markedTarget;

    int hearts;
    int strength;
    float velocity;
    float sightDistance;

    float hitTimer;
    float animTimer;
    float animDuration;
    float hitRegion;
    float time;

    float shootTimer;

    float attackSpeed;

    ENEMY_ID type;

    //const int numBullets = 10;
    //std::vector<sBullet*> bullets;

    EntityEnemy(Matrix44 model, Mesh* n_mesh, Texture* tex, int level, ENEMY_ID type_id = ENEMY_ID::WARRIOR) {
        type = type_id;
        Shader* shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
        
        animations.reserve(5);
        if (type == ENEMY_ID::WARRIOR)
        {
            animations.push_back(Animation::Get("data/warrior_idle.skanim"));
            animations.push_back(Animation::Get("data/walk.skanim"));
            animations.push_back(Animation::Get("data/warrior_attack.skanim"));
            animations.push_back(Animation::Get("data/hit.skanim"));
            animations.push_back(Animation::Get("data/dead.skanim")); 
        }
        else if(type == ENEMY_ID::ARCHER) {
            animations.push_back(Animation::Get("data/archer_idle.skanim"));
            animations.push_back(Animation::Get("data/walk.skanim"));
            animations.push_back(Animation::Get("data/archer_attack.skanim"));
            animations.push_back(Animation::Get("data/archer_hit.skanim"));
            animations.push_back(Animation::Get("data/archer_dead.skanim"));
        }
        currentAnim = ENEMY_ANIM_ID::ENEMY_IDLE;

        Texture* playerTex = Texture::Get("data/PolygonMinis_Texture_01_A.png");
        Mesh* playerMesh = Mesh::Get("data/skeleton.mesh");

        mesh = new EntityMesh(GL_TRIANGLES, model, n_mesh, tex, shader);
        
        if (type == ENEMY_ID::WARRIOR) {
            weapon = new EntityMesh(GL_TRIANGLES, Matrix44(), Mesh::Get("data/sword.obj"), Texture::Get("data/color-atlas.png"), shader);
        }
        else if (type == ENEMY_ID::ARCHER) {
            weapon = new EntityMesh(GL_TRIANGLES, Matrix44(), Mesh::Get("data/bow.obj"), Texture::Get("data/color-atlas.png"), shader);
        }
        

        jaw = 180;
        markedTarget = false;
        hearts = 3;

        if (level == 1 || level == 0)
        {
            strength = 1;
            
            velocity = 3.0f;
            if (type == ENEMY_ID::WARRIOR)
            {
                sightDistance = 5.0f;
                attackSpeed = 1.75f;
                hitRegion = 0.7f;
            }
            else if (type == ENEMY_ID::ARCHER)
            {
                sightDistance = 15.0f;
                attackSpeed = 5.0f;
            }
        }
        else if (level == 2) {
            strength = 2;
            velocity = 6.0f;
            if (type == ENEMY_ID::WARRIOR)
            {
                sightDistance = 10.0f;
                attackSpeed = 1.25f;
                hitRegion = 0.55f;
            }
            else if(type == ENEMY_ID::ARCHER)
            {
                sightDistance = 20.0f;
                attackSpeed = 2.0f;
            }
        }

        hitTimer = 0.0f;
        animTimer = 0.0f;
        time = 0.0f;
        if (type == ENEMY_ID::ARCHER) {
            shootTimer = 0.0f;
        }
    }
    void render();
    void update(float dt);
};

enum CHEST_ID : uint8 {
    CHEST_SWORD,
    CHEST_HEART,
    CHEST_STRENGTH
};

class EntityChest : public Entity
{
public:
    EntityMesh* mesh;
    EntityMesh* content;
    CHEST_ID type;
    int collidable_id;

    EntityChest(Matrix44 model, int actualLevel, CHEST_ID chest_type, ENTITY_ID e_id = ENTITY_ID::ENTITY_CHEST) {
        type = chest_type;
        id = e_id;

        mesh = new EntityMesh(GL_TRIANGLES, model, Mesh::Get("data/chest.obj"), Texture::Get("data/color-atlas.png"), Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs"), Vector4(1,1,1,1), 1.0f, ENTITY_CHEST);
        mesh->model = model;

        collidable_id = 0;

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
void renderMesh(int primitive, Matrix44& model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam, float tiling = 1.0);

#endif 