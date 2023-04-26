#include <Shader_Program.h>

Shader_Program::Shader_Program(const char* vertex_sh_path_, const char* fragment_sh_path_) {
	vertex_sh_path = vertex_sh_path_;
	fragment_sh_path = fragment_sh_path_;
	Err_pt = nullptr;
	errbuflen = 1;
	err = this->loadAndCompile();
}

Shader_Program::Shader_Program(const char* vertex_sh_path_, const char* fragment_sh_path_, char* Err_pt_, unsigned int errbuflen_) {
	vertex_sh_path = vertex_sh_path_;
	fragment_sh_path = fragment_sh_path_;
	Err_pt = Err_pt_;
	errbuflen = errbuflen_;
	err = this->loadAndCompile();
}

//Shader_Program::Shader_Program(const char* vertex_sh_source_code_, const char* fragment_sh_source_code_, int isCompileOnly_) {
//	vertex_sh_source_code = vertex_sh_source_code_;
//	fragment_sh_source_code = fragment_sh_source_code_;
//	Err_pt = nullptr;
//	errbuflen = 1;
//	err = Shader_Program::compile();
//}
//Shader_Program::Shader_Program(const char* vertex_sh_source_code_, const char* fragment_sh_source_code_, char* Err_pt_, unsigned int errbuflen_, int isCompileOnly_) {
//	vertex_sh_source_code = vertex_sh_source_code_;
//	fragment_sh_source_code = fragment_sh_source_code_;
//	Err_pt = Err_pt_;
//	errbuflen = errbuflen_;
//	err = Shader_Program::compile();
//}


bool  Shader_Program::compile() {
	bool DidCompile = true;
	unsigned int VertSh, FragmSh;
	int is_ok = 0;
    //--------
	VertSh = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertSh, 1, &(this->vertex_sh_source_code), NULL);
	glGetShaderiv(VertSh, GL_COMPILE_STATUS, &is_ok);
	/*if (!is_ok) {
		if(this->Err_pt != nullptr){
			glGetShaderInfoLog(VertSh, this->errbuflen, NULL, this->Err_pt );
		}
		DidCompile = false;
	}*/
	glGetShaderInfoLog(VertSh, this->errbuflen, NULL, this->Err_pt);
	std::cout << this->Err_pt << std::endl;
	//--------
	FragmSh = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmSh, 1, &(this->fragment_sh_source_code), NULL);
	glGetShaderiv(FragmSh, GL_COMPILE_STATUS, &is_ok);
	/*if (!is_ok) {
		if (this->Err_pt != nullptr) {
			glGetShaderInfoLog(FragmSh, this->errbuflen, NULL, this->Err_pt);
		}
		DidCompile = false;
	}*/
	glGetShaderInfoLog(FragmSh, this->errbuflen, NULL, this->Err_pt);
	std::cout << this->Err_pt << std::endl;
	//--------
	this->ID = glCreateProgram();
	glAttachShader(this->ID, VertSh);
	glAttachShader(this->ID, FragmSh);
	glLinkProgram(this->ID);
	glGetProgramiv(this->ID, GL_LINK_STATUS, &is_ok);
	/*if (!is_ok) {
		if (this->Err_pt != nullptr) {
			glGetProgramInfoLog(this->ID, this->errbuflen, NULL, this->Err_pt);
		}
		DidCompile = false;
	}*/
	glGetProgramInfoLog(this->ID, this->errbuflen, NULL, this->Err_pt);
	std::cout << this->Err_pt << std::endl;
	//--------
	glDeleteShader(VertSh);
	glDeleteShader(FragmSh);
	return DidCompile;
}

bool  Shader_Program::loadAndCompile() {

	std::string vertex_sh_source_code_str;
	std::string fragment_sh_source_code_str;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
	
		vShaderFile.open(this->vertex_sh_path);
		fShaderFile.open(this->fragment_sh_path);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertex_sh_source_code_str = vShaderStream.str();
		fragment_sh_source_code_str = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		//(this->Err_pt) = ((std::string)"ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ").c_str();
		return false;
	}
	(this->vertex_sh_source_code) = vertex_sh_source_code_str.c_str();
	this->fragment_sh_source_code = fragment_sh_source_code_str.c_str();
	bool IsOk = this->compile();
    
	return IsOk;
}

void Shader_Program::use() {
	
	glUseProgram(this->ID);
	
}