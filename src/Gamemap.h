#ifndef GAMEMAP_H
#define GAMEMAP_H
#include "utils.h"
#include <cmath>
#include "mesh.h"
#include "texture.h"

struct sPropViewData {
	Mesh* mesh;
	Texture* texture;
};

enum eCellType : uint8 {
	SEA, 
	FIELD,
	WALL,
	BOX,
};

struct sCell {
	eCellType type;
};

struct sMapHeader {
	int w; //width of map
	int h; //height of map
	unsigned char bytes; //num bytes per cell
	unsigned char extra[7]; //filling bytes, not used
};

class GameMap {
public:
	int width;
	int height;
	sCell* data;

	sPropViewData viewData[4];

	GameMap();
	GameMap(int w, int h);
	sCell& getCell(int x, int y);
	GameMap* loadGameMap(const char* filename);
	void setViewData();
};

#endif