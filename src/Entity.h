#include "utils.h"
#include "mesh.h"
#include "texture.h"

class Entity
{
public:
    Entity(); //constructor
    virtual ~Entity(); //destructor

    //some attributes 
    std::string name;
    Matrix44 model;
    Mesh* mesh;
    Texture* texture;
    BoundingBox aabb;

    //methods overwritten by derived classes 
    virtual void render();
    virtual void update(float elapsed_time);

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
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
    Vector4 color;

    //methods overwritten 
    void render();
    void update(float dt);
};

class EntityTrigger : public Entity
{
public:

};

class EntitySound : public Entity
{
public:

};

class EntityPlayer : public Entity
{
public:
    EntityMesh mesh;

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

class EntityWoods : public Entity
{

};