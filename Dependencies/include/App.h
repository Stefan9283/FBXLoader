#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define SHOWLOADEDTEXFEEDBACK false

#include "Common.h"
#include "Model.h"

#include "Window.h"
#include "Camera.h"

#include "Structs.h"
#include "LightSources.h"

class App
{
public:
	std::vector<Texture> LoadedTextures;
	std::vector<Model*> Objects;
	std::vector<StaticLightSource*> StatLights;
	std::vector<OrbitingLightSource*> OrbLights;
	Window w;
	Camera* cam;
	Shader* shader;

	std::vector<Material> Materials;

	Bone* SkellyBoi;
	std::vector<Bone*> Bones;

	App() { 
	
		SkellyBoi = NULL;
		clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		rgb_ls = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		strength = 100000.0f;

		Objects.push_back(ReadFBX("obj/ROBOT.fbx"));
		//createOrbitingLightSource("obj/helmet.fbx");


		shader = new Shader("Dependencies/shaders/vertex.glsl", "Dependencies/shaders/fragment.glsl");



		cam = new Camera(w.window);


		if (SHOWLOADEDTEXFEEDBACK)
			std::cout << "Textures that should be loaded\n";
		for (auto i = 0; i < LoadedTextures.size(); i++)
		{
			LoadedTextures[i].id = TextureFromFile(LoadedTextures[i].filename.c_str());
			if (SHOWLOADEDTEXFEEDBACK)
				std::cout << "\t" << LoadedTextures[i].filename.c_str() << " id:" << LoadedTextures[i].id << "\n";
		}

		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(w.window, true);
		ImGui_ImplOpenGL3_Init("#version 130");
		ImGui::StyleColorsDark();
	}

	~App()
	{
		ImGui::DestroyContext();

		for(auto obj : Objects)
			delete obj;
		for (auto light : StatLights)
			delete light;
		for (auto light : OrbLights)
			delete light;
		delete shader;
		delete cam;

	}

	void Run()
	{

		std::chrono::high_resolution_clock::time_point start, end;
		start = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration;
		
		while (!glfwWindowShouldClose(w.window))
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cam->Move(w.window);
			cam->update_proj(w.window);
			shader->setMat4("view", cam->getviewmatrix());
			shader->setMat4("proj", cam->getprojmatrix());
			shader->setVec3("cameraPos", cam->position);


			end = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
			
			DrawEverything(shader, duration.count());

			DisplayIMGUI();

			glfwSwapBuffers(w.window);
			glfwPollEvents();
			
			if (glfwGetKey(w.window, GLFW_KEY_ESCAPE))
				break;

		}

	}
	
	ImVec4 clear_color;
	ImVec4 rgb_ls;
	float strength;


	// Debug/Testing GUI
	void DisplayIMGUI()
	{

		// OTHER INPUTS?


		// IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{

			ImGui::Begin("SOME TOOLS");
			ImGui::Text("This is some useless text.");


			if (ImGui::CollapsingHeader("Show objects"))
			{


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


			if (ImGui::CollapsingHeader("Show Lights"))
			{
				if (ImGui::CollapsingHeader("Orbital"))
				{
					for (int i = 0; i < OrbLights.size(); i++)
					{
						std::string obj_identifier = OrbLights[i]->body->name + std::to_string(i);

						if (ImGui::CollapsingHeader(obj_identifier.c_str()))
						{
							ImGui::Begin(obj_identifier.c_str());

							rgb_ls = ImVec4(OrbLights[i]->color.x, OrbLights[i]->color.y, OrbLights[i]->color.z, 1.0f);
							ImGui::ColorEdit3("Light Source Color", (float*)&rgb_ls);
							ImGui::SliderFloat("tx", &OrbLights[i]->body->Position.x, -100, 100);
							ImGui::SliderFloat("ty", &OrbLights[i]->body->Position.y, -100, 100);
							ImGui::SliderFloat("tz", &OrbLights[i]->body->Position.z, -100, 100);
							ImGui::SliderFloat("rx", &OrbLights[i]->body->axis_rotations.x, -90, 90);
							ImGui::SliderFloat("ry", &OrbLights[i]->body->axis_rotations.y, -90, 90);
							ImGui::SliderFloat("rz", &OrbLights[i]->body->axis_rotations.z, -90, 90);
							ImGui::SliderFloat("angular velocity", &OrbLights[i]->angularVel, -90, 90);
							ImGui::SliderFloat("radius", &OrbLights[i]->radius, -90, 90);
							OrbLights[i]->color = glm::vec3(rgb_ls.x, rgb_ls.y, rgb_ls.z);
							ImGui::End();
						}
					}
				}
				if (ImGui::CollapsingHeader("Static"))
				{

				}

			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			/*
			if (ImGui::CollapsingHeader("Show Light Sources"))
			{
				bool clear_all_lights = false;
				ImGui::Checkbox("Clear Lights?", &clear_all_lights);
			}
			ImGui::SliderFloat("Light Strength", &strength, 0.0f, 1000000.0f);
			//ImGui::Text("Coords: %f %f %f ", Camera->position.x, Camera->position.y, Camera->position.z);
			*/

		
			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	}

	// Model Reading
	Mesh* App::getMeshData(FbxMesh* mesh, int material_index);
	void App::recursiveReadMeshes(FbxNode* node, std::vector<Mesh*>* meshes);
	Model* ReadFBX(const char* path);

	// Tex/Mat Funcs
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
	int App::TexturelIsLoaded(Texture tex);
	int MaterialIsLoaded(Material m);

	void DrawEverything(Shader* shader, double time)
	{
		sendLightData(shader, time);
		for (auto obj : Objects)
			obj->Draw(shader, &LoadedTextures, &Materials);
		for (auto light : StatLights)
			light->Draw(time, shader, &LoadedTextures, &Materials);
		for (auto light : OrbLights)
			light->Draw(time, shader, &LoadedTextures, &Materials);
	}
	void sendLightData(Shader* shader, double time)
	{
		int index = 0;
		for (auto light : StatLights)
		{
			
			std::string default = "lights[";	
			default.append(std::to_string(index));
			default.append("].");


			std::string position = default;
			position.append("position");

			shader->setVec3(position.c_str(), light->position);

			std::string diffuse = default;
			diffuse.append("diffuse");

			shader->setVec3(diffuse.c_str(), light->color);
			index++;
		}

		for (auto light : OrbLights)
		{
			std::string default = "lights[";
			default.append(std::to_string(index));
			default.append("].");


			std::string position = default;
			position.append("position");

			glm::vec3 coord = glm::vec3(light->orbit(time) * glm::vec4(light->origin, 1.0f));

			shader->setVec3(position.c_str(), coord);

			std::string diffuse = default;
			diffuse.append("diffuse");

			shader->setVec3(diffuse.c_str(), light->color);
			index++;
		}

		shader->setInt("NUM_OF_LIGHTS", index);

	}
	void createStaticLightSource(const char* path, glm::vec3 position = glm::vec3(0.0f))
	{
		StaticLightSource* StaticLight = new StaticLightSource;
		StaticLight->body = ReadFBX(path);
		StaticLight->position = position;
		StatLights.push_back(StaticLight);
	}
	void createOrbitingLightSource(const char* path, float radius = 3.0f, glm::vec3 origin = glm::vec3(0.0f))
	{
		OrbitingLightSource* OrbitingLight = new OrbitingLightSource;
		OrbitingLight->body = ReadFBX(path);
		OrbitingLight->origin = origin;
		OrbLights.push_back(OrbitingLight);
	}


	// LEGACY FUNC
	Mesh* extractNodeMesh(FbxNode* pNode);
};