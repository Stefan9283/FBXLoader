#pragma once

#include "Common.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class Window
{
public:
    GLFWwindow* window;

    Window()
    {
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

        if (!glfwInit())
            throw std::runtime_error("glfw didn't init\n");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        int w = 1000;
        int h = w;


        window = glfwCreateWindow(w, h, "cow.exe", NULL, NULL);

        if (!window)
        {
            std::cout << "Window failed to initialize\n";
            glfwTerminate();
            throw std::runtime_error("window is null\n");
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGL()) {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
            throw std::runtime_error("OpenGL didn't init\n");
        }
        else std::cout << "OpenGL was initialized just fine\n";


        glfwSwapInterval(1);

        int width = 500, height = 500;

        glfwGetWindowSize(window, &width, &height);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        srand((unsigned)time(NULL));

        glm::vec3 clearColor;
        clearColor.x = (float)rand() / RAND_MAX;
        clearColor.y = (float)rand() / RAND_MAX;
        clearColor.z = (float)rand() / RAND_MAX;


        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glfwWindowHint(GLFW_SAMPLES, 8);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_CULL_FACE);


    }

    ~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }


};