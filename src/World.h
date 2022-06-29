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

enum ACTION_ID : uint8 {
	NO_ACTION,
	WIN,
	DEAD,
	OPEN_CHEST,
	PAUSE,
	PAUSE_TUTORIAL,
};
	

struct levelInfo {
	int level;
	ACTION_ID tag;
	float space_pressed;
	CHEST_ID last_chest;
	int last_player_hearts;
	int last_player_strength;
};

enum TITLE_OPTIONS : uint8 {
	PLAY_GAME,
	PLAY_TUTORIAL,
	PLAY_EXIT,
};

enum MENU_OPTIONS : uint8 {
	RETURN,
	RESTART_GAME,
	EXIT,
};

class World {
public:

	float TIME_TRIAL_LVL_1 = 200.0f;

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

	levelInfo level_info;
	TITLE_OPTIONS titleOption;
	MENU_OPTIONS menuOption;

	EntityMesh* selectedEntity;

	Vector3 spawnPos;

	bool cameraLocked;
	bool camera_inverse;
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
	const int numBullets = 100;
	//std::vector<sBullet> bullets;
	sBullet bullets[100];
	bool bulletOnce = false;
	
	HCHANNEL backgroundSound;

	void loadWorld();
	void setConfiguration();
	void saveGame();
	void importMap(std::vector<EntityMesh*>& entities);
	void unifyCollidableEntities();
	void addEntity();
	void deleteEntity();
	void loadLevel();
	void PlaySoundWorld(const char* fileName);
	void StopSoundWorld();
};

GameMap* loadGameMap(const char* filename);
HSAMPLE loadSample(const char* fileName);



#endif