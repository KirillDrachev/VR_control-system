#pragma once
#include <Shader_Program.h>
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class display_plane {

public:

	unsigned int TextureID;
	unsigned int PixelHeight;
	unsigned int PixelWidth; 
	bool IsTextureUse;
	glm::vec4 MyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glm::mat4 Mat = glm::mat4(1.0f);
	glm::mat4 InitialScaleMat;
	glm::mat4 ScaleMat;
	glm::mat4 RotateMat;
	glm::mat4 TranslateMat;
	glm::mat4 TransformMat;
	void* ProjectionMat;
	glm::vec3 ScaleVec = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ForwardVec = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 UpwardVec = glm::vec3(0, 1.0f, 0.0);
	glm::vec3 PositionVec = glm::vec3(0.0, 0.0, -1.0f);

	display_plane(Shader_Program *ShProg_, unsigned int PixelWidth_, unsigned int PixelHeight_, char *image_, void* ProjectionMat_);
	display_plane(Shader_Program* ShProg_, unsigned int PixelWidth_, unsigned int PixelHeight_, glm::vec4 MyColor_, void* ProjectionMat_);
//	~display_plane();
	
	void draw();

private:
	Shader_Program *ShProg;
    GLuint VBO;
	GLuint VAO;
	void setup();
	void TransformMatUpdate();
	glm::mat4 computeRotateMat(glm::vec3 forward, glm::vec3 up_);
	float vertices[30] = {
		
	 1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
	 1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,     0.0f, 1.0f,

	
	 1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,     0.0f, 1.0f
	};
	char* image;
	
};