#include "Stage.h"
#include "game.h"
#include "utils.h"
#include "fbo.h"
#include <cmath>
#include "animation.h"
#include "input.h"
#include "extra/coldet/coldet.h"


void titleStage::render()
{
}

void titleStage::update(float seconds_elapsed)
{
}

void tutorialStage::render()
{
}

void tutorialStage::update(float seconds_elapsed)
{
}

void playStage::render()
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	// set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera as default
	game->camera->enable();

	renderWorld();

	if (world.currentStage == STAGE_ID::PLAY)
		setCamera(game->camera, player->model);

	player->render();
	
	playerInventory();
		
	//Draw the floor grid
	//drawGrid();

	glDisable(GL_BLEND);
	
	playerGUI();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	drawText(2, 25, "Time trial: "  + std::to_string(world.timeTrial), Vector3(1, 1, 0), 2);
	drawText(2, 50, "Actual level: " + std::to_string(world.actualLevel), Vector3(1, 1, 0), 2);
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(game->window);
}

void playStage::update(float seconds_elapsed)
{
	//std::cout << "PLAY STAGE" << std::endl;
	Game *game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	std::vector<EntityEnemy*>& s_enemies = Game::instance->world.enemies;
	//std::cout << "Enemies:" << world.enemies.size() << std::endl;

	Vector3 coll;
	Vector3 collnorm;
	Vector3 pushAway;
	Vector3 returned;

	for (size_t i = 0; i < world.numBullets; i++)
	{
		sBullet& currentBullet = world.bullets[i];
		if (currentBullet.isActive())
			continue;

		//first check if we have build the collision model
		if (!player->mesh->mesh->collision_model)
			player->mesh->mesh->createCollisionModel();

		CollisionModel3D* collision_model = (CollisionModel3D*) player->mesh->mesh->collision_model;
		Vector3 currentPos = currentBullet.model.getTranslation();
		Vector3 next = currentPos + (currentBullet.velocity * seconds_elapsed);
		//Vector3 ray_origin = currentBullet->last_position;
		//Vector3 ray_direction = currentBullet->model.getTranslation() - currentBullet->last_position;
		//float max_ray_dist = ray_direction.length();
		//float max_ray_dist = next.length();

		//now check where the ray collides with mesh
		//bool test = collision_model->rayCollision(currentPos.v, next.v, false, 0.0, max_ray_dist); //max ray distance

		
		currentBullet.last_position = currentPos;
		currentBullet.model.setTranslation(next.x, next.y, next.z);
		currentBullet.ttl -= seconds_elapsed;
		bool test = currentBullet.mesh->testSphereCollision(currentBullet.model, player->pos, 0.75, coll, collnorm);
		std::cout << "TTL: " << currentBullet.ttl << std::endl;
		//if ray didnt collide
		if (test == false)
			continue;

		currentBullet.ttl = -1.0f;
		player->hearts -= 1;
	}


	player->time += seconds_elapsed;

	if (player->animTimer <= 0.0f)
	{
		player->currentAnim = ANIM_ID::IDLE;
	}

	for (size_t i = 0; i < s_enemies.size(); i++)
	{
		

		s_enemies[i]->hitTimer = max(0.0f, s_enemies[i]->hitTimer - seconds_elapsed);
	}

	for (size_t i = 0; i < s_enemies.size(); i++)
	{
		s_enemies[i]->time += seconds_elapsed;
		//std::cout << "Anim duration: " << s_enemies[i]->animTimer << std::endl;
		s_enemies[i]->animTimer = max(0.0f, s_enemies[i]->animTimer - (seconds_elapsed / s_enemies[i]->animDuration));
		/*if (s_enemies[i]->animTimer <= 0.0f && s_enemies[i]->currentAnim != ANIM_ID::DEAD)
		{
			s_enemies[i]->currentAnim = ANIM_ID::SWORD_IDLE;
		}
		*/
		if (s_enemies[i]->animTimer <= 0.0f && s_enemies[i]->hearts <= 0) {
			s_enemies.erase(s_enemies.begin() + i);
		}
		if (s_enemies[i]->currentAnim == ANIM_ID::DEAD)
		{
			std::cout << "Anim duration: " << s_enemies[i]->animTimer << std::endl;
		}
	}

	player->animTimer = max(0.0f, player->animTimer - (seconds_elapsed / player->animDuration));
	//std::cout << "Anim duration: " << player->animTimer << std::endl;
	float speed = seconds_elapsed * 10; // the speed is defined by the seconds_elapsed so it goes constant

	player->jumpLock = max(0.0f, player->jumpLock - seconds_elapsed);
	player->hitTimer = max(0.0f, player->hitTimer - seconds_elapsed);
	
	
	//example
	world.angle += (float)seconds_elapsed * 10.0f;

	world.timeTrial -= seconds_elapsed;
	if (world.timeTrial <= 0.0f || player->hearts <= 0) {
		world.currentStage = STAGE_ID::TRANSITION;
	}

	float playerSpeed = 10.0f * seconds_elapsed;
	float rotateSpeed = 120.0f * seconds_elapsed;
	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		world.currentStage = STAGE_ID::EDITOR;
	}
	if (player->firstPerson)
	{
		player->pitch -= Input::mouse_delta.y * 5.0f * seconds_elapsed;
		player->jaw -= Input::mouse_delta.x * 5.0f * seconds_elapsed;
		Input::centerMouse();
		SDL_ShowCursor(false);
	}
	else {
		player->pitch -= Input::mouse_delta.y * 5.0f * seconds_elapsed;
		if (Input::isKeyPressed(SDL_SCANCODE_D)) player->jaw += rotateSpeed;
		if (Input::isKeyPressed(SDL_SCANCODE_A)) player->jaw -= rotateSpeed;
	}
	Matrix44 playerRotation;
	playerRotation.rotate(player->jaw * DEG2RAD, Vector3(0, 1, 0));
	Vector3 forward = playerRotation.rotateVector(Vector3(0, 0, -1));
	Vector3 right = playerRotation.rotateVector(Vector3(1, 0, 0));
	Vector3 jump = playerRotation.rotateVector(Vector3(0, 1, 0));
	Vector3 playerVel;


	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) && player->isGrounded == true) {
		playerSpeed = 20.0f * seconds_elapsed;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_W)) { 
		playerVel = playerVel + (forward * playerSpeed); 
		player->walkingBackwards = false;
		player->currentAnim = ANIM_ID::WALK;
		world.sPressed = false;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S)) {
		playerVel = playerVel - (forward * playerSpeed);
		//player->model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
		player->walkingBackwards = true;
		player->currentAnim = ANIM_ID::WALK;
		world.sPressed = true;

	}
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) {
		playerVel = playerVel - (right * playerSpeed);
		player->currentAnim = ANIM_ID::WALK;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_E)) {
		playerVel = playerVel + (right * playerSpeed);
		player->currentAnim = ANIM_ID::WALK;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) {
		player->currentAnim = ANIM_ID::RUN;
	}
	if (Input::isMousePressed(SDL_BUTTON_LMASK) && player->currentItem != ITEM_ID::NONE) {
		player->currentAnim = ANIM_ID::ATTACK;
		player->time = 0.0f;
		player->animDuration = player->animations[player->currentAnim]->duration / 1.25;
		player->animTimer = player->animations[player->currentAnim]->duration / 1.25;
	}

	//jump
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && player->isGrounded == true) {
		player->time = 0.0f;
		player->isGrounded = false;
		player->animDuration = player->animations[player->currentAnim]->duration / 1.5;
		player->animTimer = player->animations[player->currentAnim]->duration / 1.5;
		
		//playerVel = playerVel + (jump * sqrtf(2.0f * gravity * jumpHeight));
		player->jumpLock = 0.2f;
	}

	if (player->jumpLock != 0.0f)
	{
		playerVel[1] += 0.15f;
	}
	if (player->isGrounded == false)
	{
		player->currentAnim = ANIM_ID::JUMP;
		playerVel[1] -= seconds_elapsed * 3;
	}

	if (Input::isKeyPressed(SDL_SCANCODE_G)) {
		takeEntity(game->camera);
		checkIfFinish(game->camera);
	}

	if (Input::isKeyPressed(SDL_SCANCODE_U)) {
		for (size_t i = 0; i < world.enemies.size(); i++)
		{
			world.enemies[i]->sword->model.translateGlobal(10, 0, 0);
		}
		
	}

	Vector3 targetPos = player->pos + playerVel;
		
	player->pos = checkCollision(targetPos);
	//player->pos = targetPos;

	handleEnemies(seconds_elapsed);

}

void transitionStage::render()
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	// set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);
	// set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera as default
	game->camera->enable();

	if (world.levelDone) {
		drawText(80, 80, "YOU WIN THE LEVEL", Vector3(1, 1, 1), 6);
	}
	else {
		drawText(80, 80, "YOU LOSE THE LEVEL", Vector3(1, 1, 1), 6);
	}

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(game->window);
}

void transitionStage::update(float seconds_elapsed)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) {
		
		world.loadLevel();
	}
}

void editorStage::render()
{
}

void editorStage::update(float seconds_elapsed)
{
	//std::cout << "EDITOR STAGE" << std::endl;
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	float speed = seconds_elapsed * 10; // the speed is defined by the seconds_elapsed so it goes constant
	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		world.currentStage = STAGE_ID::PLAY;
	}
	if ((Input::mouse_state & SDL_BUTTON_LEFT)) //is left button pressed?
	{
		game->camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		game->camera->rotate(Input::mouse_delta.y * 0.005f, game->camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}
	SDL_ShowCursor(true);
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W)) game->camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S)) game->camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A)) game->camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D)) game->camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_UP)) game->camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) game->camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
}

void endStage::render()
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	// set the clear color (the background color)
	glClearColor(1.0, 0.0, 1.0, 1.0);
	// set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera as default
	game->camera->enable();

	//renderWorld();
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(game->window);
}

void endStage::update(float seconds_elapsed)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	//std::cout << "END STAGE" << std::endl;
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) {
		world.currentStage = STAGE_ID::PLAY;
		world.loadLevel();
	}
}

int getFreeBullet() {
	Game* game = Game::instance;
	World& world = game->world;
	for (size_t i = 0; i < world.numBullets; i++)
	{

		sBullet& currentBullet = world.bullets[i];
		if (currentBullet.isActive())
			return i;
	}
	return -1;
}

bool spawnBullet(sBullet& newBulletData, int enemyIndex) {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	int index = getFreeBullet();

	if (index == -1)
	{
		return false;
	}

	sBullet& bullet =world.bullets[index];
	
	bullet.model = Matrix44();
	bullet.last_position = Vector3(world.enemies[enemyIndex]->pos.x, 0.0f, world.enemies[enemyIndex]->pos.z);
	bullet.model.setTranslation(bullet.last_position.x, bullet.last_position.y, bullet.last_position.z);
	bullet.velocity = newBulletData.velocity;
	bullet.ttl = newBulletData.ttl;
	bullet.power = newBulletData.power;
	//asignar los valores de new bullet a bullet
}

void renderWorld() {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	std::vector<EntityEnemy*>& s_enemies = Game::instance->world.enemies;

	glDisable(GL_DEPTH_TEST);
	//world.skyModel.translate(game->camera->eye.x, game->camera->eye.y - 10.0f, game->camera->eye.z);
	world.sky->render();
	glEnable(GL_DEPTH_TEST);

	world.ground->render();
	world.finish->render();



	for (size_t i = 0; i < world.numBullets; i++)
	{
		sBullet& currentBullet = world.bullets[i];
		if (currentBullet.isActive()) {
			continue;
		}
		
		//first check if we have build the collision model
		//Vector3 currentPos = currentBullet.last_position;
		//currentBullet.model.translate(currentPos.x, currentPos.y, currentPos.z);
		//currentBullet.mesh->render();
		renderMesh(GL_TRIANGLES, currentBullet.model, currentBullet.mesh, currentBullet.texture, currentBullet.shader, Game::instance->camera);
	}


	for (size_t i = 0; i < world.static_entities.size(); i++)
	{
		EntityMesh* entity = world.static_entities[i];
		entity->render();
	}

	for (size_t i = 0; i < world.chests.size(); i++)
	{
		EntityChest* entity = world.chests[i];
		entity->render();
	}

	for (size_t i = 0; i < world.enemies.size(); i++)
	{
		EntityEnemy* enemy = world.enemies[i];
		enemy->render();
		enemy->resultSk = enemy->animations[enemy->currentAnim]->skeleton;
		Matrix44 handLocalMatrix = enemy->resultSk.getBoneMatrix("mixamorig_RightHandThumb1", false);
		handLocalMatrix.rotate(120 * DEG2RAD, Vector3(1, 0, 0));
		handLocalMatrix.rotate(100 * DEG2RAD, Vector3(0, 1, 0));
		handLocalMatrix.translateGlobal(20, 0, 20);
		Matrix44& actualModel = enemy->sword->model;
		actualModel = handLocalMatrix * enemy->visualModel;
		enemy->swordModel = actualModel;
		enemy->swordModel.scale(80.0f, 80.0f, 80.0f);
		enemy->swordModel.translate(-0.05, 0, 0.35);
		enemy->sword->mesh->renderBounding(enemy->swordModel);
		enemy->sword->render();

		Vector3 toTarget = player->pos - enemy->pos;
		float dist = toTarget.length();

		//RENDER ALL GUI
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//Vector3 enemyPos = enemy->pos + Vector3(0, 4, 0);
		Matrix44 ePos = enemy->resultSk.getBoneMatrix("mixamorig_Head", false) * enemy->visualModel;
		Vector3 enemyPos = ePos * Vector3(0, 1, 0);
		enemyPos = enemyPos + Vector3(0, 2, 0);
		//Vector3 enemyPos = enemy->resultSk.getBoneMatrix("mixamorig_Head", false) * Vector3(0, 2, 0);
		game->camera->updateViewMatrix();
		game->camera->updateProjectionMatrix();
		Vector3 playerScreen = game->camera->project(enemyPos, game->window_width, game->window_height);

		Texture* heartTex = Texture::Get("data/heart.png");

		for (size_t i = 0; i < enemy->hearts; i++)
		{
			//renderGUI(30 + 50 * i, 100, 100.0f, 100.0f, heartTex, true);
			if (dist < 6.0f || enemy->markedTarget)
			{
				if (playerScreen.z < 1.0f)
					renderGUI(playerScreen.x - 25 + 25 * i, game->window_height - playerScreen.y, 50.0f, 50.0f, heartTex, true);
			}
			
		}

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}
}

void setCamera(Camera *cam, Matrix44 model)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	Vector3 bottomCharacter = Vector3(player->pos.x, player->pos.y - 5.0f, player->pos.z);
	bottomCharacter = game->camera->project(bottomCharacter, game->window_width, game->window_height);

	if (player->cameraLocked)
	{
		Vector3 eye = model * Vector3(0.0f, 3.0f, 4.0f);
		Vector3 center = model * Vector3(0.0f, 0.0f, -10.0f);
		Vector3 up = model.rotateVector(Vector3(0.0f, 1.0f, 0.0f));

		if (player->firstPerson) {
			Matrix44 camModel = player->model;
			camModel.rotate(player->pitch * DEG2RAD, Vector3(1, 0, 0));

			eye = player->model * Vector3(0.0f, 1.0f, -0.5f);
			center = eye + camModel.rotateVector(Vector3(0.0f, 0.0f, -1.0f));
			up = camModel.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
		}

		Vector3 dir = cam->getRayDirection(bottomCharacter.x, bottomCharacter.y, game->window_width, game->window_height);
		Vector3 rayOrigin = cam->eye;

		for (size_t i = 0; i < world.static_entities.size(); i++)
		{
			EntityMesh* entity = world.static_entities[i];
			Vector3 pos;
			Vector3 normal;
			//if ((entity->collision))
			if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal) && (world.sPressed))
			{
				eye = model * Vector3(0.0f, 6.0f, -4.0f);
				center = model * Vector3(0.0f, -2.0f, 10.0f);
			}
		}

		cam->lookAt(eye, center, up);
	}	
}

Vector3 checkCollision(Vector3 target)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	Vector3 coll;
	Vector3 collnorm;
	Vector3 pushAway;
	Vector3 returned;

	Vector3 centerCharacter = target + Vector3(0.0f, 1.0f, 0.0f);
	Vector3 bottomCharacter = target + Vector3(0.0f, 0.0f, 0.0f);
	
	for (size_t i = 0; i < world.enemies.size(); i++)
	{
		if (world.enemies[i]->sword->mesh->testSphereCollision(world.enemies[i]->swordModel, centerCharacter, 0.75, coll, collnorm) && player->hitTimer == 0.0f 
			&& (world.enemies[i]->currentAnim == ANIM_ID::ATTACK && world.enemies[i]->animTimer >= 0.55f)) {
			player->hitTimer = player->animations[player->currentAnim]->duration;
			player->hearts -= 1;
			player->currentAnim = ANIM_ID::HIT;
			player->animDuration = player->animations[player->currentAnim]->duration;
			player->animTimer = player->animations[player->currentAnim]->duration;
		}
	}

	for (size_t i = 0; i < world.collidable_entities.size(); i++)
	{
		Vector3 posEnt = world.collidable_entities[i]->model.getTranslation();

		if (world.collidable_entities[i]->mesh->testSphereCollision(world.collidable_entities[i]->model, bottomCharacter, 0.1, coll, collnorm)) {
			if (player->pos.y >= coll.y)
			{
				std::cout << "TOP COLLISION" << std::endl;
				Game::instance->world.player->isGrounded = true;
				player->isGrounded = true;
				target.y = coll.y + 0.05f;
				return target;
			}
		}
		else if (world.ground->mesh->testSphereCollision(world.ground->model, bottomCharacter, 0.1, coll, collnorm))
		{
				player->isGrounded = true;
		}
		else {
				player->isGrounded = false;
		}
		if (world.collidable_entities[i]->mesh->testSphereCollision(world.collidable_entities[i]->model, centerCharacter, 0.25, coll, collnorm))
		{
			std::cout << "COLLISION fromt" << std::endl;

			world.collidable_entities[i]->collision = true;

			pushAway = normalize(coll - centerCharacter) * game->elapsed_time;

			returned = player->pos - pushAway;
			returned.y = player->pos.y;
			return returned;
		}			
	}
	return target;
}

Vector3 enemyCollision(EntityEnemy* enemy , Vector3 target)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	Vector3 coll;
	Vector3 collnorm;
	Vector3 pushAway;
	Vector3 returned;

	Vector3 centerCharacter = target + Vector3(0.0f, 1.0f, 0.0f);

	for (size_t i = 0; i < world.collidable_entities.size(); i++)
	{
		Vector3 posEnt = world.collidable_entities[i]->model.getTranslation();
		if (world.collidable_entities[i]->mesh->testSphereCollision(world.collidable_entities[i]->model, centerCharacter, 0.25, coll, collnorm))
		{
			std::cout << "COLLISION fromt" << std::endl;

			world.collidable_entities[i]->collision = true;

			pushAway = normalize(coll - centerCharacter) * game->elapsed_time;

			returned = enemy->pos - pushAway;
			returned.y = enemy->pos.y;
			return returned;
		}
	}
	return target;
}

void addEntityInFront(Camera* cam, const char* meshName, const char* textName) {

	Vector2 mouse = Input::mouse_position;

	Game* game = Game::instance;

	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, game->window_width, game->window_height);
	Vector3 rayOrigin = cam->eye;


	Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
	Matrix44 model;
	model.translate(spawnPos.x, spawnPos.y + 5.0f, spawnPos.z);
	model.scale(3.0f, 3.0f, 3.0f);
	EntityMesh* entity = new EntityMesh(GL_TRIANGLES, model, Mesh::Get(meshName), Texture::Get(textName), Game::instance->world.shader);
	Game::instance->world.static_entities.push_back(entity);
}

EntityMesh* rayPick(Camera* cam) {

	Vector2 mouse = Input::mouse_position;

	Game* game = Game::instance;
	World& world = game->world;

	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, game->window_width, game->window_height);
	Vector3 rayOrigin = cam->eye;

	for (size_t i = 0; i < world.static_entities.size(); i++)
	{
		EntityMesh* entity = world.static_entities[i];
		Vector3 pos;
		Vector3 normal;
		if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal))
		{
			std::cout << "RayPicked: " << entity->tiling << std::endl;
			world.selectedEntity = entity;
			break;
		}
	}
	return world.selectedEntity;
}

void takeEntity(Camera* cam) {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	std::vector<EntityChest*>& chests_address = world.chests;

	Vector2 mouse = Input::mouse_position;

	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, game->window_width, game->window_height);
	Vector3 rayOrigin = cam->eye;

	Vector3 playerPos = player->visualModel.getTranslation();
	

	for (size_t i = 0; i < Game::instance->world.chests.size(); i++)
	{
		EntityMesh* chest = world.chests[i]->mesh;
		EntityMesh* content = world.chests[i]->content;
		Vector3 pos;
		Vector3 normal;

		Vector3 entityPos = chest->model.getTranslation();

		//std::cout << "Player distance to object: " << playerPos.distance(entityPos) << std::endl;
		

		if (chest->mesh->testRayCollision(chest->model, rayOrigin, dir, pos, normal) && (playerPos.distance(entityPos) <= 3.0f))
		{
			std::cout << "Object selected: " << std::endl;
			player->inventory.push_back(content);
			player->currentItem = ITEM_ID::SWORD;
			chests_address.erase(world.chests.begin() + i);
			for (size_t i = 0; i < world.collidable_entities.size(); i++)
			{
				if (world.collidable_entities[i]->id == ENTITY_ID::ENTITY_CHEST)
				{
					world.collidable_entities.erase(world.collidable_entities.begin() + i);
				}
			}
			break;
		}
	}
}

void checkIfFinish(Camera* cam) {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	std::vector<EntityMesh*>& s_entities = Game::instance->world.static_entities;

	Vector2 mouse = Input::mouse_position;

	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, game->window_width, game->window_height);
	Vector3 rayOrigin = cam->eye;

	Vector3 playerPos = player->visualModel.getTranslation();

	EntityMesh* entity = Game::instance->world.finish;
	Vector3 pos;
	Vector3 normal;

	Vector3 entityPos = entity->model.getTranslation();

	//std::cout << "Player distance to object: " << playerPos.distance(entityPos) << std::endl;

	if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal) && (playerPos.distance(entityPos) <= 2.0f))
	{
		world.levelDone = true;
		world.currentStage = STAGE_ID::TRANSITION;
	}
	
}


void rotateSelected(float angleDegrees) {
	Game* game = Game::instance;
	World& world = game->world;
	EntityMesh* selectedEntity = world.selectedEntity;

	if (selectedEntity == NULL) {
		return;
	}
	else {
		std::cout << "aaaaaa: " << std::endl;
	}
	selectedEntity->model.rotate(angleDegrees * DEG2RAD, Vector3(0, 1, 0));
}

void playerInventory() {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	if (player->currentItem != ITEM_ID::NONE) {
		player->resultSk = player->animations[player->currentAnim]->skeleton;
		Matrix44 handLocalMatrix = player->resultSk.getBoneMatrix("mixamorig_RightHandThumb1", false);
		handLocalMatrix.rotate(120 * DEG2RAD, Vector3(1, 0, 0));
		handLocalMatrix.rotate(100 * DEG2RAD, Vector3(0, 1, 0));
		handLocalMatrix.translateGlobal(-10, 0, 0);
		Matrix44& actualModel = player->inventory[player->currentItem]->model;
		actualModel = handLocalMatrix * player->visualModel;
		actualModel.scale(1.5f, 1.5f, 1.5f);
		player->inventory[player->currentItem]->render();

		player->swordModel = actualModel;
		player->swordModel.scale(80.0f, 80.0f, 80.0f);
		//player->swordModel.translateGlobal(-0.5, 0, 0);
		player->inventory[player->currentItem]->mesh->renderBounding(player->swordModel);
		player->inventory[player->currentItem]->render();
	}
}

void playerGUI() {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	//RENDER ALL GUI
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Texture* heartTex = Texture::Get("data/heart.png");

	for (size_t i = 0; i < player->hearts; i++)
	{
		renderGUI(30 + 50 * i, 100, 100.0f, 100.0f, heartTex, true);
	}


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void handleEnemies(float seconds_elapsed) {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	for (size_t i = 0; i < world.enemies.size(); i++)
	{
		EntityEnemy* enemy = world.enemies[i];
		Matrix44 enemyModel = enemy->model;
		Vector3 side = enemyModel.rotateVector(Vector3(1, 0, 0)).normalize();
		Vector3 forward = enemyModel.rotateVector(Vector3(0, 0, -1)).normalize();

		Vector3 toTarget = player->pos - enemy->pos;
		float dist = toTarget.length();
		toTarget.normalize();

		float sideDot = side.dot(toTarget);
		float forwardDot = forward.dot(toTarget);

		if (forwardDot < 0.98f)
		{
			enemy->jaw += sign(sideDot) * 90.0f * seconds_elapsed;
		}
		if ((dist < 6.0f || enemy->markedTarget) && enemy->hitTimer <= 0.0f && enemy->hearts > 0)
		{
			
			
			//std::cout << "anim timer: " << enemy->animTimer << std::endl;
			if (enemy->currentAnim != ANIM_ID::ATTACK || (enemy->currentAnim == ANIM_ID::ATTACK && enemy->animTimer <= 0.0f))
			{
				enemy->currentAnim = ANIM_ID::ATTACK;
				enemy->time = 0.0f;
				enemy->animDuration = player->animations[player->currentAnim]->duration / 1.25;
				enemy->animTimer = player->animations[player->currentAnim]->duration / 1.25;
			}
			enemy->markedTarget = true;
			if (dist > 1.0f){
				Vector3 targetPos = enemy->pos + (forward * 3.0f * seconds_elapsed);
				enemy->pos = enemyCollision(enemy, targetPos);
			}
			if (!world.bulletOnce)
			{
				world.bulletOnce = true;
				sBullet bullet;
				bullet.ttl = 20.0f;
				bullet.mesh = Mesh::Get("data/box.obj");
				bullet.texture = Texture::Get("data/PolygonMinis_Texture_01_A.png");
				bullet.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
				bullet.power = 1;
				bullet.velocity = Vector3(30,30,30) * forward;
				if (spawnBullet(bullet, i))
				{
					std::cout << "Bullet spawned" << std::endl;
				}
				else
				{
					std::cout << "Bullet NOT spawned" << std::endl;
				}
			}
		}
		Vector3 coll;
		Vector3 collnorm;
		Vector3 pushAway;
		Vector3 returned;

		Vector3 centerCharacter = enemy->pos + Vector3(0.0f, 1.0f, 0.0f);

		if (player->currentItem != ITEM_ID::NONE)
		{
			if (player->inventory[player->currentItem]->mesh->testSphereCollision(player->swordModel, centerCharacter, 0.75, coll, collnorm) && enemy->hitTimer == 0.0f 
				&& (player->currentAnim == ANIM_ID::ATTACK && player->animTimer >= 0.55f)) {
				enemy->time = 0.0f;
				enemy->hitTimer = player->animations[player->currentAnim]->duration / 2;
				enemy->hearts -= 1;
				enemy->currentAnim = ANIM_ID::HIT;
				enemy->animDuration = player->animations[player->currentAnim]->duration / 2;
				enemy->animTimer = player->animations[player->currentAnim]->duration / 2;
				if (enemy->hearts <= 0)
				{
					enemy->time = 0.0f;
					enemy->currentAnim = ANIM_ID::DEAD;
					enemy->animDuration = player->animations[player->currentAnim]->duration / 0.75f;
					enemy->animTimer = player->animations[player->currentAnim]->duration / 0.75f;

				}
			}
		}

	}
}

float sign(float value) {
	return value >= 0.0f ? 1.0f : -1.0f;
}