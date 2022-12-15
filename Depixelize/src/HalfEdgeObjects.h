#pragma once

#include <iostream>
#include <vector>

#include "Vector2D.h"

class HalfEdge;
class PixelFace;

class VoroniVertex
{
public:
	float posX;
	float posY;
	HalfEdge* edge;

	// Pair of visible edges that are adjacent (if any)
	HalfEdge** incomingOutgoingEdgePair;
	int incomingOutgoingEdgePairCount;

	VoroniVertex(float xpos, float ypos)
		: posX(xpos), posY(ypos), incomingOutgoingEdgePair(NULL), incomingOutgoingEdgePairCount(0)
	{
		edge = NULL;
	}
	~VoroniVertex();

	void resolveTJunctions();

	inline void setPosition(float x, float y) { posX = x; posY = y; edge = NULL; }
	inline void setEdge(HalfEdge* nedge) { edge = nedge; }

	HalfEdge* getOutgoingOf(HalfEdge* incomingEdge);
	HalfEdge* getIncomingOf(HalfEdge* outgoingEdge);

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
	PixelFace();
	~PixelFace();

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

	bool visible;

public:
	inline HalfEdge(VoroniVertex* v, PixelFace* p)
		: vertex(v), pixel(p), visible(false)
	{
		next = NULL;
		prev = NULL;
		opp = NULL;
	}

	void setVisibility();
	void setVisibility(bool setVisible);
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
	inline bool isVisible() { return visible; }
};