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

	bombOffset.setTranslation(0.0f, -2.0f, 0.0f);

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,50.f, 100.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,100000.f); //set the projection, we want to be perspective

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

void renderMesh(Matrix44& model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam) {

	assert(a_mesh != NULL, "mesh in renderMesh was null");
	if (!a_shader) return;

	//enable shader
	a_shader->enable();

	//upload uniforms
	a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
	a_shader->setUniform("u_texture", tex, 0);
	a_shader->setUniform("u_time", time);

	a_shader->setUniform("u_model", model);
	//do the draw call
	a_mesh->render(GL_TRIANGLES);

	//disable shader
	a_shader->disable();

}

void renderIslands() {
	if (shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_time", time);

		Matrix44 m;
		for (size_t i = 0; i < 10; i++)
		{
			for (size_t j = 0; j < 10; j++)
			{
				//create model matrix for island
				Vector3 size = mesh->box.halfsize * 2;
				m.setTranslation(size.x * i, 0.0f, size.z * j);
				shader->setUniform("u_model", m);
				//do the draw call
				mesh->render(GL_TRIANGLES);
			}
		}

		//disable shader
		shader->disable();
	}
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

	if (cameraLocked)
	{
		Vector3 eye = planeModel * Vector3(0.0f, 9.0f, 16.0f);
		Vector3 center = planeModel * Vector3(0.0f, 0.0f, -20.0f);
		Vector3 up = planeModel.rotateVector(Vector3(0.0f, 1.0f, 0.0f));

		camera->lookAt(eye, center, up);
	
	}

	Matrix44 islandModel;
	renderMesh(islandModel, mesh, texture, shader, camera);
	renderMesh(planeModel, planeMesh, planeTexture, shader, camera);
	renderMesh(bombModel, bombMesh, bombTexture, shader, camera);
	
	//renderIslands();

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
		float planeSpeed = 30.0f * elapsed_time;
		float rotateSpeed = 90.0f * DEG2RAD * elapsed_time;
		if (Input::isKeyPressed(SDL_SCANCODE_W)) planeModel.translate(0.0f, 0.0f, -planeSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_S)) planeModel.translate(0.0f, 0.0f, planeSpeed);

		if (Input::isKeyPressed(SDL_SCANCODE_A)) planeModel.rotate(-rotateSpeed, Vector3(0.0f, 1.0f, 0.0f));
		if (Input::isKeyPressed(SDL_SCANCODE_D)) planeModel.rotate(rotateSpeed, Vector3(0.0f, 1.0f, 0.0f));
		if (Input::isKeyPressed(SDL_SCANCODE_E)) planeModel.rotate(rotateSpeed, Vector3(0.0f, 0.0f, 1.0f));
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) planeModel.rotate(-rotateSpeed, Vector3(0.0f, 0.0f, 1.0f));
		/*
		if (Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 10.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, -10.0f, 0.0f) * speed);
		*/
	}
	else {
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

