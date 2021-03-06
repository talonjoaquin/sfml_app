#include "stdafx.h"
#include "graph.h"

Graph::Graph(Playable*** map, int W, int H, Playable* owner) {
	rmap = map;
	mapW = W;
	mapH = H;
	host = owner;
}

int Graph::width() {
	return mapW;
}
int Graph::height() {
	return mapH;
}

void Graph::getMapCopy(Playable*** map, intpair o) {
	//fprintf(stderr, "o(%d,%d)\n", o.x, o.y);
	int size = mapW * mapH;
	for (int j = o.y - LOOK_AHEAD_DIST; j <= o.y + LOOK_AHEAD_DIST; j++) {
		for (int i = o.x - LOOK_AHEAD_DIST; i <= o.x + LOOK_AHEAD_DIST; i++) {
			int index = i + j * mapW;
			if (index < 0 || index >= size)
				continue;
			(*map)[index] = (*rmap)[index];
		}
	}
}

float Graph::costAdj(intpair start, intpair end) {
	//fprintf(stderr, "checking cost from (%d,%d) to (%d,%d)\n", start.x, start.y, end.x, end.y);
	//if ((start.x == 0 && start.y == 0) || (end.x == 0 && end.y == 0)) {
		//fprintf(stderr, "HEAR HEAR: (*rmap)[end.x + end.y * mapW] = %d and host = %d\n", (*rmap)[end.x + end.y * mapW], host);
	//}
	if (!isIn(start) || !isIn(end)) {
		return INFINITY;
	}
	if (abs(start.x - end.x) > 1 || abs(start.y - end.y) > 1) {
		//fprintf(stderr, "am i in here?\n");
		return INFINITY;
	}
	if (end.x < 0 || end.x >= MAP_WIDTH || end.y < 0 || end.y >= MAP_HEIGHT){
		return INFINITY;
	}
	//fprintf(stderr, "accessing rmap at %d\n", end.x + end.y * mapW);
	if ((*rmap)[end.x + end.y * mapW] == (Playable*)rmap::TERRAIN || ((int)(*rmap)[end.x + end.y * mapW] > RESERVED_RMAP_NUMBERS && (*rmap)[end.x + end.y * mapW] != host)) {
		//fprintf(stderr, "or am i in here?\n");
		return INFINITY;
	}
	/*if ((*rmap)[start.x + start.y * mapW] == (Playable*)3 || ((int)(*rmap)[start.x + start.y * mapW] > RESERVED_RMAP_NUMBERS && (*rmap)[start.x + start.y * mapW] != host)) {
		//fprintf(stderr, "or am i in here?\n");
		return INFINITY;
	}*/
	int distX = start.x - end.x;
	int distY = start.y - end.y;
	return sqrt(distX * distX + distY * distY);
}
bool Graph::isFree(intpair node) {
	if (!isIn(node)) {
		return false;
	}
	//fprintf(stderr, "isFree call\n");
	int len = 0;
	intpair* predArr = new intpair[8];
	pred(node, &predArr, &len);
	for (int i = 0; i < len; i++) {
		if (costAdj(predArr[i], node) < INFINITY) {
			//fprintf(stderr, "isFree end\n");
			return true;
		}
	}
	//fprintf(stderr, "isFree end\n");
	return false;
	/*if ((*rmap)[node.x + node.y * mapW] != 0 && (*rmap)[node.x + node.y * mapW] != host)
		return false;
	return true;*/
}
bool Graph::isIn(intpair node) {
	if (node.x >= 0 && node.x < mapW && node.y >= 0 && node.y < mapH)
		return true;
	return false;
}
void Graph::pred(intpair node, intpair** arr, int* outlen)
{
	//fprintf(stderr, "in pred call\n");
	//fprintf(stderr, "note that rmap is of a %dx%d array of size %d\n", mapW, mapH, mapW*mapH);
	//intpair** out = new intpair*[8];//(intpair**)malloc(sizeof(intpair*) * 8);
	
	if (isIn(node)) {
		*outlen = 0;
		//fprintf(stderr, "retrieving pred/succ for (%d,%d)\n", node.x, node.y);
		
		for (int yp = -1; yp < 2; yp++) {
			for (int xp = -1; xp < 2; xp++) {
				if (xp == 0 && yp == 0)
					continue;
				//fprintf(stderr, "checking if (%d,%d) is in\n", node.x + xp, node.y + yp);
				//fprintf(stderr, "checking (%d,%d)\n", node.x + xp, node.y + yp);
				if (isIn(intpair(node.x + xp, node.y + yp))) {
					//fprintf(stderr, "(%d,%d) is in map\n", node.x + xp, node.y + yp);
					//fprintf(stderr, "(%d,%d) is in - before rmap[%d] access\n", node.x + xp, node.y + yp, (node.x + xp) + (node.y + yp) * mapW);
					//fprintf(stderr, "rmap[%d] is %d\n", (node.x + xp) + (node.y + yp) * mapW, (*rmap)[(node.x + xp) + (node.y + yp) * mapW]);
					if (costAdj(intpair(node.x + xp, node.y + yp), node) < INFINITY) {
						//fprintf(stderr, "accessing out[%d]\n", *outlen);
						//fprintf(stderr, "adding in (%d,%d)\n", node.x + xp, node.y + yp);
						(*arr)[*outlen] = intpair(node.x + xp, node.y + yp);
						*outlen = *outlen + 1;
					}
					//fprintf(stderr, "rmap[%d] accessed\n", (node.x + xp) + (node.y + yp) * mapW);
				}
			}
		}
		//out = (intpair*)realloc(out, sizeof(intpair) * *outlen);
	}
	//fprintf(stderr, "end pred call\n");

}
void Graph::succ(intpair node, intpair** arr, int* outlen) {
	pred(node, arr, outlen);
}