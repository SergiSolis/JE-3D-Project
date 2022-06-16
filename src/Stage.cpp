#include "Stage.h"
#include "game.h"
#include "utils.h"
#include "fbo.h"
#include <cmath>
#include "animation.h"
#include "input.h"


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
	
	if (player->currentItem != ITEM_ID::NONE) {
		player->resultSk = player->animations[player->currentAnim]->skeleton;
		Matrix44 handLocalMatrix = player->resultSk.getBoneMatrix("mixamorig_RightHandIndex4", false);
		handLocalMatrix.scale(0.5f, 0.5f, 0.5f);
		Matrix44& actualModel = player->inventory[player->currentItem]->model;
		actualModel = handLocalMatrix * player->visualModel;
		player->inventory[player->currentItem]->render();
	}
		
	//Draw the floor grid
	//drawGrid();

	//RENDER ALL GUI
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Vector3 playerPos = player->pos + Vector3(0, 4, 0);
	game->camera->updateViewMatrix();
	game->camera->updateProjectionMatrix();
	Vector3 playerScreen = game->camera->project(playerPos, game->window_width, game->window_height);
	Texture* texture = Texture::Get("data/fangs.png");
	if(playerScreen.z < 1.0f)
		renderGUI(playerScreen.x, game->window_width - playerScreen.y, 50.0f, 50.0f, texture, true);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

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

	std::cout << "Enemies:" << world.enemies.size() << std::endl;

	player->currentAnim = ANIM_ID::IDLE;

	float speed = seconds_elapsed * 10; // the speed is defined by the seconds_elapsed so it goes constant

	player->jumpLock = max(0.0f, player->jumpLock - (seconds_elapsed));

	//example
	world.angle += (float)seconds_elapsed * 10.0f;

	world.timeTrial -= seconds_elapsed;
	if (world.timeTrial <= 0.0f) {
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
		player->currentAnim = ANIM_ID::WALK;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S)) {
		playerVel = playerVel - (forward * playerSpeed);
		player->currentAnim = ANIM_ID::WALK;
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

	//jump
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && player->isGrounded == true) {
		player->isGrounded = false;
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
		takeEntity(game->camera, world.static_entities);
		checkIfFinish(game->camera);
	}

	Vector3 targetPos = player->pos + playerVel;
		
	player->pos = checkCollision(targetPos);
	//player->pos = targetPos;

	orientationEnemies(seconds_elapsed);

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

void renderWorld() {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	glDisable(GL_DEPTH_TEST);
	//world.skyModel.translate(game->camera->eye.x, game->camera->eye.y - 10.0f, game->camera->eye.z);
	world.sky->render();
	glEnable(GL_DEPTH_TEST);

	world.ground->render();
	world.finish->render();

	for (size_t i = 0; i < world.enemies.size(); i++)
	{
		EntityEnemy* enemy = world.enemies[i];
		enemy->render();
	}

	for (size_t i = 0; i < world.static_entities.size(); i++)
	{
		EntityMesh* entity = world.static_entities[i];
		entity->render();
	}
}

void renderGUI(float x, float y, float w, float h, Texture* tex, bool flipYV) {
	int windowWidth = Game::instance->window_width;
	int windowHeight = Game::instance->window_height;
	Mesh quad;
	quad.createQuad(x, y, w, h, flipYV);

	Camera cam2D;
	cam2D.setOrthographic(0, windowWidth, windowHeight, 0, -1, 1);
	assert(mesh != NULL, "mesh in renderMesh was null");

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	
	if (!shader) return;

	//enable shader
	shader->enable();

	//upload uniforms
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
	if (tex != NULL)
	{
		shader->setUniform("u_texture", tex, 0);
	}
	shader->setUniform("u_time", time);
	shader->setUniform("u_text_tiling", 1.0f);
	Matrix44 quadModel;
	//quadModel.translate(sin(Game::instance->time ) * 20, 0, 0);
	shader->setUniform("u_model", quadModel);
	//do the draw call
	quad.render(GL_TRIANGLES);

	//disable shader
	shader->disable();
}

void setCamera(Camera *cam, Matrix44 model)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

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
	
	for (size_t i = 0; i < world.static_entities.size(); i++)
	{
		Vector3 posEnt = world.static_entities[i]->model.getTranslation();
			if (world.static_entities[i]->mesh->testSphereCollision(world.static_entities[i]->model, bottomCharacter, 0.1, coll, collnorm)) {
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
			if (world.static_entities[i]->mesh->testSphereCollision(world.static_entities[i]->model, centerCharacter, 0.25, coll, collnorm))
			{
				std::cout << "COLLISION fromt" << std::endl;
				pushAway = normalize(coll - centerCharacter) * game->elapsed_time;

				returned = player->pos - pushAway;
				returned.y = player->pos.y;
				return returned;
			}			
	}
	return target;
}

Vector3 checkCollisionBottom(Vector3 target)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	Vector3 coll;
	Vector3 coll2;
	Vector3 collnorm;

	Vector3 bottomCharacter = target + Vector3(0.0f, 0.0f, 0.0f);



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

void takeEntity(Camera* cam, std::vector<EntityMesh*>& entities) {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	std::vector<EntityMesh*>& s_entities = Game::instance->world.static_entities;

	Vector2 mouse = Input::mouse_position;

	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, game->window_width, game->window_height);
	Vector3 rayOrigin = cam->eye;

	Vector3 playerPos = player->visualModel.getTranslation();
	

	for (size_t i = 0; i < Game::instance->world.static_entities.size(); i++)
	{
		EntityMesh* entity = Game::instance->world.static_entities[i];
		Vector3 pos;
		Vector3 normal;

		Vector3 entityPos = entity->model.getTranslation();

		//std::cout << "Player distance to object: " << playerPos.distance(entityPos) << std::endl;
		

		if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal) && (playerPos.distance(entityPos) <= 2.0f))
		{
			std::cout << "Object selected: " << entity->tiling << std::endl;
			if (entity->id == ENTITY_ID::WALL_ID)
			{
				player->currentItem = ITEM_ID::WALL_HAND;
			}
			else
			{
				player->currentItem = ITEM_ID::BOX_HAND;
			}
			
			s_entities.erase(Game::instance->world.static_entities.begin() + i);
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
	world.levelDone = true;

	if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal) && (playerPos.distance(entityPos) <= 2.0f))
	{
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

void orientationEnemies(float seconds_elapsed) {
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
		/*
		if (dist > 4.0f)
		{
			enemy->pos = enemy->pos + (forward * 10.0f * seconds_elapsed);
		}
		*/

	}
}

float sign(float value) {
	return value >= 0.0f ? 1.0f : -1.0f;
}