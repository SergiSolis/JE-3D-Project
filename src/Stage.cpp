#include "Stage.h"
#include "game.h"
#include "utils.h"
#include "fbo.h"
#include <cmath>
#include "animation.h"
#include "input.h"

void playStage::render()
{
	Game* game = Game::instance;
	World world = game->world;
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
	setCamera(game->camera, player->model);
	player->render();
	
	if(player->objectSelected != NULL) {

		Mesh* mesh = Mesh::Get("data/box.obj");
		player->resultSk = player->animations[player->currentAnim]->skeleton;
		Matrix44 handLocalMatrix = player->resultSk.getBoneMatrix("mixamorig_RightHandIndex4", false);
		handLocalMatrix.scale(0.5f, 0.5f, 0.5f);
		renderMesh(GL_TRIANGLES, handLocalMatrix * player->visualModel, mesh, player->mesh->texture, player->mesh->shader, game->camera);
		//world.static_entities.erase(world.static_entities.begin() + player->objectSelected);
	}
		
	//Draw the floor grid
	//drawGrid();

	//RENDER ALL GUI
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Texture* texture = Texture::Get("data/fangs.png");
	renderGUI(100.0f, 100.0f, 100.0f, 100.0f, texture, true);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(game->window);
}

void playStage::update(float seconds_elapsed)
{

	Game *game = Game::instance;
	World world = game->world;
	EntityPlayer* player = world.player;

	//std::cout << "IS GROUNDED: " << player->isGrounded << std::endl;

	player->currentAnim = ANIM_ID::IDLE;

	float speed = seconds_elapsed * 10; // the speed is defined by the seconds_elapsed so it goes constant

	player->jumpLock = max(0.0f, player->jumpLock - (seconds_elapsed));

	//example
	world.angle += (float)seconds_elapsed * 10.0f;

	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		player->cameraLocked = !player->cameraLocked;
		std::cout << "cameraLocked: " << player->cameraLocked << std::endl;
	}
	if (player->cameraLocked)
	{
		float playerSpeed = 10.0f * seconds_elapsed;
		float rotateSpeed = 120.0f * seconds_elapsed;
		float gravity = 9.807f;
		float jumpHeight = 5.0f;
		float verticalSpeed;

		if (player->firstPerson)
		{
			player->pitch -= Input::mouse_delta.y * 5.0f * seconds_elapsed;
			player->jaw -= Input::mouse_delta.x * 5.0f * seconds_elapsed;
			Input::centerMouse();
			SDL_ShowCursor(false);
		}
		else {
			if (Input::isKeyPressed(SDL_SCANCODE_D)) player->jaw += rotateSpeed;
			if (Input::isKeyPressed(SDL_SCANCODE_A)) player->jaw -= rotateSpeed;
		}
		Matrix44 playerRotation;
		playerRotation.rotate(player->jaw * DEG2RAD, Vector3(0, 1, 0));
		Vector3 forward = playerRotation.rotateVector(Vector3(0, 0, -1));
		Vector3 right = playerRotation.rotateVector(Vector3(1, 0, 0));
		Vector3 jump = playerRotation.rotateVector(Vector3(0, 1, 0));
		Vector3 playerVel;


		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) {
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
			player->jumpLock = 0.3f;
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


		/*
		if (player->isGrounded == false)
		{
			playerVel = playerVel - (jump * gravity * seconds_elapsed);
		}
		*/

		if (Input::isKeyPressed(SDL_SCANCODE_G)) {
			takeEntity(game->camera, world.static_entities);
		}

		Vector3 targetPos = player->pos + playerVel;
		
		player->pos = checkCollision(targetPos);
		//player->pos = targetPos;

	}
	else
	{
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

}

void renderWorld() {
	Game* game = Game::instance;
	World world = game->world;
	EntityPlayer* player = world.player;

	glDisable(GL_DEPTH_TEST);
	//world.skyModel.translate(game->camera->eye.x, game->camera->eye.y - 10.0f, game->camera->eye.z);
	world.sky->render();
	glEnable(GL_DEPTH_TEST);

	world.ground->render();

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
	quadModel.translate(sin(Game::instance->time ) * 20, 0, 0);
	shader->setUniform("u_model", quadModel);
	//do the draw call
	quad.render(GL_TRIANGLES);

	//disable shader
	shader->disable();
}

void setCamera(Camera *cam, Matrix44 model)
{
	Game* game = Game::instance;
	World world = game->world;
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
	Game *game = Game::instance;
	World world = game->world;
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
		//std::cout << "POSITION ENTITY: X:" << posEnt.x << ", Y: " << posEnt.y << ", Z: " << posEnt.z << std::endl;
		if (world.static_entities[i]->mesh->testSphereCollision(world.static_entities[i]->model, centerCharacter, 0.5, coll, collnorm))
		{
			if (player->pos.y >= coll.y)
			{
				target.y = coll.y + 0.05f;
				player->isGrounded = true;
			}
			else {
				//std::cout << "COLLISION fromt" << std::endl;
				pushAway = normalize(coll - centerCharacter) * game->elapsed_time;

				returned = player->pos - pushAway;
				returned.y = player->pos.y;
				return returned;
			}
		}
		else {
			if (world.ground->mesh->testSphereCollision(world.ground->model, bottomCharacter, 0.01, coll, collnorm))
			{
				//std::cout << "EO" << std::endl;
				player->isGrounded = true;
				//target.y = coll.y;
			}
			else {
				player->isGrounded = false;
			}
		}
				
	}

	return target;
}

Vector3 checkCollisionBottom(Vector3 target)
{
	Game* game = Game::instance;
	World world = game->world;
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
	World world = game->world;

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
	World world = game->world;
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
			player->objectSelected = i;
			world.selectedEntity = entity;
			//world.static_entities.erase(world.static_entities.begin() + i);
			//world.static_entities.clear();
			s_entities.erase(Game::instance->world.static_entities.begin() + i);
			break;
		}
	}
}


void rotateSelected(float angleDegrees) {
	Game* game = Game::instance;
	World world = game->world;
	EntityMesh* selectedEntity = world.selectedEntity;

	if (selectedEntity == NULL) {
		return;
	}
	else {
		std::cout << "aaaaaa: " << std::endl;
	}
	selectedEntity->model.rotate(angleDegrees * DEG2RAD, Vector3(0, 1, 0));
}
