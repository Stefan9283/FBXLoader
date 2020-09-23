#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include "Common.h"
#include "Model.h"

#include "Window.h"
#include "Camera.h"

#include "Structs.h"





class App
{
public:
	std::vector<Texture> LoadedTextures;
	std::vector<Model*> Objects;
	Window w;


	App() { 
	
		Objects.push_back(ReadFBX("obj/helmet.fbx"));
		for (auto tex : LoadedTextures)
			tex.id = TextureFromFile(tex.filename.c_str());
	}

	~App()
	{
		for(auto obj : Objects)
			delete obj;
	}

	void Run()
	{
		Shader* shader = new Shader("Dependencies/shaders/vertex.glsl", "Dependencies/shaders/fragment.glsl");
		shader->bind();


		Camera cam(w.window);

		std::cout << "Textures that should be loaded\n";
		for (auto texture : LoadedTextures)
			std::cout << "\t" << texture.filename.c_str() << "\n";

		while (!glfwWindowShouldClose(w.window))
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cam.Move(w.window);
			cam.update_proj(w.window);
			shader->setMat4("view", cam.getviewmatrix());
			shader->setMat4("proj", cam.getprojmatrix());
			shader->setVec3("cameraPos", cam.position);

			for (auto obj : Objects)
				obj->Draw(shader, &LoadedTextures);

			glfwSwapBuffers(w.window);
			glfwPollEvents();
			//if (glfwGetKey(w.window, GLFW_KEY_ESCAPE))
				break;
		}



		delete shader;

	}
	
	unsigned int TextureFromFile(const char* name)
	{
		unsigned int texture_id;
		stbi_set_flip_vertically_on_load(true);
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		int width, height, nrChannels;

		stbi_uc* data = stbi_load(name, &width, &height, &nrChannels, 0);


		if (data)
		{
			if (nrChannels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else if (nrChannels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		return texture_id;
	}
	int App::isAlreadyLoaded(Texture tex);

	Model* ReadFBX(const char* path);
	Mesh* getMeshData(FbxNode* pNode);
};