#ifndef WORLD_H
#define WORLD_H

#include "Entity.h"
#include "utils.h"
#include "Stage.h"
#include "includes.h"

enum eCellType : uint8 {
	SEA,
	FIELD,
	WALL,
	BOX,
};

struct sCell {
	eCellType type;
};

struct sMapHeader {
	int w; //width of map
	int h; //height of map
	unsigned char bytes; //num bytes per cell
	unsigned char extra[7]; //filling bytes, not used
};

class GameMap {
public:
	int width;
	int height;
	sCell* data;

	GameMap() {
		width = height = 0;
		data = NULL;
	}
	GameMap(int w, int h) {
		width = w;
		height = h;
		data = new sCell[w * h];
	}
	sCell& getCell(int x, int y)
	{
		return data[x + y * width];
	}
};

//We assume maps always starts at (0,0)

Vector3 CellToWorld(Vector2 cellPos, int cellsize);
Vector3 CellToWorldCenter(Vector2 cellPos, int cellsize);

struct sPropViewData {
	Mesh* mesh;
	Texture* texture;
};

class World {
public:
	Mesh* mesh = NULL;
	Texture* texture = NULL;
	Shader* shader = NULL;

	float angle = 0;
	float mouse_speed = 100.0f;
	FBO* fbo = NULL;
	float timeCounter = 0.0f;
	bool startCounter = false;

	EntityPlayer* player;

	std::vector<EntityMesh*> static_entities;
	std::vector<EntityMesh*> dynamic_entities;

	StaticEntities* staticEntities;

	EntityMesh* selectedEntity;

	Mesh* mainCharacter = NULL;
	Texture* texCharacter = NULL;
	Matrix44 model;

	Mesh* sceneMesh = NULL;
	Texture* sceneText = NULL;
	Matrix44 sceneModel;

	bool cameraLocked;
	bool mouseLocked = true;

	Stage* current_stage;
	titleStage* title;
	tutorialStage* tutorial;
	playStage* play;
	endStage* end;

	GameMap* gamemap;
	sPropViewData viewDatas[3];
	const float tileWidth = 2.0f;
	const float tileHeight = 2.0f;

	//Cielo
	EntityMesh* sky;
	Mesh* skyMesh;
	Matrix44 skyModel;
	Texture* skyTex;

	//Suelo
	EntityMesh* ground;
	Mesh* ground_mesh;
	Texture* ground_text;
	Matrix44 groundModel;

	//Box
	Mesh* box_mesh;
	Texture* box_text;
	Matrix44 boxModel;

	void loadWorld();
	void setConfiguration();
	void saveGame();
	void importMap(std::vector<EntityMesh*>& entities);

	void addEntity();
	void deleteEntity();

};

GameMap* loadGameMap(const char* filename);
#endif