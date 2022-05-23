#include "World.h"
#include "game.h"

void World::loadWorld() {
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	player = new EntityPlayer();
	play = new playStage();
	current_stage = play;

	Matrix44 skyModel;
	sky = new EntityMesh(GL_TRIANGLES, skyModel, Mesh::Get("data/cielo.ASE"), Texture::Get("data/cielo.tga"), shader);

	Matrix44 groundModel;
	Mesh* groundMesh = new Mesh();
	groundMesh->createPlane(1000);
	ground = new EntityMesh(GL_TRIANGLES, groundModel, groundMesh, Texture::Get("data/ground.jpg"), shader);
	ground->tiling = 500.0f;
}
