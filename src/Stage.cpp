#include "Stage.h"
#include "game.h"
#include "utils.h"
#include "fbo.h"
#include <cmath>
#include "animation.h"
#include "input.h"
#include "extra/coldet/coldet.h"
#include <bass.h>


void titleStage::render()
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

	drawText(200, 60, "TITLE", Vector3(1, 0, 1), 10);

	if (world.titleOption == TITLE_OPTIONS::PLAY_GAME)
	{
		if (int(game->time) % 2 == 0) {
			drawText(100, 200, "PLAY", Vector3(1, 1, 1), 6);
		}
	}
	else
	{
		drawText(100, 200, "PLAY", Vector3(1, 1, 1), 6);
	}
	if (world.titleOption == TITLE_OPTIONS::PLAY_TUTORIAL)
	{
		if (int(game->time) % 2 == 0) {
			drawText(100, 260, "TUTORIAL", Vector3(1, 1, 1), 6);
		}
	}
	else
	{
		drawText(100, 260, "TUTORIAL", Vector3(1, 1, 1), 6);
	}
	if (world.titleOption == TITLE_OPTIONS::PLAY_EXIT)
	{
		if (int(game->time) % 2 == 0) {
			drawText(100, 320, "EXIT", Vector3(1, 1, 1), 6);
		}
	}
	else
	{
		drawText(100, 320, "EXIT", Vector3(1, 1, 1), 6);
	}
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(game->window);
}

void titleStage::update(float seconds_elapsed)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	if (Input::wasKeyPressed(SDL_SCANCODE_K))
	{

	}
	if (Input::wasKeyPressed(SDL_SCANCODE_ESCAPE))
	{
		world.level_info.tag = ACTION_ID::PAUSE;
		world.currentStage = STAGE_ID::MENU;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_UP)) {
		world.titleOption = static_cast<TITLE_OPTIONS>((world.titleOption - 1) % (TITLE_OPTIONS::PLAY_EXIT + 1));
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_DOWN)) {
		world.titleOption = static_cast<TITLE_OPTIONS>((world.titleOption + 1) % (TITLE_OPTIONS::PLAY_EXIT + 1));
	}
	if ((Input::wasKeyPressed(SDL_SCANCODE_RETURN) || Input::wasKeyPressed(SDL_SCANCODE_K)) && world.titleOption == TITLE_OPTIONS::PLAY_GAME)
	{
		world.titleOption = TITLE_OPTIONS::PLAY_GAME;
		world.level_info.level = 1;
		world.level_info.tag = ACTION_ID::NO_ACTION;
		world.loadLevel();
		world.currentStage = STAGE_ID::PLAY;
	}
	else if ((Input::wasKeyPressed(SDL_SCANCODE_RETURN) || Input::wasKeyPressed(SDL_SCANCODE_K)) && world.titleOption == TITLE_OPTIONS::PLAY_TUTORIAL)
	{
		world.titleOption = TITLE_OPTIONS::PLAY_GAME;
		world.level_info.level = 0;
		world.level_info.tag = ACTION_ID::NO_ACTION;
		world.loadLevel();
		world.currentStage = STAGE_ID::TUTORIAL;
	}
	else if ((Input::wasKeyPressed(SDL_SCANCODE_RETURN) || Input::wasKeyPressed(SDL_SCANCODE_K)) && world.titleOption == TITLE_OPTIONS::PLAY_EXIT)
	{
		game->must_exit = true;
	}
}

void tutorialStage::render()
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

	if (world.currentStage == STAGE_ID::TUTORIAL)
		setCamera(game->camera, player->model);

	player->render();

	playerInventory();

	//Draw the floor grid
	//drawGrid();

	glDisable(GL_BLEND);

	playerGUI();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	drawText(2, 50, "Actual level: " + std::to_string(world.level_info.level), Vector3(1, 1, 0), 2);

	//RENDER ALL GUI
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Texture* controlsTex = Texture::Get("data/controls.png");
	renderGUI(600, 100, 300.0f, 100.0f, controlsTex, true);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(game->window);
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
	//drawText(2, 25, "Time trial: "  + std::to_string(world.timeTrial), Vector3(1, 1, 0), 2);
	drawText(2, 25, "Actual level: " + std::to_string(world.level_info.level), Vector3(1, 1, 0), 2);

	if (player->currentItem == ITEM_ID::NONE)
	{
		drawText(170, 90, "YOU MUST FIND A WEAPON TO FIGHT", Vector3(1, 1, 1), 3);
	}

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
	//std::cout << "Enemies:" << s_enemies.size() << std::endl;

	bulletCollision(seconds_elapsed);

	world.level_info.space_pressed = max(0.0f, world.level_info.space_pressed - seconds_elapsed);

	player->time += seconds_elapsed;

	if (player->animTimer <= 0.0f)
	{
		if (player->currentItem == ITEM_ID::NONE)
		{
			player->currentAnim = PLAYER_ANIM_ID::PLAYER_IDLE;
		}
		else {
			player->currentAnim = PLAYER_ANIM_ID::PLAYER_WEAPON_IDLE;
		}
		
	}

	for (size_t i = 0; i < s_enemies.size(); i++)
	{
		s_enemies[i]->hitTimer = max(0.0f, s_enemies[i]->hitTimer - seconds_elapsed);

		if (s_enemies[i]->type == ENEMY_ID::ARCHER)
		{
			s_enemies[i]->shootTimer = max(0.0f, s_enemies[i]->shootTimer - seconds_elapsed);
		}
	}

	for (size_t i = 0; i < s_enemies.size(); i++)
	{
		s_enemies[i]->time += seconds_elapsed;
		s_enemies[i]->animTimer = max(0.0f, s_enemies[i]->animTimer - (seconds_elapsed / s_enemies[i]->animDuration));

		if (s_enemies[i]->animTimer <= 0.0f && s_enemies[i]->hearts <= 0) {
			s_enemies.erase(s_enemies.begin() + i);
		}
		if (s_enemies[i]->currentAnim == PLAYER_ANIM_ID::PLAYER_DEAD)
		{
			std::cout << "Anim duration: " << s_enemies[i]->animTimer << std::endl;
		}
	}

	if (s_enemies.size() <= 0 || world.level_info.level == 0)
	{
		EntityMesh* entityFinish = Game::instance->world.finish;
		entityFinish->mesh = world.exit_open;

	}

	player->animTimer = max(0.0f, player->animTimer - (seconds_elapsed / player->animDuration));

	player->jumpLock = max(0.0f, player->jumpLock - seconds_elapsed);
	player->hitTimer = max(0.0f, player->hitTimer - seconds_elapsed);
	
	//world.timeTrial -= seconds_elapsed;
	//if (world.timeTrial <= 0.0f || (player->hearts <= 0 && player->animTimer <= 0.0f)) {
	if (player->hearts <= 0 && player->animTimer <= 0.0f) {
		world.level_info.tag = ACTION_ID::DEAD;
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
		if (Input::isKeyPressed(SDL_SCANCODE_D)) {
			player->jaw += rotateSpeed;


		}
		if (Input::isKeyPressed(SDL_SCANCODE_A)) {
			player->jaw -= rotateSpeed;


		}
	}
	Matrix44 playerRotation;
	playerRotation.rotate(player->jaw * DEG2RAD, Vector3(0, 1, 0));
	Vector3 forward = playerRotation.rotateVector(Vector3(0, 0, -1));
	Vector3 right = playerRotation.rotateVector(Vector3(1, 0, 0));
	Vector3 jump = playerRotation.rotateVector(Vector3(0, 1, 0));
	Vector3 playerVel;

	player->isMoving = false;

	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) && player->isGrounded == true && player->currentAnim != PLAYER_ANIM_ID::PLAYER_DEAD) {
		playerSpeed = player->runSpeed * seconds_elapsed;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_W) && player->currentAnim != PLAYER_ANIM_ID::PLAYER_DEAD) {
		player->isMoving = true;
		playerVel = playerVel + (forward * playerSpeed);
		player->walkingBackwards = false;
		player->currentAnim = PLAYER_ANIM_ID::PLAYER_WALK;
		world.sPressed = false;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S) && player->currentAnim != PLAYER_ANIM_ID::PLAYER_DEAD) {
		player->isMoving = true;
		playerVel = playerVel - (forward * playerSpeed);
		player->walkingBackwards = true;
		player->currentAnim = PLAYER_ANIM_ID::PLAYER_WALK;
		world.sPressed = true;

	}
	if (Input::isKeyPressed(SDL_SCANCODE_Q) && player->currentAnim != PLAYER_ANIM_ID::PLAYER_DEAD) {
		player->isMoving = true;
		if (!world.camera_inverse)
		{
			playerVel = playerVel - (right * playerSpeed);
		}
		else
		{
			playerVel = playerVel + (right * playerSpeed);
		}

		player->currentAnim = PLAYER_ANIM_ID::PLAYER_WALK;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_E) && player->currentAnim != PLAYER_ANIM_ID::PLAYER_DEAD) {
		player->isMoving = true;
		if (!world.camera_inverse)
		{
			playerVel = playerVel + (right * playerSpeed);
		}
		else
		{
			playerVel = playerVel - (right * playerSpeed);
		}
		player->currentAnim = PLAYER_ANIM_ID::PLAYER_WALK;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) && player->currentAnim != PLAYER_ANIM_ID::PLAYER_DEAD && player->isMoving == true) {
		player->currentAnim = PLAYER_ANIM_ID::PLAYER_RUN;
	}
	if (Input::isMousePressed(SDL_BUTTON_LMASK) && player->currentItem != ITEM_ID::NONE && player->currentAnim != PLAYER_ANIM_ID::PLAYER_DEAD) {
		player->currentAnim = PLAYER_ANIM_ID::PLAYER_ATTACK;
		player->time = 0.0f;
		player->animDuration = player->animations[player->currentAnim]->duration / 1.25;
		player->animTimer = player->animations[player->currentAnim]->duration / 1.25;
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_K))
	{
		world.level_info.tag = ACTION_ID::WIN;
		player->currentItem = ITEM_ID::SWORD;
		world.currentStage = STAGE_ID::TRANSITION;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_ESCAPE))
	{
		world.level_info.tag = ACTION_ID::PAUSE;
		world.currentStage = STAGE_ID::MENU;
	}
	//jump
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && player->isGrounded == true && world.level_info.space_pressed <= 0.0f) {
		player->time = 0.0f;
		player->isGrounded = false;
		player->animDuration = player->animations[player->currentAnim]->duration / 1.5;
		player->animTimer = player->animations[player->currentAnim]->duration / 1.5;
		
		//playerVel = playerVel + (jump * sqrtf(2.0f * gravity * jumpHeight));
		player->jumpLock = 0.25f;
	}

	if (player->jumpLock != 0.0f)
	{
		playerVel[1] += 0.25f;
	}
	if (player->isGrounded == false)
	{
		player->currentAnim = PLAYER_ANIM_ID::PLAYER_JUMP;
		playerVel[1] -= seconds_elapsed * 3;
	}

	if (Input::isKeyPressed(SDL_SCANCODE_G)) {
		takeEntity(game->camera);
		checkIfFinish(game->camera);
	}

	if (Input::isKeyPressed(SDL_SCANCODE_U)) {
		for (size_t i = 0; i < world.enemies.size(); i++)
		{
			world.enemies[i]->weapon->model.translateGlobal(10, 0, 0);
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


	if (world.level_info.tag == ACTION_ID::OPEN_CHEST) {
		if (world.level_info.last_chest == CHEST_ID::CHEST_SWORD)
		{
			drawText(100, 80, "YOU HAVE OBTAINED", Vector3(1, 1, 1), 6);
			drawText(260, 160, "A SWORD", Vector3(1, 1, 1), 6);

			//RENDER ALL GUI
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			Texture* swordTex = Texture::Get("data/sword.png");
			renderGUI(400, 350, 200.0f, 200.0f, swordTex, true);


			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glDisable(GL_BLEND);
		}
		else if (world.level_info.last_chest == CHEST_ID::CHEST_HEART)
		{
			drawText(100, 80, "YOU HAVE OBTAINED", Vector3(1, 1, 1), 6);
			drawText(260, 160, "A HEART", Vector3(1, 1, 1), 6);

			//RENDER ALL GUI
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			Texture* swordTex = Texture::Get("data/heart.png");
			renderGUI(400, 350, 200.0f, 200.0f, swordTex, true);


			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glDisable(GL_BLEND);
		}
		else if (world.level_info.last_chest == CHEST_ID::CHEST_STRENGTH)
		{
			drawText(100, 80, "YOU HAVE INCREASE", Vector3(1, 1, 1), 6);
			drawText(260, 160, "YOUR STRENGTH", Vector3(1, 1, 1), 6);

			//RENDER ALL GUI
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			Texture* strengthTex = Texture::Get("data/strength.png");
			renderGUI(400, 350, 200.0f, 200.0f, strengthTex, true);


			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glDisable(GL_BLEND);
		}
		else
		{
			drawText(100, 80, "YOU HAVE INCREASE", Vector3(1, 1, 1), 6);
			drawText(260, 160, "YOUR VELOCITY", Vector3(1, 1, 1), 6);

			//RENDER ALL GUI
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			Texture* strengthTex = Texture::Get("data/velocity.png");
			renderGUI(400, 350, 200.0f, 200.0f, strengthTex, true);


			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glDisable(GL_BLEND);
		}
		
		if (int(game->time) % 2 == 0) {
			drawText(200, 500, "Press SPACE to continue", Vector3(1, 1, 1), 3);
		}


	}
	if (world.level_info.tag == ACTION_ID::WIN) {

		world.level_info.last_player_hearts = player->hearts;
		world.level_info.last_player_strength = player->strength;

		drawText(80, 80, "YOU WIN THE LEVEL", Vector3(1, 1, 1), 6);
		if (int(game->time) % 2 == 0) {
			drawText(200, 500, "Press SPACE to continue", Vector3(1, 1, 1), 3);
		}
	}
	else if (world.level_info.tag == ACTION_ID::DEAD) {
		drawText(80, 80, "YOU LOSE THE LEVEL", Vector3(1, 1, 1), 6);
		if (int(game->time) % 2 == 0) {
			drawText(200, 500, "Press SPACE to continue", Vector3(1, 1, 1), 3);
		}
	}

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(game->window);
}

void transitionStage::update(float seconds_elapsed)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;

	player->currentAnim = PLAYER_ANIM_ID::PLAYER_IDLE;
	world.level_info.space_pressed = 2.0f;

	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) {

		if (world.level_info.tag == ACTION_ID::OPEN_CHEST) {
			world.currentStage = STAGE_ID::PLAY;
		}
		else{
			world.loadLevel();
		}
		
	}if (Input::wasKeyPressed(SDL_SCANCODE_K))
	{
		world.loadLevel();
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_ESCAPE))
	{
		world.loadLevel();
		world.level_info.tag = ACTION_ID::PAUSE;
		world.currentStage = STAGE_ID::MENU;
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

	if (Input::wasKeyPressed(SDL_SCANCODE_ESCAPE))
	{
		world.level_info.tag = ACTION_ID::PAUSE;
		world.currentStage = STAGE_ID::MENU;
	}
}

void menuStage::render()
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

	drawText(100, 60, "MENU", Vector3(0, 1, 1), 8);

	if (world.menuOption == MENU_OPTIONS::RETURN)
	{
		if (int(game->time) % 2 == 0) {
			drawText(100, 200, "RETURN", Vector3(1, 1, 1), 6);
		}
	}
	else
	{
		drawText(100, 200, "RETURN", Vector3(1, 1, 1), 6);
	}
	if (world.menuOption == MENU_OPTIONS::RESTART_GAME)
	{
		if (int(game->time) % 2 == 0) {
			drawText(100, 260, "RESTART GAME", Vector3(1, 1, 1), 6);
		}
	}
	else
	{
		drawText(100, 260, "RESTART GAME", Vector3(1, 1, 1), 6);
	}
	if (world.menuOption == MENU_OPTIONS::EXIT)
	{
		if (int(game->time) % 2 == 0) {
			drawText(100, 320, "EXIT", Vector3(1, 1, 1), 6);
		}
	}
	else
	{
		drawText(100, 320, "EXIT", Vector3(1, 1, 1), 6);
	}

	//RENDER ALL GUI
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Texture* controlsTex = Texture::Get("data/controls_empty.png");
	renderGUI(600, 100, 300.0f, 100.0f, controlsTex, true);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(game->window);
}

void menuStage::update(float seconds_elapsed)
{
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	std::cout << "MENU OPTION:" << (int)world.menuOption << std::endl;
	if (Input::wasKeyPressed(SDL_SCANCODE_UP)) {
		world.menuOption = static_cast<MENU_OPTIONS>((world.menuOption - 1) % (MENU_OPTIONS::EXIT + 1));
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_DOWN)) {
		world.menuOption = static_cast<MENU_OPTIONS>((world.menuOption + 1) % (MENU_OPTIONS::EXIT + 1));
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_RETURN) && world.menuOption == MENU_OPTIONS::RETURN)
	{
		world.menuOption = MENU_OPTIONS::RETURN;
		if (world.level_info.level == 0 && world.level_info.tag == ACTION_ID::PAUSE)
		{
			world.level_info.tag = ACTION_ID::NO_ACTION;
			world.currentStage = STAGE_ID::TITLE;
		}
		else if (world.level_info.level == 0 && world.level_info.tag == ACTION_ID::PAUSE_TUTORIAL)
		{
			world.level_info.tag = ACTION_ID::NO_ACTION;
			world.currentStage = STAGE_ID::TUTORIAL;
		}
		else {
			world.level_info.tag = ACTION_ID::NO_ACTION;
			world.currentStage = STAGE_ID::PLAY;
		}
	}
	else if (Input::wasKeyPressed(SDL_SCANCODE_RETURN) && world.menuOption == MENU_OPTIONS::RESTART_GAME) 
	{
		world.menuOption = MENU_OPTIONS::RETURN;
		world.level_info.tag = ACTION_ID::NO_ACTION;
		world.currentStage = STAGE_ID::TITLE;
	}
	else if (Input::wasKeyPressed(SDL_SCANCODE_RETURN) && world.menuOption == MENU_OPTIONS::EXIT) 
	{
		world.menuOption = MENU_OPTIONS::RETURN;
		game->must_exit = true;
	}
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
	if (Input::wasKeyPressed(SDL_SCANCODE_ESCAPE))
	{
		world.level_info.tag = ACTION_ID::PAUSE;
		world.currentStage = STAGE_ID::MENU;
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

	PlayGameSound("data/arrow.wav");

	sBullet& bullet =world.bullets[index];
	
	bullet.mesh = newBulletData.mesh;
	bullet.model = Matrix44();
	bullet.last_position = Vector3(world.enemies[enemyIndex]->pos.x, 0.0f, world.enemies[enemyIndex]->pos.z);
	bullet.model.setTranslation(bullet.last_position.x, 0.75f, bullet.last_position.z);
	bullet.velocity = newBulletData.velocity;
	bullet.ttl = newBulletData.ttl;
	bullet.power = newBulletData.power;
	bullet.author = newBulletData.author;
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
		
		Matrix44 model = currentBullet.model;
		model.scale(2.0f, 2.0f, 2.0f);
		model.rotate(s_enemies[currentBullet.author]->jaw * DEG2RAD, Vector3(0, 1, 0));
		model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
		renderMesh(GL_TRIANGLES, model, currentBullet.mesh, currentBullet.texture, currentBullet.shader, Game::instance->camera);
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
		renderEnemyWeapon(enemy);
		renderEnemyGUI(enemy);
	}
}

void RenderMinimap(int widthStart)
{
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	std::vector<EntityEnemy*>& enemies = Game::instance->world.enemies;
	std::vector<EntityMesh*>& entities = Game::instance->world.static_entities;
	std::vector<EntityChest*>& chests = Game::instance->world.chests;
	int window_width = Game::instance->window_width;
	int window_height = Game::instance->window_height;

	//player 
	glViewport(window_width - 200, window_height - 200, 200, 200);
	glClear(GL_DEPTH_BUFFER_BIT);

	Camera cam;
	cam.setPerspective(60, 1, 0.1f, 1000.f);

	Matrix44 playerModel = player->model;
	//Vector3 eye = playerModel * Vector3(0.0f, 3.0f, 4.0f);
	Vector3 eye = playerModel * Vector3(0.0f, 100.0f, 0.0f);
	Vector3 center = player->pos;
	
	Vector3 up = playerModel.rotateVector(Vector3(0.0f, 0.0f, -1.0f));
	cam.lookAt(eye, center, up);

	//minimap
	renderMesh(GL_TRIANGLES, world.ground->model, world.ground->mesh, world.ground->texture, world.ground->shader, &cam, Vector4(1,1,1,1), 500.0f);

	//Use flat.fs since no texture is provided
	
	//objects of the map
	for (size_t i = 0; i < entities.size(); i++)
	{
		//optional
		Matrix44 entityModel = entities[i]->model;
		entityModel.scale(1.5f, 1.5f, 1.5f);
		renderMesh(GL_TRIANGLES, entityModel, Mesh::Get("data/sphere.obj"), NULL, shader, &cam, Vector4(0.568f, 0.568f, 0.568f, 1));

	}

	//enemies as a red point
	for (size_t i = 0; i < enemies.size(); i++)
	{
		Matrix44 enemyModel = enemies[i]->model;
		enemyModel.scale(1.5f, 1.5f, 1.5f);
		renderMesh(GL_TRIANGLES, enemyModel, Mesh::Get("data/sphere.obj"), NULL, shader, &cam, Vector4(1, 0, 0, 1));
	}
	//we as a green point

	Matrix44 playerPoint = player->model;
	playerPoint.scale(1.5f, 1.5f, 1.5f);
	renderMesh(GL_TRIANGLES, playerPoint, Mesh::Get("data/sphere.obj"), NULL, shader, &cam, Vector4(1, 1, 0, 1));
	
	//restore viewport
	glViewport(0, 0, window_width, window_height);
}

void renderEnemyWeapon(EntityEnemy* enemy) {
	enemy->resultSk = enemy->animations[enemy->currentAnim]->skeleton;
	if (enemy->type == ENEMY_ID::WARRIOR)
	{
		Matrix44 handLocalMatrix = enemy->resultSk.getBoneMatrix("mixamorig_RightHandThumb1", false);
		handLocalMatrix.rotate(60 * DEG2RAD, Vector3(1, 0, 0));
		handLocalMatrix.rotate(150 * DEG2RAD, Vector3(0, 1, 0));
		handLocalMatrix.translateGlobal(-20, -1, -17.5);
		Matrix44& actualModel = enemy->weapon->model;
		actualModel = handLocalMatrix * enemy->visualModel;
		actualModel.scale(1.5f, 1.5f, 1.5f);
		enemy->weaponModel = actualModel;
		enemy->weaponModel.rotate(-80 * DEG2RAD, Vector3(1, 0, 0));
		enemy->weaponModel.rotate(-100 * DEG2RAD, Vector3(0, 1, 0));
		enemy->weaponModel.rotate(20 * DEG2RAD, Vector3(0, 0, 1));
		enemy->weaponModel.scale(80.0f, 80.0f, 80.0f);
		enemy->weaponModel.translate(0.15, -0.15, -0.15);
		enemy->weapon->mesh->renderBounding(enemy->weaponModel);
	}
	else
	{
		Matrix44 handLocalMatrix = enemy->resultSk.getBoneMatrix("mixamorig_LeftHand", false);
		handLocalMatrix.rotate(70 * DEG2RAD, Vector3(1, 0, 0));
		handLocalMatrix.rotate(110 * DEG2RAD, Vector3(0, 0, 1));
		handLocalMatrix.rotate(300 * DEG2RAD, Vector3(0, 1, 0));
		handLocalMatrix.translateGlobal(-20, -5, 30);
		Matrix44& actualModel = enemy->weapon->model;
		actualModel = handLocalMatrix * enemy->visualModel;
		enemy->weaponModel = actualModel;
		actualModel.scale(1.25f, 1.25f, 1.25f);
	}

	enemy->weapon->render();
}

void renderEnemyGUI(EntityEnemy* enemy) {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
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

	Texture* heartTex = Texture::Get("data/heart_enemy.png");

	if (enemy->hearts >= 0)
	{
		for (size_t i = 0; i < enemy->hearts; i++)
		{
			//renderGUI(30 + 50 * i, 100, 100.0f, 100.0f, heartTex, true);
			if (dist < enemy->sightDistance || enemy->markedTarget)
			{
				if (playerScreen.z < 1.0f){
					if (enemy->hearts >= 4)
					{
						if (i >= 4)
						{
							renderGUI(playerScreen.x - 25 + 25 * (i % 3), game->window_height - playerScreen.y + 25, 25.0f, 25.0f, heartTex, true);
						}else{
							renderGUI(playerScreen.x - 25 + 25 * i, game->window_height - playerScreen.y, 25.0f, 25.0f, heartTex, true);
						}
					}
					else
					{
						renderGUI(playerScreen.x - 25 + 25 * i, game->window_height - playerScreen.y, 25.0f, 25.0f, heartTex, true);
					}
				}	
			}

		}
	}
	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
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
		world.camera_inverse = false;
		Vector3 eye = model * Vector3(0.0f, 4.0f, 6.0f);
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
			if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal) && (world.sPressed))
			{
				world.camera_inverse = true;
				eye = model * Vector3(4.0f, 12.0f, -8.0f);
				center = model * Vector3(0.0f, 0.0f, 0.0f);
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
		if (world.enemies[i]->weapon->mesh->testSphereCollision(world.enemies[i]->weaponModel, centerCharacter, 0.75, coll, collnorm) && player->hitTimer == 0.0f
			&& (world.enemies[i]->currentAnim == ENEMY_ANIM_ID::ENEMY_ATTACK && world.enemies[i]->animTimer >= world.enemies[i]->hitRegion) && world.level_info.level != 0) {
			//PlayGameSound("data/hit.wav");
			player->hitTimer = player->animations[player->currentAnim]->duration;
			player->hearts -= world.enemies[i]->strength;
			if (player->hearts <= 0)
			{
				player->currentAnim = PLAYER_ANIM_ID::PLAYER_DEAD;
				player->animDuration = player->animations[player->currentAnim]->duration / 2.65f;
				player->animTimer = player->animations[player->currentAnim]->duration / 2.65f;
			}
			else {
				player->currentAnim = PLAYER_ANIM_ID::PLAYER_HIT;
				player->animDuration = player->animations[player->currentAnim]->duration;
				player->animTimer = player->animations[player->currentAnim]->duration;
			}

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
	for (size_t i = 0; i < world.enemies.size(); i++)
	{
		Vector3 posEnt = world.enemies[i]->model.getTranslation();

		if (world.enemies[i]->mesh->mesh->testSphereCollision(world.enemies[i]->model, centerCharacter, 0.25, coll, collnorm))
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

void bulletCollision(float seconds_elapsed) {
	Vector3 coll;
	Vector3 collnorm;
	Vector3 pushAway;
	Vector3 returned;

	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	std::vector<EntityEnemy*>& s_enemies = Game::instance->world.enemies;

	for (size_t i = 0; i < world.numBullets; i++)
	{
		sBullet& currentBullet = world.bullets[i];
		if (currentBullet.isActive())
			continue;

		//first check if we have build the collision model
		if (!player->mesh->mesh->collision_model)
			player->mesh->mesh->createCollisionModel();

		CollisionModel3D* collision_model = (CollisionModel3D*)player->mesh->mesh->collision_model;
		Vector3 currentPos = currentBullet.model.getTranslation();
		Vector3 next = currentPos + (currentBullet.velocity * seconds_elapsed);

		currentBullet.last_position = currentPos;
		currentBullet.model.setTranslation(next.x, next.y, next.z);
		currentBullet.ttl -= seconds_elapsed;
		
		//std::cout << "TTL: " << currentBullet.ttl << std::endl;

		bool test = currentBullet.mesh->testSphereCollision(currentBullet.model, player->pos, 1.0f, coll, collnorm);
		if (test == true){
			currentBullet.ttl = -1.0f;
			player->hearts -= world.enemies[i]->strength;
		}
		else {
			for (size_t i = 0; i < world.collidable_entities.size(); i++)
			{
				bool test_other_entities = world.collidable_entities[i]->mesh->testSphereCollision(world.collidable_entities[i]->model, next, 0.25, coll, collnorm);
				if (test_other_entities == true) {
					currentBullet.ttl = -1.0f;
					break;
				}
			}
		}
		
	}
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
		
		//if (chest->mesh->testRayCollision(chest->model, rayOrigin, dir, pos, normal) && (playerPos.distance(entityPos) <= 3.0f))
		if (playerPos.distance(entityPos) <= 3.0f)
		{
			std::cout << "Object selected: " << std::endl;
			world.level_info.last_chest = world.chests[i]->type;
			if (world.chests[i]->type == CHEST_ID::CHEST_SWORD)
			{
				player->inventory.push_back(content);
				player->currentItem = ITEM_ID::SWORD;
			}
			else if (world.chests[i]->type == CHEST_ID::CHEST_HEART) {
				player->hearts += 1;
			}
			else if (world.chests[i]->type == CHEST_ID::CHEST_STRENGTH) {
				player->strength += 1;
			}
			else {
				player->runSpeed += 5.0f;
			}
			
			for (size_t j = 0; j < world.collidable_entities.size(); j++)
			{
				if (world.collidable_entities[j]->chest_id == chests_address[i]->collidable_id)
				{
					world.collidable_entities.erase(world.collidable_entities.begin() + j);
				}
			}
			chests_address.erase(world.chests.begin() + i);
			world.level_info.tag = ACTION_ID::OPEN_CHEST;
			world.currentStage = STAGE_ID::TRANSITION;

			break;
		}
	}
}

void checkIfFinish(Camera* cam) {
	Game* game = Game::instance;
	World& world = game->world;
	EntityPlayer* player = world.player;
	std::vector<EntityEnemy*>& s_enemies = Game::instance->world.enemies;

	Vector2 mouse = Input::mouse_position;

	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, game->window_width, game->window_height);
	Vector3 rayOrigin = cam->eye;

	Vector3 playerPos = player->visualModel.getTranslation();

	EntityMesh* entity = Game::instance->world.finish;
	Vector3 pos;
	Vector3 normal;

	Vector3 entityPos = entity->model.getTranslation();

	//std::cout << "Player distance to object: " << playerPos.distance(entityPos) << std::endl;

	//if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal) && (playerPos.distance(entityPos) <= 3.0f) && s_enemies.size() == 0)
	if (playerPos.distance(entityPos) <= 3.0f && (s_enemies.size() == 0 || world.level_info.level == 0))
	{
		world.level_info.tag = ACTION_ID::WIN;
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

	if (player->hearts >= 0)
	{
		for (size_t i = 0; i < player->hearts; i++)
		{
			renderGUI(30 + 50 * i, 100, 50.0f, 50.0f, heartTex, true);
		}
	}

	RenderMinimap(500);

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

		
		if (enemy->type == ENEMY_ID::WARRIOR)
		{
			if (forwardDot < 0.98f)
			{
				enemy->jaw += sign(sideDot) * 90.0f * seconds_elapsed;
			}
			if ((dist < enemy->sightDistance || enemy->markedTarget) && enemy->hitTimer <= 0.0f && enemy->hearts > 0)
			{
				std::cout << "anim timer: " << enemy->animTimer << std::endl;
				if (enemy->currentAnim != ENEMY_ANIM_ID::ENEMY_ATTACK || (enemy->currentAnim == ENEMY_ANIM_ID::ENEMY_ATTACK && enemy->animTimer <= 0.0f))
				{
					enemy->currentAnim = ENEMY_ANIM_ID::ENEMY_ATTACK;
					enemy->time = 0.0f;
					enemy->animDuration = enemy->animations[enemy->currentAnim]->duration ;
					enemy->animTimer = enemy->animations[enemy->currentAnim]->duration;
				}
				enemy->markedTarget = true;
				if (dist > 1.0f) {
					Vector3 targetPos = enemy->pos + (forward * enemy->velocity * seconds_elapsed);
					enemy->pos = enemyCollision(enemy, targetPos);
				}
			}
		}
		else if (enemy->type == ENEMY_ID::ARCHER)
		{
			if (forwardDot < 0.999f)
			{
				enemy->jaw += sign(sideDot) * 90.0f * seconds_elapsed;
			}
			if ((dist < enemy->sightDistance || enemy->markedTarget) && enemy->hitTimer <= 0.0f && enemy->hearts > 0)
			{
				enemy->markedTarget = true;

				//std::cout << "anim timer: " << enemy->animTimer << std::endl;
				if (enemy->currentAnim != ENEMY_ANIM_ID::ENEMY_ATTACK || (enemy->currentAnim == ENEMY_ANIM_ID::ENEMY_ATTACK && enemy->animTimer <= 0.0f))
				{
					enemy->currentAnim = ENEMY_ANIM_ID::ENEMY_ATTACK;
					enemy->time = 0.0f;
					enemy->animDuration = enemy->animations[enemy->currentAnim]->duration / 0.005f;
					enemy->animTimer = enemy->animations[enemy->currentAnim]->duration / 0.005f;
				}
				
				if (enemy->shootTimer <= 0.0f && enemy->type == ENEMY_ID::ARCHER)
				{
					enemy->shootTimer = enemy->attackSpeed;
					world.bulletOnce = true;
					sBullet bullet;
					bullet.ttl = 5.0f;
					bullet.mesh = Mesh::Get("data/arrow.obj");
					bullet.texture = Texture::Get("data/color-atlas.png");
					bullet.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
					bullet.power = 1;
					//model.rotate(jaw * DEG2RAD, Vector3(0, 1, 0));
					bullet.author = i;

					bullet.velocity = Vector3(10, 10, 10) * forward;

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
		}

		Vector3 coll;
		Vector3 collnorm;
		Vector3 pushAway;
		Vector3 returned;

		Vector3 centerCharacter = enemy->pos + Vector3(0.0f, 1.0f, 0.0f);

		if (player->currentItem != ITEM_ID::NONE)
		{
			if (player->inventory[player->currentItem]->mesh->testSphereCollision(player->swordModel, centerCharacter, 0.75, coll, collnorm) && enemy->hitTimer == 0.0f 
				&& (player->currentAnim == PLAYER_ANIM_ID::PLAYER_ATTACK && player->animTimer >= 0.55f)) {
				//PlayGameSound("data/hit.wav");
				enemy->time = 0.0f;
				enemy->hitTimer = player->animations[player->currentAnim]->duration / 2;
				enemy->hearts -= player->strength;
				enemy->currentAnim = ENEMY_ANIM_ID::ENEMY_HIT;
				enemy->animDuration = enemy->animations[enemy->currentAnim]->duration / 2;
				enemy->animTimer = enemy->animations[enemy->currentAnim]->duration / 2;
				if (enemy->hearts <= 0)
				{
					enemy->time = 0.0f;
					enemy->currentAnim = ENEMY_ANIM_ID::ENEMY_DEAD;
					enemy->animDuration = enemy->animations[enemy->currentAnim]->duration / 2;
					enemy->animTimer = enemy->animations[enemy->currentAnim]->duration / 2;

				}
			}
		}

	}
}

float sign(float value) {
	return value >= 0.0f ? 1.0f : -1.0f;
}

void PlayGameSound(const char* fileName) {

	HSAMPLE hSample = loadSample(fileName);

	HCHANNEL hSampleChannel;
	hSampleChannel = BASS_SampleGetChannel(hSample, false);

	BASS_ChannelPlay(hSampleChannel, true);
}
