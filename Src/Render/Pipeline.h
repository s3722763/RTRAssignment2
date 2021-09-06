#pragma once

#include <glad/glad.h>
#include <map>
#include <string>
#include <glm/mat4x4.hpp>

//typedef uint64_t PipelineCreateFlags;

typedef enum PipelineCreateInfoFlags {
	VertexShader = 1 << 0,
	FragmentShader = 1 << 1,
	GeometryShader = 1 << 2,
	TessellationControlShader = 1 << 3,
	TessellationEvaluationShader = 1 << 4,
	ComputeShader = 1 << 5
} PipelineInfoFlags;

// const char* vertexPath, const char* fragmentPath, const std::map<std::string, std::string>* defines
typedef struct PipelineCreateInfo {
	uint64_t flags;
	const std::map<std::string, std::string>* defines;
	const char* vertexShaderPath;
	const char* fragmentShaderPath;
	const char* geometryShaderPath;
	const char* tessellationControlShaderPath;
	const char* tessellationEvaulationShaderPath;
	const char* computeShaderPath;
} PipelineCreateInfo;

class Pipeline {
private:
	GLuint programID;

	bool load_shader_module(const char* file_path, GLuint shader_type, GLuint* id, const std::map<std::string, std::string>* defines);
	void print_program_log(GLuint program);
	void print_shader_log(GLuint shader);

public:
	void init(PipelineCreateInfo* info);

	void setMatrix4x4Uniform(const char* name, const glm::mat4 &matrix);
	void setVec4Uniform(const char* name, const glm::vec4& vector);
	void setVec3Uniform(const char* name, const glm::vec3& vector);
	void setFloatUniform(const char* name, const GLfloat& value);
	void setBooleanUniform(const char* name, const GLboolean& value);

	GLuint getVertexAttribIndex(const char* name);
	GLuint getUniformBlockIndex(const char* name);
	void bindUniformBlock(const char* name, GLuint point);

	void use();
	void destroy();
};