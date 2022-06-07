#include "Entity.h"
#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "framework.h"
#include "animation.h"

#include <cmath>

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



void EntityPlayer::update(float dt) {
	mesh->update(dt);
}

void EntityPlayer::checkIsGrounded() {
	if (pos.y == 0)
	{
		isGrounded = true;
	}
	else if (pos.y > 0) {
		isGrounded = false;
	}
}

void EntityPlayer::render() {
	Game* game = Game::instance;
	World world = game->world;

	model = Matrix44();
	model.translate(pos.x, pos.y, pos.z);
	model.rotate(jaw * DEG2RAD, Vector3(0, 1, 0));
	visualModel = model;

	visualModel.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
	visualModel.scale(0.01f, 0.01f, 0.01f);

	assert(a_mesh != NULL, "mesh in renderMesh was null");
	if (!mesh->shader) return;

	//enable shader
	mesh->shader->enable();

	float t = fmod(Game::instance->time, animations[currentAnim]->duration) / animations[currentAnim]->duration;
	animations[currentAnim]->assignTime(t * animations[currentAnim]->duration);
	//player->run->assignTime(t * player->run->duration);

	resultSk = animations[currentAnim]->skeleton;

	//blendSkeleton(&player->walk->skeleton, &player->run->skeleton, 0.5f, &player->resultSk);

	//Matrix44& neckLocalMatrix = player->resultSk.getBoneMatrix("mixamorig_Neck");
	//neckLocalMatrix.rotate(180.0f * sin(Game::instance->time) * DEG2RAD, Vector3(0, 1, 0));

	Matrix44& headLocalMatrix = resultSk.getBoneMatrix("mixamorig_Head");
	headLocalMatrix.scale(2.0f, 2.0f, 2.0f);

	//Matrix44& RightForeArmLocalMatrix = resultSk.getBoneMatrix("mixamorig_RightForeArm");
	//RightForeArmLocalMatrix.scale(0.0f, 0.0f, 0.0f);

	//upload uniforms
	mesh->shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	mesh->shader->setUniform("u_viewprojection", game->camera->viewprojection_matrix);
	if (mesh->texture != NULL)
	{
		mesh->shader->setUniform("u_texture", mesh->texture, 0);
	}
	mesh->shader->setUniform("u_time", time);
	mesh->shader->setUniform("u_text_tiling", 1.0f);
	mesh->shader->setUniform("u_model", visualModel);




	//do the draw call
	mesh->mesh->renderAnimated(mesh->primitive, &resultSk);

	//disable shader
	mesh->shader->disable();

	if (!game->world.cameraLocked) {
		mesh->mesh->renderBounding(visualModel);
	}
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

void renderMeshAnim(int primitive, Matrix44& model, Mesh* a_mesh, Animation* anim, Texture* tex, Shader* a_shader, Camera* cam, float tiling) {
	Game* game = Game::instance;
	World world = game->world;
	EntityPlayer* player = world.player;
	assert(a_mesh != NULL, "mesh in renderMesh was null");
	if (!a_shader) return;

	//enable shader
	a_shader->enable();

	float t = fmod(Game::instance->time, anim->duration) / anim->duration;
	anim->assignTime(t * anim->duration);
	//player->run->assignTime(t * player->run->duration);

	player->resultSk = player->animations[player->currentAnim]->skeleton;
	
	//blendSkeleton(&player->walk->skeleton, &player->run->skeleton, 0.5f, &player->resultSk);

	//Matrix44& neckLocalMatrix = player->resultSk.getBoneMatrix("mixamorig_Neck");
	//neckLocalMatrix.rotate(180.0f * sin(Game::instance->time) * DEG2RAD, Vector3(0, 1, 0));

	Matrix44& headLocalMatrix = player->resultSk.getBoneMatrix("mixamorig_Head");
	headLocalMatrix.scale(2.0f, 2.0f, 2.0f);

	//Matrix44& RightForeArmLocalMatrix = resultSk.getBoneMatrix("mixamorig_RightForeArm");
	//RightForeArmLocalMatrix.scale(0.0f, 0.0f, 0.0f);

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
	a_mesh->renderAnimated(primitive, &player->resultSk);

	//disable shader
	a_shader->disable();

	if (!game->world.cameraLocked) {
		a_mesh->renderBounding(model);
	}
}