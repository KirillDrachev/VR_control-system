#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
#include <iostream>



#define COMPILE_ONLY = 1

class Shader_Program {

public:
	unsigned int ID;
	bool err = true;
	Shader_Program(){}
	Shader_Program(const char* vertex_sh_path_, const char* fragment_sh_path_);
	Shader_Program(const char* vertex_sh_path_, const char* fragment_sh_path_, char* Err_pt_, unsigned int errbuflen_);
	Shader_Program(const char* vertex_sh_source_code_, const char* fragment_sh_source_code_, int isOnlyCompile_);
	Shader_Program(const char* vertex_sh_source_code_, const char*fragment_sh_source_code_,char* Err_pt_,unsigned int errbuflen_, int isCompileOnly_);
	//~Shader_Program();

	void use();
//private:
	const char* vertex_sh_path;
	const char* fragment_sh_path;
	
	/*char vertex_sh_source_code[1024];
	char fragment_sh_source_code[1024];*/
    const char* vertex_sh_source_code;
	const char* fragment_sh_source_code;
    char* Err_pt = nullptr;
	unsigned int errbuflen;

	bool compile();
	bool loadAndCompile();
};