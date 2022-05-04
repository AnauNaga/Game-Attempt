#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED
#include "BMPIN.h";
#include "BMPOUT.h";
#include "BMPIO.h";
#include "GuiUtilites.h";
#include <cmath>;
struct Tile {
	int x;
	int y;
	short Tile;
};
class Chunk {
public:
	Chunk();
	Chunk(int layer, int x, int y, int seed);
	~Chunk();
	int drawChunk(bufferMem* buffer);

private:
	Tile* tileList;
	int Xcoord;
	int Ycoord;
};

#endif

