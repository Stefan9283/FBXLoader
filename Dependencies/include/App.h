#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include "Common.h"
#include "Model.h"

#include "Window.h"
#include "Camera.h"

#include "Structs.h"

#define SHOWLOADEDTEXFEEDBACK false

class App
{
public:
	std::vector<Texture> LoadedTextures;
	std::vector<Model*> Objects;
	Window *w;
	std::vector<Material> Materials;

	App() { 
	
		w = new Window;

		Objects.push_back(ReadFBX("obj/cow/cow3.fbx"));

	}

	~App()
	{
		for(auto obj : Objects)
			delete obj;
		delete w;
	}

	void Run()
	{
		Shader* shader = new Shader("Dependencies/shaders/vertex.glsl", "Dependencies/shaders/fragment.glsl");
		shader->bind();

		

		Camera cam(w->window);


		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(w->window, true);
		ImGui_ImplOpenGL3_Init("#version 130");
		ImGui::StyleColorsDark();

		clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		rgb_ls = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		strength = 100000.0f;
		angular_vel = 100;

		Model* light = ReadFBX("obj/helmet.fbx"); ///////////

		if (SHOWLOADEDTEXFEEDBACK)
			std::cout << "Textures that should be loaded\n";
			for (auto i = 0; i < LoadedTextures.size(); i++)
			{
				LoadedTextures[i].id = TextureFromFile(LoadedTextures[i].filename.c_str());
				if (SHOWLOADEDTEXFEEDBACK)
					std::cout << "\t" << LoadedTextures[i].filename.c_str() << " id:" << LoadedTextures[i].id << "\n";
			}
		
		std::chrono::high_resolution_clock::time_point start, end;
		start = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration;
		while (!glfwWindowShouldClose(w->window))
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cam.Move(w->window);
			cam.update_proj(w->window);
			shader->setMat4("view", cam.getviewmatrix());
			shader->setMat4("proj", cam.getprojmatrix());
			shader->setVec3("cameraPos", cam.position);


			end = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
			glm::mat4 orbitTransform = orbit(duration.count());

			glm::vec4 LightPos = glm::vec4(light->meshes[0]->vertices[0].Position, 1.0f);
			LightPos = orbitTransform * light->getModelMatrix() * LightPos;
			//LightPos = light->getModelMatrix() * LightPos;
			glm::vec3 croppedLightPos = glm::vec3(LightPos);
			shader->setVec3("LightPos", croppedLightPos);

			light->Draw(shader, &LoadedTextures, orbitTransform);

			for (auto obj : Objects)
				obj->Draw(shader, &LoadedTextures);

			DisplayIMGUI();

			glfwSwapBuffers(w->window);
			glfwPollEvents();
			
			if (glfwGetKey(w->window, GLFW_KEY_ESCAPE))
				break;

		}
		delete light;
		ImGui::DestroyContext();


		delete shader;

	}
	
	float angular_vel;
	ImVec4 clear_color;
	ImVec4 rgb_ls;
	float strength;

	int MaterialIsLoaded(Material m);

	glm::mat4 orbit(double time)
	{
		glm::mat4 T, R;
		T = glm::translate(glm::mat4(1), glm::vec3(30, 0, 0));
		R = glm::rotate(glm::mat4(1), (float)glm::radians(time) * angular_vel, glm::vec3(0.0f, 1.0f, 0.0f));
		return R * T;
	}

	void DisplayIMGUI()
	{

		// OTHER INPUTS


		// IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{

			ImGui::Begin("SOME TOOLS");
			ImGui::Text("This is some useless text.");


			if (ImGui::CollapsingHeader("Show objects"))
			{

				ImGui::SliderFloat("angular velocity", &angular_vel, -100, 100);

				for (int i = 0; i < Objects.size(); i++)
				{
					std::string obj_identifier = Objects[i]->name + std::to_string(i);

					if (ImGui::CollapsingHeader(obj_identifier.c_str()))
					{
						ImGui::Begin(obj_identifier.c_str());

						ImGui::SliderFloat("tx", &Objects[i]->Position.x, -100, 100);
						ImGui::SliderFloat("ty", &Objects[i]->Position.y, -100, 100);
						ImGui::SliderFloat("tz", &Objects[i]->Position.z, -100, 100);
						ImGui::SliderFloat("rx", &Objects[i]->axis_rotations.x, -90, 90);
						ImGui::SliderFloat("ry", &Objects[i]->axis_rotations.y, -90, 90);
						ImGui::SliderFloat("rz", &Objects[i]->axis_rotations.z, -90, 90);

						ImGui::End();
					}
				}
			}
			if (ImGui::CollapsingHeader("Show Light Sources"))
			{
				bool clear_all_lights = false;
				ImGui::Checkbox("Clear Lights?", &clear_all_lights);

			}


			ImGui::ColorEdit3("Light Source Color", (float*)&rgb_ls);
			ImGui::SliderFloat("Light Strength", &strength, 0.0f, 1000000.0f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			//ImGui::Text("Coords: %f %f %f ", Camera->position.x, Camera->position.y, Camera->position.z);

			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	}

	
	unsigned int TextureFromFile(const char* name)
	{
		unsigned int texture_id = 0;
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
		else {
			std::cout << "Texture " << name << " can not be opened\n";
			assert(0);
		}
		stbi_image_free(data);

		return texture_id;
	}
	int App::isAlreadyLoaded(Texture tex);

	Model* ReadFBX(const char* path);
	Mesh* getMeshData(FbxNode* pNode);
};