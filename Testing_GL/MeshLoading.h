#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Textures.h"

using namespace std;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

class Mesh
{
public:
	int shininess = 1024;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Textures> textures;
	glm::vec3 ambient_color = glm::vec3(0.2f);
	glm::vec3 diffuse_color = glm::vec3(0.5f);
	glm::vec3 specular_color = glm::vec3(1.0f);
	
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Textures> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->SetupMesh();
	}

	void Render_Textures(GLuint shader_program, std::vector<Textures> textures)
	{

		glEnable(GL_TEXTURE_2D);
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		for (GLuint i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::stringstream ss;
			std::string number;
			std::string name = textures[i].type;

			/*if (name == "diffuse")
			{
				ss << diffuseNr++;
			}
			else if (name == "specular")
			{
				ss << specularNr++;
			}

			number = ss.str();*/
			glBindTexture(GL_TEXTURE_2D, textures[i].GetTextureID());
			glUniform1i(glGetUniformLocation(shader_program, ("material." + name).c_str()), i);

			int light_feature_loc = glGetUniformLocation(shader_program, "light_feature.ambient");
			glUniform3fv(light_feature_loc, 1, glm::value_ptr(ambient_color));

			light_feature_loc = glGetUniformLocation(shader_program, "light_feature.diffuse");
			glUniform3fv(light_feature_loc, 1, glm::value_ptr(diffuse_color));

			light_feature_loc = glGetUniformLocation(shader_program, "light_feature.specular");
			glUniform3fv(light_feature_loc, 1, glm::value_ptr(specular_color));

			int light_shininess_loc = glGetUniformLocation(shader_program, "material.shininess");
			glUniform1i(light_shininess_loc, shininess);
			
		}
	}

	void Draw(Shader_Object* shader)
	{
		this->Render_Textures(shader->get_shader_program(), this->textures);
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

private:
	GLuint VAO = 0, VBO = 0, EBO = 0;

	void SetupMesh()
	{
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));

		glBindVertexArray(0);
	}
};