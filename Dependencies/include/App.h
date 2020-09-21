#pragma once

#include "Common.h"
#include "Model.h"

#include "Window.h"
#include "Camera.h"

struct Texture
{
	std::string filename;
};

class App
{
public:
	std::vector<Texture> LoadedTextures;
	std::vector<Model*> Objects;
	Window w;


	App() { Objects.push_back(ReadFBX("cow3.fbx")); }

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
				obj->Draw(shader);

			glfwSwapBuffers(w.window);
			glfwPollEvents();
			if (glfwGetKey(w.window, GLFW_KEY_ESCAPE))
				break;
		}



		delete shader;

	}

	int App::isAlreadyLoaded(Texture tex);

	Model* ReadFBX(const char* path);
	Mesh* getMeshData(FbxNode* pNode);
};