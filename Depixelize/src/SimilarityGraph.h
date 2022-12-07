#pragma once

#include <vector>

#include "RasterSprite.h"

class SimilarityGraph
{
private:
	RasterSprite rasterSprite;

	// Adjacent vertices are numbered 0-7 as follows:

	// 0   1   2
	//   \ | /
	// 3 - V - 4
	//   / | \ 
	// 5   6   7

	std::vector<std::vector<bool>> adjacencyList;
	float vertexRadius;

	float* graphVertexVertices;
	unsigned int* graphVertexIndices;
	float* graphEdgeVertices;
	unsigned int* graphEdgeIndices;

	int graphEdgeCount;

public:
	SimilarityGraph(RasterSprite raster, float vertexRenderRadius);
	~SimilarityGraph();

	float* getGraphVertexVertices();
	unsigned int* getGraphVertexIndices();
	float* getGraphEdgeVertices();
	unsigned int* getGraphEdgeIndices();
	int getGraphEdgeCount();

	// Returns the number of 2D vertices to be stored in the vertex buffer
	int getGraphVertexRenderCount();
	// Returns the number of indices to be stored in the index buffer
	int getGraphVertexIndexCount();

	// Set edges between all adjacent (side/diagonal) vertices, BASED ON ADJACENCY LIST
	void setGraphEdges();
	// Resolve color dissimilarities; Remove edges between vertices having dissimilar colours
	void resolveGraphDissimilarities();
	// Resolve checkerboard overlaps, i.e., 2x2 subgrids having checkerboard patterns
	void resolveGraphCheckerboarding();

	inline bool edgeExists(int i, int j, int edgeNo) { return adjacencyList[i * rasterSprite.getPixelCols() + j][edgeNo]; }

private:
	bool colorsSimilar(std::vector<std::vector<unsigned int>> pixelData, int i1, int j1, int i2, int j2);
	int vertexDegree(int i, int j);
};