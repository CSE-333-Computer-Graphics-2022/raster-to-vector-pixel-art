#include "ObjectRenderer.h"

ObjectRenderer::ObjectRenderer(const void* vertexData, unsigned int vertexDataSize, const void* indexData, unsigned int indexcount)
	:indexCount(indexcount)
{
	// Initialising Vertex Array
	glGenVertexArrays(1, &VAO_ID);

	// Initialising Vertex Buffer
	glGenBuffers(1, &VBO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID); // Selects the particular buffer, and specifies that the buffer generated is an array
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW); // Specifying the array that contains the buffer

	// Initialising Index Buffer
	glGenBuffers(1, &IBO_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
}

ObjectRenderer::~ObjectRenderer()
{
	// Deallocating Vertex Array
	glDeleteVertexArrays(1, &VAO_ID);

	// Deallocating Index Buffer
	glDeleteBuffers(1, &IBO_ID);

	glDeleteProgram(shaderProgram);
}

void ObjectRenderer::bindVAO()
{
	glBindVertexArray(VAO_ID);
}

void ObjectRenderer::bindVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
}

void ObjectRenderer::bindIBO()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_ID);
}

void ObjectRenderer::unbindVAO()
{
	glBindVertexArray(0);
}

void ObjectRenderer::unbindVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ObjectRenderer::unbindIBO()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ObjectRenderer::updateVBO(const void* vertexData, unsigned int vertexDataSize)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW);
}

void ObjectRenderer::updateIBO(const void* indexData, unsigned int indexcount)
{
	indexCount = indexcount;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
}

void ObjectRenderer::setShaders(char* vertexShader, char* fragmentShader)
{
	vertexShaderCode = getShaderCode(vertexShader);
	fragmentShaderCode = getShaderCode(fragmentShader);

	shaderProgram = CreateShader(vertexShaderCode, fragmentShaderCode);
}

void ObjectRenderer::useShader()
{
	glUseProgram(shaderProgram);
}

void ObjectRenderer::addFloatAttrib(std::string name, int size)
{
	attribNames.push_back(name);
	attribSizes.push_back(size);
}

void ObjectRenderer::AddBuffersToVAO()
{
	unsigned int stride = 0;
	unsigned int offset = 0;
	for (int a : attribSizes)
	{
		stride += a;
	}

	bindVAO();
	bindVBO();
	for (int i = 0; i < attribSizes.size(); i++)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, attribSizes[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (const void*)offset);
		offset += attribSizes[i] * sizeof(float);
	}
}

void ObjectRenderer::drawTriangles()
{
	glUseProgram(shaderProgram);
	bindVAO();
	bindIBO();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void ObjectRenderer::drawLines()
{
	glUseProgram(shaderProgram);
	bindVAO();
	bindIBO();
	glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
}

unsigned int ObjectRenderer::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); // Pointer to the beginning of the data/program
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

unsigned int ObjectRenderer::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram(); // returns a number corresponding to created program
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

char* ObjectRenderer::getShaderCode(const char* filename)
{
	FILE* input;
	int err = fopen_s(&input, filename, "rb");
	if (err != 0)
	{
		std::cout << "Error opening file";
		return NULL;
	}
	if (input == NULL) return NULL;

	if (fseek(input, 0, SEEK_END) == -1) return NULL;
	long size = ftell(input);
	if (size == -1) return NULL;
	if (fseek(input, 0, SEEK_SET) == -1) return NULL;

	/*if using c-compiler: dont cast malloc's return value*/
	char* content = (char*)malloc((size_t)size + 1);
	if (content == NULL) return NULL;

	fread(content, 1, (size_t)size, input);
	if (ferror(input)) {
		free(content);
		return NULL;
	}

	fclose(input);
	content[size] = '\0';
	return content;
}