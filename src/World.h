#ifndef WORLD_H
#define WORLD_H

#include "Entity.h"
#include "utils.h"
#include "Stage.h"

class World {
public:
	Mesh* mesh = NULL;
	Texture* texture = NULL;
	Shader* shader = NULL;

	float angle = 0;
	float mouse_speed = 100.0f;
	FBO* fbo = NULL;
	float timeCounter = 0.0;
	bool startCounter = false;

	EntityPlayer* player;

	std::vector<EntityMesh*> static_entities;
	std::vector<EntityMesh*> dynamic_entities;

	Mesh* mainCharacter = NULL;
	Texture* texCharacter = NULL;
	Matrix44 model;

	Mesh* sceneMesh = NULL;
	Texture* sceneText = NULL;
	Matrix44 sceneModel;

	bool cameraLocked = false;

	Stage* current_stage;
	titleStage* title;
	tutorialStage* tutorial;
	playStage* play;
	endStage* end;

	GameMap* map;

	//Cielo
	EntityMesh* sky;
	Mesh* skybox;
	Matrix44 skymodel;
	Texture* tex;

	//Suelo
	EntityMesh* ground;
	Mesh* ground_mesh;
	Texture* ground_text;
	Matrix44 groundModel;

	//Puerta
	Mesh* doormesh;
	Texture* doortext;
	Matrix44 doorModel;

	//Box
	Mesh* box_mesh;
	Texture* box_text;
	Matrix44 boxModel;

	void loadWorld();
	void setConfiguration();
	void saveGame();

};

#endif