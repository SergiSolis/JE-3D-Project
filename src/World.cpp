#include "World.h"
#include "game.h"
#include <bass.h>

void World::loadWorld() {
	Game* game = Game::instance;

	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	player = new EntityPlayer();

	//bullets.reserve(numBullets);

	for (size_t i = 0; i < numBullets; i++)
	{
		bullets[i].ttl = 0.0f;
		bullets[i].model = Matrix44();
		bullets[i].last_position = Vector3();
		bullets[i].mesh = Mesh::Get("data/box.obj");
		bullets[i].texture = Texture::Get("data/PolygonMinis_Texture_01_A.png");
		bullets[i].shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		bullets[i].power = 1;
		bullets[i].velocity = Vector3(20.0f, 20.0f, 20.0f);
	}
	

	currentStage = STAGE_ID::TITLE;
	titleOption = TITLE_OPTIONS::PLAY_GAME;
	menuOption = MENU_OPTIONS::RETURN;

	stages.reserve(7);
	stages.push_back(new titleStage());
	stages.push_back(new tutorialStage());
	stages.push_back(new playStage());
	stages.push_back(new transitionStage());
	stages.push_back(new editorStage());
	stages.push_back(new menuStage());
	stages.push_back(new endStage());

	skyModel = Matrix44();
	skyMesh = Mesh::Get("data/cielo.ASE");
	skyTex = Texture::Get("data/night.tga");
	sky = new EntityMesh(GL_TRIANGLES, skyModel, skyMesh, skyTex, shader);

	exit_open = Mesh::Get("data/exit_open.obj");

	Matrix44 groundModel;
	Mesh* groundMesh = new Mesh();
	groundMesh->createPlane(1000);
	ground = new EntityMesh(GL_TRIANGLES, groundModel, groundMesh, Texture::Get("data/ground.jpg"), shader, Vector4(1,1,1,1), 500.0f);
	cameraLocked = true;

	viewDatas[1].mesh = Mesh::Get("data/wall.obj");
	viewDatas[1].texture = Texture::Get("data/color-atlas.png");

	viewDatas[2].mesh = Mesh::Get("data/exit.obj");
	viewDatas[2].texture = Texture::Get("data/color-atlas.png");

	viewDatas[3].mesh = Mesh::Get("data/enemy_warrior.mesh");
	viewDatas[3].texture = Texture::Get("data/PolygonMinis_Texture_01_A.png");

	viewDatas[4].mesh = Mesh::Get("data/enemy_archer.mesh");
	viewDatas[4].texture = Texture::Get("data/PolygonMinis_Texture_01_A.png");

	viewDatas[5].mesh = Mesh::Get("data/graveyard.obj");
	viewDatas[5].texture = Texture::Get("data/color-atlas.png");

	level_info.level = 0;

	gamemap = new GameMap();
	//gamemap = loadGameMap("data/lvl1.map");
	//importMap(static_entities);
	//unifyCollidableEntities();

	level_info.tag = ACTION_ID::NO_ACTION;
	level_info.space_pressed = 0.0f;

	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) //-1 significa usar el por defecto del sistema operativo
	{
		std::cout << "Error init BASS" << std::endl;
	}
	//std::cout << "static_entities: " << static_entities.size() << std::endl;
	//camera_inverse = false;
}

Vector3 CellToWorld(Vector2 cellPos, int cellsize) { //esquina superior izquierda
	Game* game = Game::instance;
	Vector2 result = cellPos * cellsize;
	return Vector3(cellPos.x * game->world.tileWidth, 0.0f, cellPos.y * game->world.tileWidth);
}

Vector3 CellToWorldCenter(Vector2 cellPos, int cellsize) { //center
	Vector2 result = cellPos * cellsize - Vector2(cellsize / 2, cellsize / 2);
	return Vector3(result.x, 0.0f, result.y);
}

void World::importMap(std::vector<EntityMesh*>& entities) {
	for (size_t i = 0; i < gamemap->width; i++)
	{
		for (size_t j = 0; j < gamemap->height; j++)
		{
			sCell& cell = gamemap->getCell(i, j);
			int index = (int)cell.type;
			sPropViewData& prop = viewDatas[0];
			if (index == 0) continue;
			if (index == 3) {
				player->pos = CellToWorldCenter(Vector2(i, j), tileWidth);
				spawnPos = player->pos;
			}else{
				Matrix44 cellModel;
				cellModel.translate(i * tileWidth, 0.0f, j * tileHeight);
				cellModel.scale(3, 3, 3);
				//renderMesh(GL_TRIANGLES, cellModel, prop.mesh, prop.texture, world.shader, game->camera);
				if (index == 1) {
					prop = viewDatas[1];
					EntityMesh* entity = new EntityMesh(GL_TRIANGLES, cellModel, prop.mesh, prop.texture, shader);
					entities.push_back(entity);
				}
				else if (index == 2) {
					prop = viewDatas[1];
					cellModel.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
					EntityMesh* entity = new EntityMesh(GL_TRIANGLES, cellModel, prop.mesh, prop.texture, shader);
					entities.push_back(entity);
				}
				else if (index == 4) {
					prop = viewDatas[2];
					EntityMesh* entity = new EntityMesh(GL_TRIANGLES, cellModel, prop.mesh, prop.texture, shader);
					entity->id = ENTITY_ID::ENTITY_MESH;
					finish = entity;
					//entities.push_back(entity);
				}else if (index == 5) {
					prop = viewDatas[2];
					cellModel.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
					EntityMesh* entity = new EntityMesh(GL_TRIANGLES, cellModel, prop.mesh, prop.texture, shader);
					entity->id = ENTITY_ID::ENTITY_MESH;
					finish = entity;
					//entities.push_back(entity);
				}
				else if (index == 6) {
					prop = viewDatas[3];
					//cellModel.rotate(90 * DEG2RAD, Vector3(1, 0, 0));
					EntityEnemy* enenmy = new EntityEnemy(cellModel, prop.mesh, prop.texture, level_info.level, ENEMY_ID::WARRIOR);
					enenmy->pos = CellToWorldCenter(Vector2(i, j), tileWidth);
					enenmy->spawnPos = enenmy->pos;
					enemies.push_back(enenmy);

				}
				//arquero
				else if (index == 7) {
					prop = viewDatas[4];
					//cellModel.rotate(90 * DEG2RAD, Vector3(1, 0, 0));
					EntityEnemy* enenmy = new EntityEnemy(cellModel, prop.mesh, prop.texture, level_info.level, ENEMY_ID::ARCHER);
					enenmy->pos = CellToWorldCenter(Vector2(i, j), tileWidth);
					enenmy->spawnPos = enenmy->pos;
					enemies.push_back(enenmy);

				}
				else if (index == 8) {
					//prop = viewDatas[4];
					cellModel.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
					EntityChest* entity = new EntityChest(cellModel, level_info.level, CHEST_ID::CHEST_SWORD);

					chests.push_back(entity);
				}
				else if (index == 9) {
					//prop = viewDatas[4];
					cellModel.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
					EntityChest* entity = new EntityChest(cellModel, level_info.level, CHEST_ID::CHEST_HEART);
					chests.push_back(entity);
				}
				else if (index == 10) {
					//prop = viewDatas[4];
					cellModel.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
					EntityChest* entity = new EntityChest(cellModel, level_info.level, CHEST_ID::CHEST_STRENGTH);
					chests.push_back(entity);
				}
				else if (index == 11) {
					//prop = viewDatas[4];
					cellModel.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
					EntityChest* entity = new EntityChest(cellModel, level_info.level, CHEST_ID::CHEST_VELOCITY);
					chests.push_back(entity);
				}
				else if (index == 12) {
					prop = viewDatas[5];
					EntityMesh* entity = new EntityMesh(GL_TRIANGLES, cellModel, prop.mesh, prop.texture, shader);
					entities.push_back(entity);
				}
				else if (index == 13) {
					prop = viewDatas[5];
					cellModel.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
					EntityMesh* entity = new EntityMesh(GL_TRIANGLES, cellModel, prop.mesh, prop.texture, shader);
					entities.push_back(entity);
				}
			}
		}
	}
}

void  World::unifyCollidableEntities() {
	if (!collidable_entities.empty())
	{
		collidable_entities.clear();
	}
	for (size_t i = 0; i < static_entities.size(); i++)
	{
		EntityMesh* entity = static_entities[i];
		//collidable_entities.emplace_back(entity);
		//collidable_entities.push_back(std::make_shared<Entity*>(entity));
		collidable_entities.push_back(entity);
	}
	for (size_t i = 0; i < chests.size(); i++)
	{
		EntityMesh* entity = chests[i]->mesh;
		entity->id = ENTITY_ID::ENTITY_CHEST;
		//collidable_entities.push_back(std::make_shared<Entity*>(entity));
		entity->chest_id = i;
		collidable_entities.push_back(entity);

		chests[i]->collidable_id = i;
	}
	for (size_t i = 0; i < enemies.size(); i++)
	{
		//EntityMesh* entity = enemies[i]->mesh;
		//collidable_entities.push_back(entity);
	}
}

GameMap* loadGameMap(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (file == NULL) //file not found
		return NULL;

	sMapHeader header; //read header and store it in the struct
	fread(&header, sizeof(sMapHeader), 1, file);
	assert(header.bytes == 1); //always control bad cases!!


	//allocate memory for the cells data and read it
	unsigned char* cells = new unsigned char[header.w * header.h];
	fread(cells, header.bytes, header.w * header.h, file);
	fclose(file); //always close open files
	//create the map where we will store it
	GameMap* map = new GameMap(header.w, header.h);

	for (int x = 0; x < map->width; x++)
		for (int y = 0; y < map->height; y++)
			map->getCell(x, y).type = (eCellType)cells[x + y * map->width];

	delete[] cells; //always free any memory allocated!

	return map;
}

void World::loadLevel() {
	if (level_info.tag == ACTION_ID::WIN) {
		level_info.level += 1;
		if (level_info.level == 4) {
			currentStage = STAGE_ID::END;
			return;
		}
		
	}

	if (level_info.level == 1) {
		StopSoundWorld();
		PlaySoundWorld("data/background1.wav");
		player->currentItem = ITEM_ID::NONE;

	}
	else if (level_info.level == 0)
	{
		StopSoundWorld();
		PlaySoundWorld("data/background.wav");
		player->currentItem = ITEM_ID::SWORD;
	}
	else {

	}

	static_entities.clear();
	collidable_entities.clear();
	chests.clear();
	enemies.clear();
	std::string s = std::to_string(level_info.level);
	char const* level = s.c_str();
	char i_path[100] = "data/lvl";
	char const* path = strcat(i_path, level);
	path = strcat(i_path, ".map");
	std::cout << path << std::endl;

	gamemap = loadGameMap(path);
	importMap(static_entities);
	unifyCollidableEntities();
	if (level_info.level != 0) {
		
		timeTrial = TIME_TRIAL_LVL_1;
	}
	
	level_info.tag == ACTION_ID::NO_ACTION;
	
	if (level_info.level == 1 ) {
		player->hearts = 3;
		player->strength = 1;
	}
	else if (level_info.level == 0) {
		player->hearts = 8;
		player->strength = 1;
	}
	else {
		if (level_info.last_player_hearts < 3){
			player->hearts = 3;
		}
		else {
			player->hearts = level_info.last_player_hearts;
		}
		player->strength = level_info.last_player_strength;
	}
	player->hitTimer = 0.0f;
	player->jaw = 0;

	//camera_inverse = false;

	currentStage = STAGE_ID::PLAY;
}

void World::PlaySoundWorld(const char* fileName) {
	HSAMPLE hSample = loadSample(fileName);

	backgroundSound = BASS_SampleGetChannel(hSample, false);

	BASS_ChannelPlay(backgroundSound, true);
}

void World::StopSoundWorld() {

	BASS_ChannelStop(backgroundSound);
}

HSAMPLE loadSample(const char* fileName) {
	//El handler para un sample
	HSAMPLE hSample;
	//use BASS_SAMPLE_LOOP in the last param to have a looped sound
	hSample = BASS_SampleLoad(false, fileName, 0, 0, 3, 0);
	if (hSample == 0)
	{
		std::cout << "ERROR load " << fileName << std::endl;
	}
	std::cout << "+ AUDIO load " << fileName << std::endl;
	return hSample;
}



