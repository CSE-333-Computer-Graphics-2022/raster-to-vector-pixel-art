#pragma once

#include <iostream>

class HalfEdge;

class VoroniVertex
{
public:
	float posX;
	float posY;
	HalfEdge* edge;

	VoroniVertex(float xpos, float ypos)
		: posX(xpos), posY(ypos)
	{
		edge = NULL;
	}

	inline void setPosition(float x, float y) { posX = x; posY = y; edge = NULL; }
	inline void setEdge(HalfEdge* nedge) { edge = nedge; }

	inline float getPosX() { return posX; }
	inline float getPosY() { return posY; }
	inline HalfEdge* getEdge() { return edge; }
};

class PixelFace
{
private:
	int index;
	float posX;
	float posY;
	float colR;
	float colG;
	float colB;
	float colA;
	HalfEdge* edge;

public:
	inline PixelFace()
	{
		edge = NULL;
		colR = 0.0f;
		colG = 0.0f;
		colB = 0.0f;
		colA = 0.0f;
	}
	/*inline PixelFace(int i) :
		index(i)
	{
		edge = NULL;
	}*/

	inline void setPosition(float x, float y) { posX = x; posY = y; }
	inline void setColor(float r, float g, float b, float a) { colR = r; colG = g; colB = b; colA = a; }
	inline void setEdge(HalfEdge* newedge) { edge = newedge; }

	inline float getPosX() { return posX; }
	inline float getPosY() { return posY; }
	inline float getColR() { return colR; }
	inline float getColG() { return colG; }
	inline float getColB() { return colB; }
	inline float getColA() { return colA; }
	inline HalfEdge* getEdge() { return edge; }
};

class HalfEdge
{
private:
	VoroniVertex* vertex;
	PixelFace* pixel;
	HalfEdge* next;
	HalfEdge* prev;
	HalfEdge* opp;

public:
	inline HalfEdge(VoroniVertex* v, PixelFace* p)
		: vertex(v), pixel(p)
	{
		next = NULL;
		prev = NULL;
		opp = NULL;
	}
	/*inline HalfEdge(VoroniVertex v, PixelFace p)
		: vertex(&v), pixel(&p)
	{
		next = NULL;
		prev = NULL;
		opp = NULL;
	}*/

	inline void setNext(HalfEdge* e) { next = e; }
	inline void setPrev(HalfEdge* e) { prev = e; }
	inline void setOpp(HalfEdge* e) { opp = e; }
	inline void setNextwithPrev(HalfEdge* e) { next = e; e -> setPrev(this); }
	inline void setOppPair(HalfEdge* e) { opp = e; e->setOpp(this); }

	inline VoroniVertex* getVertex() { return vertex; }
	inline PixelFace* getPixel() { return pixel; }
	inline HalfEdge* getNext() { return next; }
	inline HalfEdge* getPrev() { return prev; }
	inline HalfEdge* getOpp() { return opp; }
};