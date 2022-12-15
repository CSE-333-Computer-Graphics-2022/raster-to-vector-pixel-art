#include "RasterSprite.h"
#include <math.h>

RasterSprite::RasterSprite(std::string path, int wWidth, int wHeight, int pixelsPerSquare, int scale)
	: imagePath(path), /*displayTexture(path),*/ spriteScale(scale), pixelData(2), pixelsPerRasterSquare(pixelsPerSquare),
	windowWidth(wWidth), windowHeight(wHeight)/*, spriteVA(), spriteVB(), spriteIB()*/
{
	stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load(path.c_str(), &imageWidth, &imageHeight, &bitsPerPixel, 4);

	if (imageWidth % pixelsPerSquare != 0)
		std::cout << "Warning: Sprite width " << imageWidth <<
		"is not divisible by pixelsPerSquare " << pixelsPerSquare << std::endl;
	if (imageHeight % pixelsPerSquare != 0)
		std::cout << "Warning: Sprite height " << imageHeight <<
		"is not divisible by pixelsPerSquare " << pixelsPerSquare << std::endl;
	
	pixelCols = imageWidth / pixelsPerSquare;
	pixelRows = imageHeight / pixelsPerSquare;

	pixelRenderWidth = (float)scale / (std::max(pixelCols, pixelRows) * wWidth);
	pixelRenderHeight = (float)scale / (std::max(pixelCols, pixelRows) * wHeight);

	extractPixelData();
}

RasterSprite::~RasterSprite()
{}

void RasterSprite::getVertices(float vertices[])
{
	float positions[] = {
		-pixelRenderWidth * pixelCols / 2, -pixelRenderHeight * pixelRows / 2,   0.0f, 0.0f, // 0
		 pixelRenderWidth * pixelCols / 2, -pixelRenderHeight * pixelRows / 2,   1.0f, 0.0f, // 1
		 pixelRenderWidth * pixelCols / 2,  pixelRenderHeight * pixelRows / 2,   1.0f, 1.0f, // 2
		-pixelRenderWidth * pixelCols / 2,  pixelRenderHeight * pixelRows / 2,   0.0f, 1.0f  // 3
	};
	for (int i = 0; i < 16; i++)
	{
		vertices[i] = positions[i];
	}
}

void RasterSprite::getIndices(unsigned int indices[])
{
	unsigned int v_indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	for (int i = 0; i < 6; i++)
	{
		indices[i] = v_indices[i];
	}
}

float* RasterSprite::getRasterTriangles()
{
	float* vertices = (float*)malloc(pixelRows * pixelCols * 4 * 6 * sizeof(float));
	int ptr = 0;
	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			float x = (2 * j - (pixelCols - 1)) * getPixelRenderWidth() / 2;
			float y = (2 * (pixelRows - i - 1) - (pixelRows - 1)) * getPixelRenderHeight() / 2;

			vertices[ptr++] = x - (getPixelRenderWidth()/2);
			vertices[ptr++] = y + (getPixelRenderHeight()/2);
			vertices[ptr++] = (float)pixelData[i][4 * j] / 255.0f; vertices[ptr++] = (float)pixelData[i][4 * j + 1] / 255.0f;
			vertices[ptr++] = (float)pixelData[i][4 * j + 2] / 255.0f; vertices[ptr++] = (float)pixelData[i][4 * j + 3] / 255.0f;

			vertices[ptr++] = x + (getPixelRenderWidth() / 2);
			vertices[ptr++] = y + (getPixelRenderHeight() / 2);
			vertices[ptr++] = (float)pixelData[i][4 * j] / 255.0f; vertices[ptr++] = (float)pixelData[i][4 * j + 1] / 255.0f;
			vertices[ptr++] = (float)pixelData[i][4 * j + 2] / 255.0f; vertices[ptr++] = (float)pixelData[i][4 * j + 3] / 255.0f;

			vertices[ptr++] = x - (getPixelRenderWidth() / 2);
			vertices[ptr++] = y - (getPixelRenderHeight() / 2);
			vertices[ptr++] = (float)pixelData[i][4 * j] / 255.0f; vertices[ptr++] = (float)pixelData[i][4 * j + 1] / 255.0f;
			vertices[ptr++] = (float)pixelData[i][4 * j + 2] / 255.0f; vertices[ptr++] = (float)pixelData[i][4 * j + 3] / 255.0f;

			vertices[ptr++] = x + (getPixelRenderWidth() / 2);
			vertices[ptr++] = y - (getPixelRenderHeight() / 2);
			vertices[ptr++] = (float)pixelData[i][4 * j] / 255.0f; vertices[ptr++] = (float)pixelData[i][4 * j + 1] / 255.0f;
			vertices[ptr++] = (float)pixelData[i][4 * j + 2] / 255.0f; vertices[ptr++] = (float)pixelData[i][4 * j + 3] / 255.0f;
		}
	}
	return vertices;
}

unsigned int* RasterSprite::getRasterIndices()
{
	unsigned int* indices = (unsigned int*)malloc(pixelRows * pixelCols * 6 * sizeof(unsigned int));
	int ptr = 0;
	for (int i = 0; i < 4 * pixelRows * pixelCols; i += 4)
	{
		indices[ptr++] = i + 2;
		indices[ptr++] = i + 1;
		indices[ptr++] = i + 0;
		indices[ptr++] = i + 1;
		indices[ptr++] = i + 2;
		indices[ptr++] = i + 3;
	}
	return indices;
}

int RasterSprite::getRasterVertexCount()
{
	return pixelRows * pixelCols * 4;
}

int RasterSprite::getRasterIndexCount()
{
	return pixelRows * pixelCols * 6;
}

void RasterSprite::extractPixelData()
{
	unsigned char* pixels = localBuffer;
	for (int i = 0; i < pixelRows; i++)
	{
		std::vector<unsigned int> newvec;
		pixelData.push_back(newvec);
	}
	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			pixelData[pixelRows - i - 1].push_back((unsigned int)pixels[4 * (i * pixelCols * pixelsPerRasterSquare + j) * pixelsPerRasterSquare]);
			pixelData[pixelRows - i - 1].push_back((unsigned int)pixels[4 * (i * pixelCols * pixelsPerRasterSquare + j) * pixelsPerRasterSquare + 1]);
			pixelData[pixelRows - i - 1].push_back((unsigned int)pixels[4 * (i * pixelCols * pixelsPerRasterSquare + j) * pixelsPerRasterSquare + 2]);
			pixelData[pixelRows - i - 1].push_back((unsigned int)pixels[4 * (i * pixelCols * pixelsPerRasterSquare + j) * pixelsPerRasterSquare + 3]);
		}
	}
}
