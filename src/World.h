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

	float TIME_TRIAL_LVL_1 = 50.0f;

	Mesh* mesh = NULL;
	Texture* texture = NULL;
	Shader* shader = NULL;

	float angle = 0;
	float mouse_speed = 100.0f;

	EntityPlayer* player;


	std::vector<EntityMesh*> static_entities;
	std::vector<EntityChest*> chests;

	std::vector<EntityEnemy*> enemies;

	//std::vector<std::shared_ptr<Entity*>> collidable_entities;

	std::vector<EntityMesh*> collidable_entities;

	EntityMesh* finish;
	int actualLevel;
	bool levelDone;

	EntityMesh* selectedEntity;

	Vector3 spawnPos;

	bool cameraLocked;
	bool mouseLocked = true;

	std::vector<Stage*> stages;
	STAGE_ID currentStage;

	GameMap* gamemap;
	sPropViewData viewDatas[6];
	const float tileWidth = 6.0f;
	const float tileHeight = 6.0f;

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

	float timeTrial = TIME_TRIAL_LVL_1;

	bool sPressed = false;

	void loadWorld();
	void setConfiguration();
	void saveGame();
	void importMap(std::vector<EntityMesh*>& entities);
	void unifyCollidableEntities();
	void addEntity();
	void deleteEntity();
	void loadLevel();
};

GameMap* loadGameMap(const char* filename);

#endif