#pragma once

#include <map>

#include "HalfEdgeObjects.h"
#include "SubPixel.h"
#include "RasterSprite.h"
#include "SimilarityGraph.h"
#include "Vector2D.h"

class VoroniGraph
{
private:
	RasterSprite* sprite;
	SimilarityGraph* graph;
	PixelFace** pixelFaces;
	SubPixel** subpixels;

	float* voroniGraphTriangles;
	unsigned int* voroniGraphIndices;
	int voroniGraphTriangleCount;

	float* voroniVisibleEdgeLines;
	unsigned int* voroniVisibleEdgeIndices;
	int voroniVisibleEdgeCount;

public:
	VoroniGraph(RasterSprite* rasterSprite, SimilarityGraph* similarityGraph);
	~VoroniGraph();

	void setPixelFaces();
	void setSubPixels();
	void constructHalfEdges();
	void setVoroniGraphTriangles();
	void setVoroniVisibleEdgeLines();
	void setHalfEdgesVisibility();
	void simplifyVoroniGraph();

	inline float* getVoroniGraphTriangles() { return voroniGraphTriangles; }
	inline unsigned int* getVoroniGraphIndices() { return voroniGraphIndices; }
	inline int getVoroniGraphTriangleCount() { return voroniGraphTriangleCount; }

	inline float* getVoroniVisibleEdgeLines() { return voroniVisibleEdgeLines; }
	inline unsigned int* getVoroniVisibleEdgeIndices() { return voroniVisibleEdgeIndices; }
	inline int getVoroniVisibleEdgeCount() { return voroniVisibleEdgeCount; }

	// Methods for converting visible straight edges to spline curves
	Vector2D* findBezierPoint(HalfEdge* edge);
	void insertSplineCurve(HalfEdge* edge, Vector2D* bezierPoint, int samples);

	void replaceVisibleEdgesWithSplines();

private:
	void constructBorderEdges();
	void constructSubPixelEdges();
};