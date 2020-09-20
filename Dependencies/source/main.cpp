
#include "Common.h"


#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    int w = 1000;
    int h = w;
   

    GLFWwindow* window = glfwCreateWindow(w, h, "cow.exe", NULL, NULL);
    
    if (!window)
    {
        std::cout << "Window failed to initialize\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }
    else std::cout << "OpenGL was initialized just fine\n";

    
    glfwSwapInterval(1);

    int width = 500, height = 500;

    glfwGetWindowSize(window, &width, &height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    
    Shader* shader = new Shader("Dependencies/shaders/vertex.glsl", "Dependencies/shaders/fragment.glsl");
    shader->bind();


    Model* Obj = ReadFBX("SKELL.fbx");

    
    Camera cam(window);

    glm::mat4* proj, * view;

    glm::mat4 model = glm::mat4(1);
    shader->setMat4("model", &model);

    while (!glfwWindowShouldClose(window))
    {
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        cam.Move(window);
        view = cam.getviewmatrix();
        proj = cam.getprojmatrix();
        shader->setMat4("view", view);
        shader->setMat4("proj", proj);
        shader->setVec3("cameraPos", cam.position);

        Obj->Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))
            break;
    }   
   
    glfwDestroyWindow(window);
    glfwTerminate();



    delete shader;
    delete Obj;

   
    return 0;
}