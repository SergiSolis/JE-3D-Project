#ifndef STAGE_H
#define STAGE_H

#include "utils.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "Gamemap.h"
#include "animation.h"
#include "extra/bass.h"

class Stage {
public:
	//Stage();
	virtual void render() {};
	virtual void update(float dt) {};
};

class tutorialStage : public Stage {
public:
	//tutorialStage();
	virtual void render();
	virtual void update(float dt);
};

class titleStage : public Stage {
public:
	//titleStage();
	virtual void render();
	virtual void update(float dt) ;
};

class playStage : public Stage {
public:
	//playStage();
	virtual void render() ;
	virtual void update(float dt);
	//void loadMesh();
};

class endStage : public Stage {
public:
	//endStage();
	virtual void render() {};
	virtual void update(float dt) {};
};

void loadmap(GameMap* map);
void setCamera(Camera* cam, Matrix44 model);
void RenderGUI(float x, float y, float w, float h, Texture* dialogo, bool flipuvs);
void runAnimation(Animation* anim, float time, Mesh* mesh, Texture* text, Matrix44 model,bool loop);
//void checkCollision(std::vector<Entity*> entidades);
void renderMesh(int primitive, Matrix44& model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam, float tiling = 1.0f);
Vector3 checkCollision(Vector3 target);

void addEntityInFront(Camera* cam, const char* meshName, const char* textName);
void rayPick(Camera* cam);
void rotateSelected(float angleDegrees);

#endif 