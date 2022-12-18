#pragma once

#include <iostream>
#include <vector>

#include "Vector2D.h"

class HalfEdge;
class PixelFace;

class VoroniVertex
{
public:
	//float posX;
	//float posY;
	Vector2D(pos);
	float origX;
	float origY;
	HalfEdge* edge;

	// If an edge is sharp, it cannot be moved by vertex optimisation algorithm
	bool sharp;

	// Pair of visible edges that are adjacent (if any)
	HalfEdge** incomingOutgoingEdgePair;
	int incomingOutgoingEdgePairCount;

	// Energy of curvature and displacement. Used for optimising spline interpolated images.
	float curvatureEnergy;
	float displacementEnergy;

	VoroniVertex(float xpos, float ypos)
		: pos(xpos, ypos), origX(xpos), origY(ypos), sharp(false),
		incomingOutgoingEdgePair(NULL), incomingOutgoingEdgePairCount(0),
		curvatureEnergy(1000.0f), displacementEnergy(1000.0f)
	{
		edge = NULL;
	}
	~VoroniVertex();

	void resolveTJunctions();

	inline void setPosition(float x, float y) { pos.x = x; pos.y = y; edge = NULL; }
	inline void setEdge(HalfEdge* nedge) { edge = nedge; }

	HalfEdge* getOutgoingOf(HalfEdge* incomingEdge);
	HalfEdge* getIncomingOf(HalfEdge* outgoingEdge);

	HalfEdge* getOutgoingVisibleEdge(int index);

	inline void setPosX(float x) { pos.x = x; }
	inline void setPosY(float y) { pos.y = y; }
	inline Vector2D getPos() { return pos; }
	inline float getPosX() { return pos.x; }
	inline float getPosY() { return pos.y; }
	inline HalfEdge* getEdge() { return edge; } 
	inline void setSharp(bool newSharpness) { sharp = newSharpness; }
	inline bool isSharp() { return sharp; };
};

class PixelFace
{
private:
	int index;
	//float posX;
	//float posY;
	Vector2D pos;
	float colR;
	float colG;
	float colB;
	float colA;
	HalfEdge* edge;

public:
	PixelFace();
	~PixelFace();

	inline void setPosition(float x, float y) { pos.x = x; pos.y = y; }
	inline void setColor(float r, float g, float b, float a) { colR = r; colG = g; colB = b; colA = a; }
	inline void setEdge(HalfEdge* newedge) { edge = newedge; }

	inline Vector2D getPos() { return pos; }
	inline float getPosX() { return pos.x; }
	inline float getPosY() { return pos.y; }
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