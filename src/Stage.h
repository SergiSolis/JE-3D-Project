#ifndef STAGE_H
#define STAGE_H

#include "utils.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "animation.h"
#include "Entity.h"
#include "Audio.h"

enum STAGE_ID : uint8 {
	INTRO,
	TITLE,
	TUTORIAL,
	PLAY,
	TRANSITION,
	EDITOR,
	MENU,
	END
};

class Stage {
public:
	//Stage();
	virtual void render() {};
	virtual void update(float dt) {};
};

class introStage : public Stage {
public:
	//titleStage();
	virtual void render();
	virtual void update(float dt);
};

class titleStage : public Stage {
public:
	//titleStage();
	virtual void render();
	virtual void update(float dt);
};

class tutorialStage : public Stage {
public:
	//tutorialStage();
	virtual void render();
	virtual void update(float dt);
};

class playStage : public Stage {
public:
	//playStage();
	virtual void render() ;
	virtual void update(float dt);
};

class transitionStage : public Stage {
public:
	//playStage();
	virtual void render();
	virtual void update(float dt);
};

class editorStage : public Stage {
public:
	//editorStage();
	virtual void render();
	virtual void update(float dt);
};

class menuStage : public Stage {
public:
	//editorStage();
	virtual void render();
	virtual void update(float dt);
};

class endStage : public Stage {
public:
	//endStage();
	virtual void render();
	virtual void update(float dt);
};

void renderWorld();
void RenderMinimap();
void renderEnemyWeapon(EntityEnemy* enemy);
void renderEnemyGUI(EntityEnemy* enemy);
void setCamera(Camera* cam, Matrix44 model);
void runAnimation(Animation* anim, float time, Mesh* mesh, Texture* text, Matrix44 model,bool loop);
Vector3 checkCollision(Vector3 target);
Vector3 enemyCollision(EntityEnemy* player, Vector3 target);
void bulletCollision(float seconds_elapsed);

void addEntityInFront(Camera* cam, const char* meshName, const char* textName);
EntityMesh* rayPick(Camera* cam);
void takeEntity(Camera* cam);
void checkIfFinish(Camera* cam);
void rotateSelected(float angleDegrees);

void playerInventory();
void playerGUI();
void handleEnemies(float seconds_elapsed);
void handleBoss(float seconds_elapsed);
void checkEnemyHits(EntityEnemy* enemy);

float sign(float value);


int getFreeBullet();
bool spawnBullet(sBullet& newBulletData, int enemyIndex);
void tutorialChangeEnemy();
float lerp_f(float a, float b, float f);
#endif 