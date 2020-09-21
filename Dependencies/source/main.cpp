

#include "Common.h"


#include "Model.h"
#include "Shader.h"
#include "Camera.h"

#include "Window.h"

int main()
{
    Window w;
    
    Shader* shader = new Shader("Dependencies/shaders/vertex.glsl", "Dependencies/shaders/fragment.glsl");
    shader->bind();


    Model* Obj = ReadFBX("cow2.fbx");


    
    Camera cam(w.window);

    glm::mat4* proj, * view;


    while (!glfwWindowShouldClose(w.window))
    {
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        cam.Move(w.window);
        view = cam.getviewmatrix();
        cam.update_proj(w.window);
        proj = cam.getprojmatrix();
        shader->setMat4("view", view);
        shader->setMat4("proj", proj);
        shader->setVec3("cameraPos", cam.position);

        Obj->Draw(shader);

        glfwSwapBuffers(w.window);
        glfwPollEvents();
        if (glfwGetKey(w.window, GLFW_KEY_ESCAPE))
            break;
    }   
   


    delete shader;
    delete Obj;

   
    return 0;
}
