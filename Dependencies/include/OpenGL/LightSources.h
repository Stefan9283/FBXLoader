#pragma once

#include "Common.h"
#include "Model.h"

class LightSource
{
public:
	Model* body;
	glm::vec3 color, diffuse, ambient, specular, position;


	LightSource()
	{
		body = NULL;
		color = glm::vec3(1.0f);
		diffuse = glm::vec3(0.5f);
		ambient = glm::vec3(0.0001f);
		specular = glm::vec3(0.001f);
	}
	~LightSource()
	{
		if (body)
			delete body;
	}
};

class StaticLightSource : public LightSource
{
public:
	StaticLightSource(){}
	~StaticLightSource(){}
	
	void Draw(double time, Shader* shader, std::vector<Texture>* textures, std::vector<Material>* materials)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1), position);
		body->Draw(shader, textures, materials, transform);
	}

};
class OrbitingLightSource : public LightSource
{
public:
	glm::vec3 origin; 
	float radius, angularVel;
	
	OrbitingLightSource()
	{
		this->radius = 30.0f;
		this->origin = glm::vec3(0.0f);
		angularVel = 100.0f;
	}
	~OrbitingLightSource(){}

	void Draw(double time, Shader * shader, std::vector<Texture>* textures, std::vector<Material>* materials)
	{
		glm::mat4 transform = orbit(time);
		position = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		body->Draw(shader, textures, materials, transform);
	}

	glm::mat4 orbit(double time)
	{
		glm::mat4 T, R;
		T = glm::translate(glm::mat4(1), glm::vec3(radius, 0, 0));
		R = glm::rotate(glm::mat4(1), (float)glm::radians(time) * angularVel, glm::vec3(0.0f, 1.0f, 0.0f));
		return R * T;
	}

};
