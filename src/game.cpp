#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"

#include <cmath>

//some globals
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;

bool cameraLocked = true;
Mesh* planeMesh = NULL;
Texture* planeTexture = NULL;
Matrix44 planeModel;

Mesh* bombMesh = NULL;
Texture* bombTexture = NULL;
Matrix44 bombModel;
Matrix44 bombOffset;
bool bombAttached = true;

Animation* anim = NULL;
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;

Game* Game::instance = NULL;

const int planes_width = 200;
const int planes_height = 200;
float padding = 20.0f;

float lod_distance = 200.0f;
float no_render_distance = 1000.0f;

class Prop{
public:
	int id;
	Mesh* mesh;
	Texture* texture;
};

Prop props[20];

class Base
{
public:
	int a;
	virtual void Foo() {};
};

class Child : public Base{
public:
	void Foo() override {};
};


class Entity {
public:

	//some attributes 
	Matrix44 model;
	Mesh* mesh;
	Texture* texture;

};

struct sPlayer {
	Vector3 pos;
	float jaw;
	float pitch;
};

sPlayer player;

std::vector<Entity*> entities;
Entity* selectedEntity = NULL;

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

	std::cout << sizeof(Base) << std::endl;
	std::cout << sizeof(Child) << std::endl;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	bombOffset.setTranslation(0.0f, -2.0f, 0.0f);

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

	planeMesh = Mesh::Get("data/spitfire.ASE");
	planeTexture = Texture::Get("data/spitfire_color_spec.tga");

	bombMesh = Mesh::Get("data/torpedo.ASE");
	bombTexture = Texture::Get("data/torpedo.tga");
	
	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

void renderMesh(int primitive, Matrix44& model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam, float tiling = 1.0f) {

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

	if (!cameraLocked) {
		a_mesh->renderBounding(model);
	}
}

void addEntityInFront(Camera* cam, const char* meshName, const char* textName) {

	Vector2 mouse = Input::mouse_position;

	Game* game = Game::instance;

	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y,game->window_width, game->window_height);
	Vector3 rayOrigin = cam->eye;
	

	Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
	Matrix44 model;
	model.translate(spawnPos.x, spawnPos.y, spawnPos.z);
	model.scale(3.0f, 3.0f, 3.0f);
	Entity* entity = new Entity();
	entity->model = model;
	entity->mesh = Mesh::Get(meshName);
	entity->texture = Texture::Get(textName);
	entities.push_back(entity);
}

void rayPick(Camera* cam) {

	Vector2 mouse = Input::mouse_position;

	Game* game = Game::instance;

	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, game->window_width, game->window_height);
	Vector3 rayOrigin = cam->eye;

	for (size_t i = 0; i < entities.size(); i++)
	{
		Entity* entity = entities[i];
		Vector3 pos;
		Vector3 normal;
		if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal))
		{
			std::cout << "col" << std::endl;
			selectedEntity = entity;
			//points.push_back(pos);
			break;
		}
	}
}


void rotateSelected(float angleDegrees) {
	if (selectedEntity == NULL) {
		return;
	}

	selectedEntity->model.rotate(angleDegrees * DEG2RAD, Vector3(0,1,0));
}

//what to do when the image has to be draw
void Game::render(void)
{
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
		Entity* entity = entities[i];
		renderMesh(GL_TRIANGLES, entity->model, entity->mesh, entity->texture, shader, camera);
	}

	//Draw the floor grid
	drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
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
			player.pitch -= Input::mouse_delta.y * 10.0f * elapsed_time;
			player.jaw -= Input::mouse_delta.x * 10.0f * elapsed_time;
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
			Entity* currentEntity = entities[i];

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
		*/
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
	
	if (Input::isKeyPressed(SDL_SCANCODE_F)) {
		bombAttached = !bombAttached;
	}

	if (bombAttached)
	{
		bombModel = bombOffset * planeModel;
	}
	else {
		bombModel.translateGlobal(0.0f, -9.8f * elapsed_time, 0.0f);
	}

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
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

