#pragma once
#include "HalfEdgeObjects.h"
#include "SubPixel.h"
#include "RasterSprite.h"
#include "SimilarityGraph.h"

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

public:
	VoroniGraph(RasterSprite* rasterSprite, SimilarityGraph* similarityGraph);
	~VoroniGraph();

	void setPixelFaces();
	void setSubPixels();
	void constructHalfEdges();
	void setVoroniGraphTriangles();
	void simplifyVoroniGraph();

	inline float* getVoroniGraphTriangles() { return voroniGraphTriangles; }
	inline unsigned int* getVoroniGraphIndices() { return voroniGraphIndices; }
	inline int getVoroniGraphTriangleCount() { return voroniGraphTriangleCount; }

private:
	void constructBorderEdges();
	void constructSubPixelEdges();
};