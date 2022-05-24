#include "Entity.h"
#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"

void EntityMesh::render() {
	assert(mesh != NULL, "mesh in renderMesh was null");
	if (!shader) return;

	//enable shader
	shader->enable();

	//upload uniforms
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
	if (texture != NULL)
	{
		shader->setUniform("u_texture", texture, 0);
	}
	shader->setUniform("u_time", time);
	shader->setUniform("u_text_tiling", tiling);
	shader->setUniform("u_model", model);
	//do the draw call
	mesh->render(primitive);

	//disable shader
	shader->disable();

	if (!Game::instance->cameraLocked) {
		mesh->renderBounding(model);
	}
}

void EntityMesh::update(float dt) {
	float a = 20.0f * dt;
}

void EntityPlayer::render() {
	mesh->render();
}

void EntityPlayer::update(float dt) {
	mesh->update(dt);
}

void EntityPlayer::checkIsGrounded() {
	if (mov.pos.y == 0)
	{
		isGrounded = true;
	}
	else if (mov.pos.y > 0) {
		isGrounded = false;
	}
}