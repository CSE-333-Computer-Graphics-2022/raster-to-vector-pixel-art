#pragma once

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class ObjectRenderer
{
private:
	unsigned int VAO_ID;
	unsigned int VBO_ID;
	unsigned int vertexCount;
	unsigned int IBO_ID;
	unsigned int indexCount;
	unsigned int shaderProgram;

	char* vertexShaderCode;
	char* fragmentShaderCode;

	std::vector<std::string> attribNames;
	std::vector<int> attribSizes;

public:
	ObjectRenderer(const void* vertexData, unsigned int vertexDataSize, const void* indexData, unsigned int indexCount);
	~ObjectRenderer();

	// Binding & Unbinding functions
	void bindVAO();
	void bindVBO();
	void bindIBO();
	void unbindVAO();
	void unbindVBO();
	void unbindIBO();

	void updateVBO(const void* vertexData, unsigned int vertexDataSize);
	void updateIBO(const void* indexData, unsigned int indexcount);

	// Create shader program
	void setShaders(char* vertexShader, char* fragmentShader);
	void useShader();

	// Functions for defining buffer data
	void addFloatAttrib(std::string name, int size);
	void AddBuffersToVAO();

	// Functions for rendering images (Using lines, or triangles)
	void drawTriangles();
	void drawLines();

private:
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	char* getShaderCode(const char* filename);
};