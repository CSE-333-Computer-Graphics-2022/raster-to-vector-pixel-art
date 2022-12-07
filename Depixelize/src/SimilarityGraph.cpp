#include "SimilarityGraph.h"

SimilarityGraph::SimilarityGraph(RasterSprite raster, float vertexRenderRadius)
	: rasterSprite(raster), vertexRadius(vertexRenderRadius), graphEdgeCount(0)
{
	int pixelRows = raster.getPixelRows();
	int pixelCols = raster.getPixelCols();
	std::vector<std::vector<unsigned int>> pixelData = rasterSprite.getPixelData();

	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			//std::vector<bool> newvec = { true, true, true, true, true, true, true, true };
			std::vector<bool> newvec = {false, false, false, false, false, false, false, false};
			adjacencyList.push_back(newvec);
		}
	}

	resolveGraphDissimilarities();
	resolveGraphCheckerboarding();
	setGraphEdges();

	// SETTING GRAPH VERTEX VERTICES
	float* vertices = (float*)malloc((2+4) * getGraphVertexRenderCount() * sizeof(float));

	/*const int pixelRows = rasterSprite.getPixelRows();
	const int pixelCols = rasterSprite.getPixelCols();*/
	int ptr = 0;
	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			float x = (2 * j - (pixelCols - 1)) * rasterSprite.getPixelRenderWidth() / 2;
			float y = (2 * (pixelRows-i-1) - (pixelRows - 1)) * rasterSprite.getPixelRenderHeight() / 2;

			vertices[ptr++] = x - (vertexRadius * rasterSprite.getPixelRenderWidth());
			vertices[ptr++] = y + (vertexRadius * rasterSprite.getPixelRenderHeight());
			vertices[ptr++] = 1.0f - pixelData[i][4 * j] / 255.0f; vertices[ptr++] = 1.0f - pixelData[i][4 * j + 1] / 255.0f; 
			vertices[ptr++] = 1.0f - pixelData[i][4 * j + 2] / 255.0f; vertices[ptr++] = (pixelData[i][4 * j + 3] == 0 ? 0.2f : 1.0f);

			vertices[ptr++] = x + (vertexRadius * rasterSprite.getPixelRenderWidth());
			vertices[ptr++] = y + (vertexRadius * rasterSprite.getPixelRenderHeight());
			vertices[ptr++] = 1.0f - pixelData[i][4 * j] / 255.0f; vertices[ptr++] = 1.0f - pixelData[i][4 * j + 1] / 255.0f;
			vertices[ptr++] = 1.0f - pixelData[i][4 * j + 2] / 255.0f; vertices[ptr++] = (pixelData[i][4 * j + 3] == 0 ? 0.2f : 1.0f);

			vertices[ptr++] = x - (vertexRadius * rasterSprite.getPixelRenderWidth());
			vertices[ptr++] = y - (vertexRadius * rasterSprite.getPixelRenderHeight());
			vertices[ptr++] = 1.0f - pixelData[i][4 * j] / 255.0f; vertices[ptr++] = 1.0f - pixelData[i][4 * j + 1] / 255.0f;
			vertices[ptr++] = 1.0f - pixelData[i][4 * j + 2] / 255.0f; vertices[ptr++] = (pixelData[i][4 * j + 3] == 0 ? 0.2f : 1.0f);

			vertices[ptr++] = x + (vertexRadius * rasterSprite.getPixelRenderWidth());
			vertices[ptr++] = y - (vertexRadius * rasterSprite.getPixelRenderHeight());
			vertices[ptr++] = 1.0f - pixelData[i][4 * j] / 255.0f; vertices[ptr++] = 1.0f - pixelData[i][4 * j + 1] / 255.0f;
			vertices[ptr++] = 1.0f - pixelData[i][4 * j + 2] / 255.0f; vertices[ptr++] = (pixelData[i][4 * j + 3] == 0 ? 0.2f : 1.0f);
		}
	}
	graphVertexVertices = vertices;

	// SETTING GRAPH VERTEX INDICES
	unsigned int* indices = (unsigned int*)malloc(getGraphVertexIndexCount() * sizeof(unsigned int));
	ptr = 0;
	for (int i = 0; i < 4 * rasterSprite.getPixelRows() * rasterSprite.getPixelCols(); i += 4)
	{
		indices[ptr++] = i + 2;
		indices[ptr++] = i + 1;
		indices[ptr++] = i + 0;
		indices[ptr++] = i + 1;
		indices[ptr++] = i + 2;
		indices[ptr++] = i + 3;
	}
	graphVertexIndices = indices;
}

SimilarityGraph::~SimilarityGraph()
{
	free(graphVertexVertices);
	free(graphVertexIndices);
	free(graphEdgeVertices);
	free(graphEdgeIndices);
}

float* SimilarityGraph::getGraphVertexVertices()
{
	return graphVertexVertices;
}

unsigned int* SimilarityGraph::getGraphVertexIndices()
{
	return graphVertexIndices;
}

float* SimilarityGraph::getGraphEdgeVertices()
{
	return graphEdgeVertices;
}

unsigned int* SimilarityGraph::getGraphEdgeIndices()
{
	return graphEdgeIndices;
}

int SimilarityGraph::getGraphEdgeCount()
{
	return graphEdgeCount;
}

int SimilarityGraph::getGraphVertexRenderCount()
{
	return rasterSprite.getPixelRows() * rasterSprite.getPixelCols() * 4;
}

int SimilarityGraph::getGraphVertexIndexCount()
{
	return rasterSprite.getPixelRows() * rasterSprite.getPixelCols() * 6;
}

void SimilarityGraph::setGraphEdges()
{
	int pixelRows = rasterSprite.getPixelRows();
	int pixelCols = rasterSprite.getPixelCols();
	std::vector<std::vector<unsigned int>> pixelData = rasterSprite.getPixelData();

	// SETTING GRAPH EDGE VERTICES (each graph vertex gets one)
	int ptr = 0;
	free(graphEdgeVertices);
	graphEdgeVertices = (float*)malloc(pixelRows * pixelCols * (2+4) * sizeof(float));

	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			graphEdgeVertices[ptr++] = (2 * j - (pixelCols - 1)) * rasterSprite.getPixelRenderWidth() / 2;
			graphEdgeVertices[ptr++] = (2 * (pixelRows-i-1) - (pixelRows - 1)) * rasterSprite.getPixelRenderHeight() / 2;
			graphEdgeVertices[ptr++] = 1.0f - pixelData[i][4 * j] / 255.0f; graphEdgeVertices[ptr++] = 1.0f - pixelData[i][4 * j + 1] / 255.0f;
			graphEdgeVertices[ptr++] = 1.0f - pixelData[i][4 * j + 2] / 255.0f; graphEdgeVertices[ptr++] = (pixelData[i][4 * j + 3] == 0 ? 0.25f : 0.5f);
		}
	}

	// SETTING GRAPH EDGE INDICES (based on whether it exists)
	graphEdgeCount = 0;
	free(graphEdgeIndices);
	graphEdgeIndices = (unsigned int*)malloc(getGraphVertexRenderCount() * 2 * sizeof(unsigned int));

	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			if (adjacencyList[i * pixelCols + j][4] && j < pixelCols-1)
			{
				graphEdgeIndices[2 * graphEdgeCount] = i * pixelCols + j;
				graphEdgeIndices[2 * graphEdgeCount + 1] = i * pixelCols + (j + 1);
				graphEdgeCount++;
			}
			if (adjacencyList[i * pixelCols + j][5] && i < pixelRows-1 && j > 0)
			{
				graphEdgeIndices[2 * graphEdgeCount] = i * pixelCols + j;
				graphEdgeIndices[2 * graphEdgeCount + 1] = (i + 1) * pixelCols + (j - 1);
				graphEdgeCount++;
			}
			if (adjacencyList[i * pixelCols + j][6] && i < pixelRows-1)
			{
				graphEdgeIndices[2 * graphEdgeCount] = i * pixelCols + j;
				graphEdgeIndices[2 * graphEdgeCount + 1] = (i + 1) * pixelCols + (j);
				graphEdgeCount++;
			}
			if (adjacencyList[i * pixelCols + j][7] && i < pixelRows-1 && j < pixelCols-1)
			{
				graphEdgeIndices[2 * graphEdgeCount] = i * pixelCols + j;
				graphEdgeIndices[2 * graphEdgeCount + 1] = (i + 1) * pixelCols + (j + 1);
				graphEdgeCount++;
			}
		}
	}
}

void SimilarityGraph::resolveGraphDissimilarities()
{
	int pixelRows = rasterSprite.getPixelRows();
	int pixelCols = rasterSprite.getPixelCols();

	std::vector<std::vector<unsigned int>> pixelData = rasterSprite.getPixelData();

	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			bool edgeShouldExist;
			
			if (j < pixelCols - 1)
			{
				edgeShouldExist = colorsSimilar(pixelData, i, j, i, j + 1);
				adjacencyList[(i)*pixelCols + (j)][4] = edgeShouldExist;
				adjacencyList[(i)*pixelCols + (j + 1)][3] = edgeShouldExist;
				if (!edgeShouldExist) graphEdgeCount--;
			}

			if (i < pixelRows-1 && j > 0)
			{
				edgeShouldExist = colorsSimilar(pixelData, i, j, i + 1, j - 1);
				adjacencyList[(i)*pixelCols + (j)][5] = edgeShouldExist;
				adjacencyList[(i + 1) * pixelCols + (j - 1)][2] = edgeShouldExist;
				if (!edgeShouldExist) graphEdgeCount--;
			}

			if (i < pixelRows - 1)
			{
				edgeShouldExist = colorsSimilar(pixelData, i, j, i + 1, j);
				adjacencyList[(i)*pixelCols + (j)][6] = edgeShouldExist;
				adjacencyList[(i + 1) * pixelCols + (j)][1] = edgeShouldExist;
				if (!edgeShouldExist) graphEdgeCount--;
			}
			
			if (i < pixelRows - 1 && j < pixelCols - 1)
			{
				edgeShouldExist = colorsSimilar(pixelData, i, j, i + 1, j + 1);
				adjacencyList[(i)*pixelCols + (j)][7] = edgeShouldExist;
				adjacencyList[(i + 1) * pixelCols + (j + 1)][0] = edgeShouldExist;
				if (!edgeShouldExist) graphEdgeCount--;
			}
		}
	}
}

void SimilarityGraph::resolveGraphCheckerboarding()
{
	int pixelRows = rasterSprite.getPixelRows();
	int pixelCols = rasterSprite.getPixelCols();

	for (int i = 0; i < pixelRows - 1; i++)
	{
		for (int j = 0; j < pixelCols - 1; j++)
		{
			// Are there overlapping diagonal edges? If yes, resolve them
			if (adjacencyList[i * pixelCols + j][7] && adjacencyList[i * pixelCols + j + 1][5])
			{
				// If all squares in the 2x2 grid are similar, remove both diagonals
				if (adjacencyList[i * pixelCols + j][4])
				{
					adjacencyList[i * pixelCols + j][7] = false;
					adjacencyList[(i + 1) * pixelCols + (j + 1)][0] = false;
					adjacencyList[i * pixelCols + j + 1][5] = false;
					adjacencyList[(i + 1) * pixelCols + j][2] = false;
					graphEdgeCount -= 2;
				}

				// Else, we have come across a checkerboard pattern. We need to choose one of the diagonals to be preserved, and remove the other
				
				// Preserve the set with minimum degree
				else
				{
					int deg0 = vertexDegree(i, j);
					int deg3 = vertexDegree(i + 1, j + 1);
					int deg1 = vertexDegree(i, j + 1);
					int deg2 = vertexDegree(i + 1, j);

					if (std::min(deg0, deg3) <= std::min(deg1, deg2))
					{
						adjacencyList[(i)*pixelCols + (j)][7] = true;
						adjacencyList[(i + 1) * pixelCols + (j + 1)][0] = true;
						adjacencyList[(i)*pixelCols + (j + 1)][5] = false;
						adjacencyList[(i + 1) * pixelCols + (j)][2] = false;
					}
					else
					{
						adjacencyList[(i)*pixelCols + (j)][7] = false;
						adjacencyList[(i + 1) * pixelCols + (j + 1)][0] = false;
						adjacencyList[(i)*pixelCols + (j + 1)][5] = true;
						adjacencyList[(i + 1) * pixelCols + (j)][2] = true;
					}
				}
			}
		}
	}
}

bool SimilarityGraph::colorsSimilar(std::vector<std::vector<unsigned int>> pixelData, int i1, int j1, int i2, int j2)
{
	unsigned int a[4] = { pixelData[i1][4 * j1],pixelData[i1][4 * j1 + 1], pixelData[i1][4 * j1 + 2], pixelData[i1][4 * j1 + 3] };
	unsigned int b[4] = { pixelData[i2][4 * j2],pixelData[i2][4 * j2 + 1], pixelData[i2][4 * j2 + 2], pixelData[i2][4 * j2 + 3] };

	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
}

int SimilarityGraph::vertexDegree(int i, int j)
{
	int pixelCols = rasterSprite.getPixelCols();
	int deg = 0;
	for (int c = 0; c < 8; c++)
	{
		if (adjacencyList[i * pixelCols + j][c])
			deg++;
	}
	return deg;
}