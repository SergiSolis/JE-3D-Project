#pragma once

#include "Entity.h"
#include "utils.h"


class World {
public:

	World();
	std::vector<Entity*> entities;
	void setInit();
	void setConfiguration();
	void saveGame();

};