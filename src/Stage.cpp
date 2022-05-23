#include "Stage.h"
#include "game.h"
#include "utils.h"
#include "fbo.h"
#include <cmath>
#include "animation.h"
#include "input.h"
#include "Gamemap.h"

void playStage::render()
{
	Game* game = Game::instance;
	World world = game->world;
	EntityPlayer* player = world.player;
	// loadMesh();

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



	world.sky->model = Matrix44();
	world.sky->mesh = Mesh::Get("data/cielo.ASE");
	world.sky->texture = Texture::Get("data/cielo.tga");
	world.sky->model.translate(game->camera->eye.x, game->camera->eye.y - 10.0f, game->camera->eye.z);
	glDisable(GL_DEPTH_TEST);
	renderMesh(GL_TRIANGLES, world.sky->model, world.sky->mesh, world.sky->texture, world.sky->shader, game->camera);
	//world.sky->render();
	glEnable(GL_DEPTH_TEST);

	player->model = Matrix44();
	player->model.translate(player->mov.pos.x, player->mov.pos.y, player->mov.pos.z);
	player->model.rotate(player->mov.jaw * DEG2RAD, Vector3(0, 1, 0));

	setCamera(game->camera, player->model);

	world.ground->model = Matrix44();
	world.ground->render();
	
	player->render();

	for (size_t i = 0; i < world.static_entities.size(); i++)
	{
		EntityMesh* entity = world.static_entities[i];
		entity->render();
	}
	// m.rotate(game->angle * DEG2RAD, Vector3(0, 1, 0));

	//RenderMesh(game->world.shader, game->world.skybox, game->world.skymodel, game->camera, game->world.tex);
	//RenderMesh(game->world.shader, game->world.ground_mesh, game->world.groundModel, game->camera, game->world.ground_text);
	//RenderMesh(game->world.shader, game->world.mainCharacter, game->world.model, game->camera, game->world.texCharacter);
	
	// RenderMesh(game->shader, game->escenaMesh, game->escenaModel, game->camera,game->escenaText);

	// RenderMesh(game->shader, game->static_entities[0]->mesh, game->static_entities[0]->model, game->camera, game->static_entities[0]->texture);

	//player->mesh->mesh->renderBounding(game->world.model);

	//Draw the floor grid
	drawGrid();

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

	float speed = seconds_elapsed * 10; // the speed is defined by the seconds_elapsed so it goes constant

	//example
	world.angle += (float)seconds_elapsed * 10.0f;

	// mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || game->mouse_locked) // is left button pressed?
	{
		game->camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		game->camera->rotate(Input::mouse_delta.y * 0.005f, game->camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		world.cameraLocked = !world.cameraLocked;
	}
	if (!world.cameraLocked)
	{
		SDL_ShowCursor(true);
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W)) game->camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S)) game->camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A)) game->camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D)) game->camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_UP)) game->camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) game->camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
	}
	else
	{

		float playerSpeed = 20.0f * seconds_elapsed;
		float rotateSpeed = 120.0f * seconds_elapsed;

		if (player->firstPerson)
		{
			player->mov.pitch -= Input::mouse_delta.y * 5.0f * seconds_elapsed;
			player->mov.jaw -= Input::mouse_delta.x * 5.0f * seconds_elapsed;
			Input::centerMouse();
			SDL_ShowCursor(false);
		}
		else{
			if (Input::isKeyPressed(SDL_SCANCODE_D)) player->mov.jaw += rotateSpeed;
			if (Input::isKeyPressed(SDL_SCANCODE_A)) player->mov.jaw -= rotateSpeed;
		}
		Matrix44 playerRotation;
		playerRotation.rotate(player->mov.jaw * DEG2RAD, Vector3(0, 1, 0));
		Vector3 forward = playerRotation.rotateVector(Vector3(0, 0, -1));
		Vector3 right = playerRotation.rotateVector(Vector3(1, 0, 0));
		Vector3 playerVel;

		if (Input::isKeyPressed(SDL_SCANCODE_W)) playerVel = playerVel + (forward * playerSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_S)) playerVel = playerVel - (forward * playerSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) playerVel = playerVel - (right * playerSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_E)) playerVel = playerVel + (right * playerSpeed);

		Vector3 targetPos = player->mov.pos + playerVel;
		if ((targetPos.x <= 183.0 && targetPos.x >= 3.0 && targetPos.z <= 183.0 && targetPos.z >= 3.0))
		{
			player->mov.pos = checkCollision(targetPos);
		}
	}

	// async input to move the camera around

	/*
	for (size_t i = 0; i < game->dynamic_entities.size(); i++) {
		checkCollision(dynamic_entities[i],game->static_entities);
		checkCollision(dynamic_entities[i], game->dynamic_entities);
	}*/

	//checkCollision();

	if (Input::isKeyPressed(SDL_SCANCODE_C))
		world.cameraLocked = !world.cameraLocked;
	// to navigate with the mouse fixed in the middle
	if (game->mouse_locked)
		Input::centerMouse();
}

void renderMesh(int primitive, Matrix44& model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam, float tiling) {
	Game* game = Game::instance;
	assert(a_mesh != NULL, "mesh in renderMesh was null");
	if (!a_shader) return;

	//enable shader
	a_shader->enable();

	//upload uniforms
	a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
	if (tex != NULL)
	{
		a_shader->setUniform("u_texture", tex, 0);
	}
	a_shader->setUniform("u_time", time);
	a_shader->setUniform("u_text_tiling", tiling);
	a_shader->setUniform("u_model", model);
	//do the draw call
	a_mesh->render(primitive);

	//disable shader
	a_shader->disable();

	if (!game->world.cameraLocked) {
		a_mesh->renderBounding(model);
	}
}

void setCamera(Camera *cam, Matrix44 model)
{
	Game* game = Game::instance;
	World world = game->world;
	EntityPlayer* player = world.player;

	if (world.cameraLocked)
	{
		Vector3 eye = model * Vector3(0.0f, 3.0f, 4.0f);
		Vector3 center = model * Vector3(0.0f, 0.0f, -10.0f);
		Vector3 up = model.rotateVector(Vector3(0.0f, 1.0f, 0.0f));

		if (player->firstPerson) {
			Matrix44 camModel = player->model;
			camModel.rotate(player->mov.pitch * DEG2RAD, Vector3(1, 0, 0));

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

	Vector3 centerCharacter = target + Vector3(0.0, 1.0, 0.0);

	// eCellType type = game->map->getCell(x-6, y-6).type;

	for (size_t i = 0; i < game->world.static_entities.size(); i++)
	{

		if (game->world.static_entities[i]->mesh->testSphereCollision(game->world.static_entities[i]->model, centerCharacter, 0.5, coll, collnorm))
		{
			pushAway = normalize(coll - centerCharacter) * game->elapsed_time;

			returned = player->mov.pos - pushAway;
			returned.y = player->mov.pos.y;
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
	model.translate(spawnPos.x, spawnPos.y, spawnPos.z);
	model.scale(3.0f, 3.0f, 3.0f);
	EntityMesh* entity = new EntityMesh(GL_TRIANGLES, model, Mesh::Get(meshName), Texture::Get(textName), Game::instance->world.shader);
	Game::instance->world.static_entities.push_back(entity);
}

void rayPick(Camera* cam) {

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
			std::cout << "col" << std::endl;
			//points.push_back(pos);
			break;
		}
	}
}


void rotateSelected(float angleDegrees) {
	/*
	if (selectedEntity == NULL) {
		return;
	}

	selectedEntity->model.rotate(angleDegrees * DEG2RAD, Vector3(0, 1, 0));
	*/
}
