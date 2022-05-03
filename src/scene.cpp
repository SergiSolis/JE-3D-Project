#include "scene.h"
#include <cmath>

void EntityMesh::render()
{
	//get the last camera that was activated
	Camera* camera = Camera::current;
	Matrix44 model = this->model;

	//enable shader and pass uniforms
	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	//shader->setTexture("u_texture", texture);

	//render the mesh using the shader
	
	//mesh->render(GL_TRIANGLES, shader);

	//disable the shader after finishing rendering
	shader->disable();
}
