#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"

#include <cmath>

#include "World.h"
#include "Stage.h"

//some globals
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;

bool cameraLocked = true;

Animation* anim = NULL;
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;

Game* Game::instance = NULL;


sPlayer player;

std::vector<EntityMesh*> entities;
EntityMesh* selectedEntity = NULL;

bool firstPerson = true;

Mesh* groundMesh;
Texture* groundTex;

//std::vector<Vector3> points;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	world.loadWorld();

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,50.f, 100.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,100000.f); //set the projection, we want to be perspective

	groundMesh = new Mesh();
	groundMesh->createPlane(1000);
	groundTex = Texture::Get("data/ground.jpg");

	//load one texture without using the Texture Manager (Texture::Get would use the manager)
	

	// example of loading Mesh from Mesh Manager
	mesh = Mesh::Get("data/island.ASE");
	texture = Texture::Get("data/island_color.tga");
	
	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void)
{
	world.current_stage->render();
	/*
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//set the camera as default
	camera->enable();

	Texture* skyTex = Texture::Get("data/cielo.tga");
	Mesh* skyMesh = Mesh::Get("data/cielo.ASE");
	Matrix44 skyModel;
	skyModel.translate(camera->eye.x, camera->eye.y -10.0f, camera->eye.z);
	glDisable(GL_DEPTH_TEST);
	renderMesh(GL_TRIANGLES, skyModel, skyMesh, skyTex, shader, camera);
	glEnable(GL_DEPTH_TEST);

	Matrix44 playerModel;
	playerModel.translate(player.pos.x, player.pos.y, player.pos.z);
	playerModel.rotate(player.jaw * DEG2RAD, Vector3(0, 1, 0));

	if (cameraLocked)
	{
		Vector3 eye = playerModel * Vector3(0.0f, 3.0f, 4.0f);
		Vector3 center = playerModel * Vector3(0.0f, 0.0f, -10.0f);
		Vector3 up = playerModel.rotateVector(Vector3(0.0f, 1.0f, 0.0f));

		if (firstPerson) {
			Matrix44 camModel = playerModel;
			camModel.rotate(player.pitch * DEG2RAD, Vector3(1, 0, 0));

			eye = playerModel * Vector3(0.0f, 1.0f, -0.5f);
			center = eye + camModel.rotateVector(Vector3(0.0f, 0.0f, -1.0f));
			up = camModel.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
		}

		camera->lookAt(eye, center, up);
	}
	

	renderMesh(GL_TRIANGLES, Matrix44(), groundMesh, groundTex, shader, camera, 500.0f);

	Texture* playerTex = Texture::Get("data/PolygonMinis_Texture_01_A.png");
	Mesh* playerMesh = Mesh::Get("data/skeleton_back.obj");
	
	renderMesh(GL_TRIANGLES, playerModel, playerMesh, playerTex, shader, camera);

	for (size_t i = 0; i < entities.size(); i++)
	{
		EntityMesh* entity = entities[i];
		renderMesh(GL_TRIANGLES, entity->model, entity->mesh, entity->texture, shader, camera);
	}

	//Draw the floor grid
	drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
	*/
}

void Game::update(double seconds_elapsed)
{
	world.current_stage->update(seconds_elapsed);
	/*
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)seconds_elapsed * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		cameraLocked = !cameraLocked;
	}

	//async input to move the camera around
	if (cameraLocked)
	{
		float playerSpeed = 20.0f * elapsed_time;
		float rotateSpeed = 120.0f * elapsed_time;


		
		if (firstPerson)
		{
			player.pitch -= Input::mouse_delta.y * 5.0f * elapsed_time;
			player.jaw -= Input::mouse_delta.x * 5.0f * elapsed_time;
			Input::centerMouse();
			SDL_ShowCursor(false);
		}
		else {
			if (Input::isKeyPressed(SDL_SCANCODE_D)) player.jaw += rotateSpeed;
			if (Input::isKeyPressed(SDL_SCANCODE_A)) player.jaw -= rotateSpeed;
		}

		Matrix44 playerRotation;
		playerRotation.rotate(player.jaw * DEG2RAD, Vector3(0, 1, 0));
		Vector3 forward = playerRotation.rotateVector(Vector3(0, 0, -1));
		Vector3 right = playerRotation.rotateVector(Vector3(1, 0, 0));
		Vector3 playerVel;

		if (Input::isKeyPressed(SDL_SCANCODE_W)) playerVel = playerVel + (forward* playerSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_S)) playerVel = playerVel - (forward * playerSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) playerVel = playerVel - (right * playerSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_E)) playerVel = playerVel + (right * playerSpeed);



		Vector3 nextPos = player.pos + playerVel;

		//calculamos el centro de la esfera de colisión del player elevandola hasta la cintura
		Vector3 character_center = nextPos + Vector3(0, 1, 0);

		for (size_t i = 0; i < entities.size(); i++)
		{
			EntityMesh* currentEntity = entities[i];

			Vector3 coll;
			Vector3 collnorm;

			//comprobamos si colisiona el objeto con la esfera (radio 3)
			if (!currentEntity->mesh->testSphereCollision(currentEntity->model, character_center, 0.5f, coll, collnorm))
				continue; //si no colisiona, pasamos al siguiente objeto

			//si la esfera está colisionando muevela a su posicion anterior alejandola del objeto
			Vector3 push_away = normalize(coll - character_center) * elapsed_time;
			nextPos = player.pos - push_away; //move to previous pos but a little bit further

			//cuidado con la Y, si nuestro juego es 2D la ponemos a 0
			nextPos.y = 0;

			//reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
			//velocity = reflect(velocity, collnorm) * 0.95;
		}

		
		player.pos = nextPos;


		
		/*
		if (Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 10.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, -10.0f, 0.0f) * speed);

	}
	else {
		SDL_ShowCursor(true);
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
	}
	
	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
	*/
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break; 
			//#ifdef EDITOR
		case SDLK_2: addEntityInFront(camera,"data/building-antique-china.obj" , "data/color-atlas.png"); break;
		case SDLK_3: rayPick(camera); break;
		case SDLK_KP_PLUS: rotateSelected(10.0f); break;
		case SDLK_KP_MINUS: rotateSelected(-10.0f); break;
		case SDLK_0: firstPerson = !firstPerson; break;
			//#endif // EDITOR

	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1 : 0.9;
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

