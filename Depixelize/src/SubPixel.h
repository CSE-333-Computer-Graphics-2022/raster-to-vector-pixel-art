#pragma once

#include "HalfEdgeObjects.h"

class SubPixel
{
private:
	VoroniVertex* vertexU;
	VoroniVertex* vertexD;
	VoroniVertex* vertexL;
	VoroniVertex* vertexR;

public:
	SubPixel();
	SubPixel(VoroniVertex* U, VoroniVertex* D, VoroniVertex* L, VoroniVertex* R);
	~SubPixel();

	inline void setVertexU(VoroniVertex* v) { vertexU = v; }
	inline void setVertexD(VoroniVertex* v) { vertexD = v; }
	inline void setVertexL(VoroniVertex* v) { vertexL = v; }
	inline void setVertexR(VoroniVertex* v) { vertexR = v; }

	inline VoroniVertex* getVertexU() { return vertexU; }
	inline VoroniVertex* getVertexD() { return vertexD; }
	inline VoroniVertex* getVertexL() { return vertexL; }
	inline VoroniVertex* getVertexR() { return vertexR; }
};