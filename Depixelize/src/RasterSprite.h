#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "stb_image.h"

class RasterSprite
{
private:
	std::string imagePath;
	unsigned char* localBuffer;
	int imageWidth, imageHeight, bitsPerPixel;

	//Texture displayTexture;
	std::vector<std::vector<unsigned int>> pixelData;
	float spriteScale;
	float pixelRenderWidth, pixelRenderHeight;
	int pixelsPerRasterSquare;
	int pixelCols, pixelRows;
	int windowWidth, windowHeight;

public:
	RasterSprite(std::string path, int wWidth, int wHeight, int pixelsPerSquare, int scale);
	~RasterSprite();

	void getVertices(float vertices[]);
	void getIndices(unsigned int indices[]);

	//Texture getDisplayTexture() const;

	float* getRasterTriangles();
	unsigned int* getRasterIndices();
	int getRasterVertexCount();
	int getRasterIndexCount();

	inline int getPixelRows() { return pixelRows; }
	inline int getPixelCols() { return pixelCols; }
	inline float getPixelRenderWidth() { return pixelRenderWidth; }
	inline float getPixelRenderHeight() { return pixelRenderHeight; }
	inline std::vector<std::vector<unsigned int>> getPixelData() { return pixelData; }

private:
	void extractPixelData();
};