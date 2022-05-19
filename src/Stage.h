#pragma once
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

void RenderMesh(Shader* shader, Mesh* mesh, Matrix44 model, Camera* cam, Texture* tex = NULL, float tiling = 1.0f);
void loadmap(GameMap* map);
void setCamera(Camera* cam, Matrix44 model);
void RenderGUI(float x, float y, float w, float h, Texture* dialogo, bool flipuvs);
void runAnimation(Animation* anim, float time, Mesh* mesh, Texture* text, Matrix44 model,bool loop);
//void checkCollision(std::vector<Entity*> entidades);
Vector3 checkCollision(Vector3 target);