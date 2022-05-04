#include "Chunk.h"
Chunk::Chunk() {
	Xcoord = 0;
	Ycoord = 0;
	Chunk::tileList = new Tile[256];
	for (int tiley = 0; tiley < 16; tiley++) {
		for (int tilex = 0; tilex < 16; tilex++) {
			Chunk::tileList[tilex + tiley * 16].x = tilex;
			Chunk::tileList[tilex + tiley * 16].y = tiley;
			Chunk::tileList[tilex + tiley * 16].Tile = 1;
		}
	}
}
Chunk::Chunk(int layer,int x, int y, int seed){
	Xcoord = x;
	Ycoord = y;
	srand(seed);
	Chunk::tileList = new Tile[256];
	for (int tiley = 0; tiley < 16; tiley++) {
		for (int tilex = 0; tilex < 16; tilex++) {
			Chunk::tileList[tilex + tiley * 16].x = tilex;
			Chunk::tileList[tilex + tiley * 16].y = tiley;
			Chunk::tileList[tilex + tiley * 16].Tile = rand() % 2;
		
		}
	}
}

Chunk::~Chunk(){
}


int Chunk::drawChunk(bufferMem* buffer){
	BMPIN* grass = new BMPIN();
	grass->Import("grass.bmp");
	BMPIN* stone = new BMPIN();
	stone->Import("Stone.bmp");
	for (int tile = 0; tile < 256; tile++) {
		int x = tileList[tile].x*32 + Xcoord;
		int y = tileList[tile].y*32 + Ycoord;
		if (tileList[tile].Tile == 1) {
			drawImage(x , y, 1, grass, buffer, (unsigned int*)buffer->background);
		}
		else {
			drawImage(x, y, 1, stone, buffer, (unsigned int*)buffer->background);
		}
	}



	return 0;
}
