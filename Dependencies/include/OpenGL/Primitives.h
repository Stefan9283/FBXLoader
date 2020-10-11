#pragma once
#include "Common.h"

class Line
{

public:
	glm::vec3 color;
	Line() {}

	Line(glm::vec3 start, glm::vec3 end, glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f))
	{
		this->color = color;
		float somevalue = 0.01f;
		float vertices[] = {
			///	 x								y			    z	  
				 start.x + somevalue, start.y + somevalue, start.z,// 0 
				 start.x + somevalue, start.y - somevalue, start.z,// 1 
				 start.x - somevalue, start.y - somevalue, start.z,// 2 
				 start.x - somevalue, start.y + somevalue, start.z,// 3 
				   end.x + somevalue,   end.y + somevalue,   end.z,// 4 
				   end.x + somevalue,   end.y - somevalue,   end.z,// 5 
				   end.x - somevalue,   end.y - somevalue,   end.z,// 6 
				   end.x - somevalue,   end.y + somevalue,   end.z // 7 
		};
		unsigned int indices[] = {
			4, 6, 5,
			4, 7, 6,
			3, 7, 4,
			4, 0, 3,
			0, 4, 5,
			5, 1, 0,
			7, 3, 2,
			2, 6, 7,
			5, 6, 2,
			2, 1, 5,
			0, 1, 2,
			2, 3, 0
		};


		indicesCount = (sizeof(indices) / sizeof(unsigned int));

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

		unsigned int strideSize = 3 * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideSize, (void*)0);


		glBindVertexArray(0);

	}

	~Line()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void Draw(Shader* shader, Camera* camera)
	{
		glm::mat4 identity = glm::identity<glm::mat4>();
		shader->setMat4("model", &identity);
		shader->setMat4("proj", camera->getprojmatrix());
		shader->setMat4("view", camera->getviewmatrix());
		shader->setVec3("color", this->color);
		shader->bind();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)indicesCount, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}


protected:
	unsigned int VAO, VBO, EBO;
	unsigned int indicesCount;

};
class Ray : public Line
{
protected:
	glm::vec3 direction, origin;
	float length;
	Line* body;
public:

	Ray(glm::vec3 origin, glm::vec3 direction, float length)
	{
		this->origin = origin;
		this->direction = direction;
		this->length = length;
		this->body = new Line(origin, origin + direction * length, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	~Ray() { delete this->body; }
	void Draw(Shader* shader, Camera* camera)
	{
		this->body->Draw(shader, camera);
	}
};

class Quad
{
public:
	unsigned int VAO, VBO, EBO;
	unsigned int indicesCount;
	Quad()
	{
		createQuad();
	}
	void createQuad()
	{
		float vertices[] = {
			///	 x		y	   z		tx	  ty		nx	  ny		nz
				 1.0f,  1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
				 1.0f, -1.0f, 2.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,
				-1.0f,  1.0f, 2.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,
				-1.0f, -1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f
		};
		unsigned int indices[] = {
			0, 2, 1,
			1, 2, 3
		};


		indicesCount = (sizeof(indices) / sizeof(unsigned int));

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

		unsigned int strideSize = 8 * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideSize, (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideSize, (void*)(5 * sizeof(float)));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, strideSize, (void*)(3 * sizeof(float)));

		glBindVertexArray(0);
	}
	~Quad()
	{
		destroyQuad();
	}
	void destroyQuad()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	//virtual void Draw(Shader* shader) = 0;

};



#define QUAD_DRAW_CONST 0
#define QUAD_DRAW_DEPTH 1
#define QUAD_DRAW_COLOR 2
class ShadowQuad : public Quad
{
private:
	unsigned int depthMapFBO, depthMap;
	unsigned int rgbMapFBO, rgbMap;
	const unsigned int RGB_WIDTH = 1024, RGB_HEIGHT = 1024;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

public:
	void GenDepthMap(std::vector<Model*>* Objects, glm::vec3 position, Shader* shader, GLFWwindow* window)
	{
		///std::cout << "Fac ceva\n";
		// 1. first render to depth map

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// set matrices here
		float near_plane = 1.0f, far_plane = 40.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(
			position,
			glm::vec3(0.0f, 0.0f, 0.0f),  // center
			glm::vec3(0.0f, 1.0f, 0.0f)); // up
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		shader->setMat4("lightSpaceMatrix", &lightSpaceMatrix);
		// set matrices here

		for (auto obj : *Objects)
			obj->DrawNoTex(shader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		int w, h;
		glfwGetWindowSize(window, &w, &h);
		glViewport(0, 0, w, h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}
	void GenColorMap(std::vector<Model*>* Objects, Camera* cam, Shader* shader, GLFWwindow* window)
	{
		// 1. first render to depth map

		glViewport(0, 0, RGB_WIDTH, RGB_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, rgbMapFBO);
		glClear(GL_COLOR_BUFFER_BIT);

		// set matrices here
		float near_plane = 1.0f, far_plane = 40.0f;
		glm::mat4 lightProjection = *cam->getprojmatrix(); //glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		glm::mat4 lightView = *cam->getviewmatrix();
			/*glm::lookAt(
			cam->position,
			cam->position + cam->goFront,  // center
			cam->goUp); // up*/
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		shader->setMat4("lightSpaceMatrix", &lightSpaceMatrix);
		// set matrices here


		unsigned long long int max = RAND_MAX;
		double tens = 1;
		while (max)
		{
			max /= 10;
			tens *= 10;
		}

		srand(time(NULL));

		//std::cout << (float)rand() / tens << " " << (float)rand() / tens << " " << (float)rand() / tens << "\n";

		for (auto obj : *Objects)
		{
			glm::vec3 color = glm::vec3((float)rand() / tens, (float)rand() / tens, (float)rand() / tens);
			//std::cout << glm::to_string(color) << "\n";
			shader->setVec3("color", color);
			obj->DrawNoTex(shader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		int w, h;
		glfwGetWindowSize(window, &w, &h);
		glViewport(0, 0, w, h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}
	
	Model* getSelected(std::vector<Model*>* Objects, Camera* cam, Shader* shader, GLFWwindow* window)
	{

		glViewport(0, 0, RGB_WIDTH, RGB_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, rgbMapFBO);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 lightProjection = *cam->getprojmatrix();
		glm::mat4 lightView = *cam->getviewmatrix();
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		shader->setMat4("lightSpaceMatrix", &lightSpaceMatrix);


		unsigned long long int max = RAND_MAX;
		float tens = 1;
		while (max)
		{
			max /= 10;
			tens *= 10;
		}

		srand(time(NULL));

		std::vector<std::pair<glm::vec3, int>> color4Obj;

		for (auto i=0;i<(*Objects).size(); i++)
		{
			glm::vec3 color = glm::vec3(1);

			bool doesntExist = false;
			while (!doesntExist)
			{
				doesntExist = true;
				color = glm::vec3((float)rand() / tens, (float)rand() / tens, (float)rand() / tens);
				for (int i = 0; i < color4Obj.size(); i++)
					if (color4Obj[i].first == color)
						doesntExist = false;
			}

			//std::cout << glm::to_string(color) << "\n";

			color4Obj.push_back(std::make_pair(color, i));

			shader->setVec3("color", color);
			(*Objects)[i]->DrawNoTex(shader);
		}


		

		int w, h;
		glfwGetWindowSize(window, &w, &h);


		float x = cam->lastx_mouse;// * RGB_WIDTH / (float)w;
		float y = h - cam->lasty_mouse;// * RGB_HEIGHT / (float)h;

		float data[4];
		glReadPixels((float)x, (float)y, 1, 1, GL_RGB, GL_FLOAT, data);
		//std::cout << data[0] << " " << data[1] << " " << data[2] << " " << data[3] << "\n";
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glViewport(0, 0, w, h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float eps = 0.01f;
		for (auto pair : color4Obj)
		{
			if ( abs(pair.first.x - data[0]) <= eps && abs(pair.first.y - data[1]) <= eps && abs(pair.first.z - data[2]) <= eps)
				return (*Objects)[pair.second];
		}

		return NULL;

	}
	void Draw(Shader* shader, int mode)
	{
		glm::mat4 identity = glm::identity<glm::mat4>();
		shader->setMat4("model", &identity);
		shader->setMat4("mesh_model", &identity);


		glActiveTexture(GL_TEXTURE0);
		shader->setInt("DebugTexture", 0);
		if(mode == 1)
			glBindTexture(GL_TEXTURE_2D, depthMap);
		else if (mode == 2)
			glBindTexture(GL_TEXTURE_2D, rgbMap);
		shader->setInt("DebugTexMode", mode);


		shader->bind();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)indicesCount, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		shader->setInt("DebugTexMode", 0);

	}

	ShadowQuad()
	{
		createQuad();

		// depth 
		glGenFramebuffers(1, &depthMapFBO);

		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// rgb
		glGenFramebuffers(1, &rgbMapFBO);

		glGenTextures(1, &rgbMap);
		glBindTexture(GL_TEXTURE_2D, rgbMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			RGB_WIDTH, RGB_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, rgbMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rgbMap, 0);
		//glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	~ShadowQuad() { destroyQuad(); glDeleteFramebuffers(1, &depthMapFBO); glDeleteFramebuffers(1, &rgbMapFBO); }
};

