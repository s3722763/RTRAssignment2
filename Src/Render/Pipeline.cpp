#include "Pipeline.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

void Pipeline::init(PipelineCreateInfo* info) {
	this->programID = glCreateProgram();

	// Create vertex shader
	if ((info->flags & PipelineCreateInfoFlags::VertexShader) != 0) {
		GLuint vertex_shader = 0;
		if (this->load_shader_module(info->vertexShaderPath, GL_VERTEX_SHADER, &vertex_shader, info->defines)) {
			// Attach vertex shader to program
			glAttachShader(this->programID, vertex_shader);
		} else {
			this->print_shader_log(vertex_shader);
		}
	}

	if ((info->flags & PipelineCreateInfoFlags::FragmentShader) != 0) {
		GLuint fragment_shader = 0;
		if (this->load_shader_module(info->fragmentShaderPath, GL_FRAGMENT_SHADER, &fragment_shader, info->defines)) {
			glAttachShader(this->programID, fragment_shader);
		} else {
			this->print_shader_log(fragment_shader);
		}
	}

	if ((info->flags & PipelineCreateInfoFlags::GeometryShader) != 0) {
		GLuint geometry_shader = 0;
		if (this->load_shader_module(info->geometryShaderPath, GL_GEOMETRY_SHADER, &geometry_shader, info->defines)) {
			glAttachShader(this->programID, geometry_shader);
		} else {
			this->print_shader_log(geometry_shader);
		}
	}

	if ((info->flags & PipelineCreateInfoFlags::TessellationControlShader) != 0) {
		GLuint tesselation_control_shader = 0;
		if (this->load_shader_module(info->tessellationControlShaderPath, GL_TESS_CONTROL_SHADER, &tesselation_control_shader, info->defines)) {
			glAttachShader(this->programID, tesselation_control_shader);
		} else {
			this->print_shader_log(tesselation_control_shader);
		}
	}

	if ((info->flags & PipelineCreateInfoFlags::TessellationEvaluationShader) != 0) {
		GLuint tesselation_evaluation_shader = 0;
		if (this->load_shader_module(info->geometryShaderPath, GL_TESS_EVALUATION_SHADER, &tesselation_evaluation_shader, info->defines)) {
			glAttachShader(this->programID, tesselation_evaluation_shader);
		} else {
			this->print_shader_log(tesselation_evaluation_shader);
		}
	}

	if ((info->flags & PipelineCreateInfoFlags::ComputeShader) != 0) {
		GLuint compute_shader = 0;
		if (this->load_shader_module(info->geometryShaderPath, GL_TESS_EVALUATION_SHADER, &compute_shader, info->defines)) {
			glAttachShader(this->programID, compute_shader);
		} else {
			this->print_shader_log(compute_shader);
		}
	}

	glLinkProgram(this->programID);

	// Check for errors
	GLint program_success = GL_TRUE;

	glGetProgramiv(this->programID, GL_LINK_STATUS, &program_success);

	if (program_success != GL_TRUE) {
		std::cout << "Error linking program: " << this->programID << "\n";
		this->print_program_log(this->programID);
	}
}

void Pipeline::setMatrix4x4Uniform(const char* name, const glm::mat4& matrix) {
	glUniformMatrix4fv(glGetUniformLocation(this->programID, name), 1, GL_FALSE, &matrix[0][0]);
}

void Pipeline::setVec4Uniform(const char* name, const glm::vec4& vector) {
	glUniform4fv(glGetUniformLocation(this->programID, name), 1, glm::value_ptr(vector));
}

void Pipeline::setVec3Uniform(const char* name, const glm::vec3& vector) {
	glUniform3fv(glGetUniformLocation(this->programID, name), 1, glm::value_ptr(vector));
}

void Pipeline::setFloatUniform(const char* name, const GLfloat& value) {
	glUniform1f(glGetUniformLocation(this->programID, name), value);
}

void Pipeline::setBooleanUniform(const char* name, const GLboolean& value) {
	glUniform1i(glGetUniformLocation(this->programID, name), value);
}

void Pipeline::bindUniformBlock(const char* name, GLuint binding) {
	GLuint index = glGetUniformBlockIndex(this->programID, name);
	glUniformBlockBinding(this->programID, index, binding);
}

GLuint Pipeline::getVertexAttribIndex(const char* name) {
	return glGetAttribLocation(this->programID, name);
}

GLuint Pipeline::getUniformBlockIndex(const char* name) {
	return glGetUniformBlockIndex(this->programID, name);
}

void Pipeline::use() {
	glUseProgram(this->programID);
}

void Pipeline::destroy() {
	glDeleteProgram(this->programID);
}

bool Pipeline::load_shader_module(const char* file_path, GLuint shader_type, GLuint* id, const std::map<std::string, std::string>* defines) {
	std::ifstream file(file_path);

	if (!file.is_open()) {
		return false;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	std::string code_str = buffer.str();

	size_t endFirstLine = code_str.find('\n');
	std::string startString = code_str.substr(0, endFirstLine + 1);
	std::string endString = code_str.substr(endFirstLine, code_str.length());
	std::string code_with_defines;

	if (defines != nullptr) {
		for (auto& defined : *defines) {
			std::string definition = "#define " + defined.first + " " + defined.second + "\n";
			startString.append(definition);
		}
	}

	startString.append(endString);

	const char* code_char = startString.c_str();

	//std::cout << code_char << '\n';

	GLuint shader = glCreateShader(shader_type);

	glShaderSource(shader, 1, &code_char, NULL);

	glCompileShader(shader);

	GLint shader_compiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);

	if (shader_compiled != GL_TRUE) {
		std::cout << "Unable to compile shader: " << shader << "\n";
		this->print_shader_log(shader);
		return false;
	}

	*id = shader;
	return true;
}

void Pipeline::print_shader_log(GLuint shader) {
	if (glIsShader(shader)) {
		int info_log_length = 0;
		int max_length = info_log_length;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

		char* info_log = new char[max_length];

		glGetShaderInfoLog(shader, max_length, &info_log_length, info_log);

		if (info_log_length > 0) {
			std::cout << info_log << "\n";
		}

		delete[] info_log;
	}
}

void Pipeline::print_program_log(GLuint program) {
	if (glIsProgram(program)) {
		int info_log_length = 0;
		int max_length = info_log_length;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

		char* info_log = new char[max_length];

		glGetProgramInfoLog(program, max_length, &info_log_length, info_log);

		if (info_log_length > 0) {
			std::cout << info_log << "\n";
		}

		delete[] info_log;
	}
}