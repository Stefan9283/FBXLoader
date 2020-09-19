#pragma once

#include "Common.h"

class Camera
{
public:
	glm::vec3 goRight, goUp, goFront;
    float speed, mouse_sensitivity;
    double lastx_mouse, lasty_mouse;
    float yaw, pitch, roll;
	glm::vec3 position, up;
    glm::mat4 proj, view;
    Camera(GLFWwindow* window)
    {
        glfwGetCursorPos(window, &lastx_mouse, &lasty_mouse);

        speed = 0.5f;
        mouse_sensitivity = 0.05f;

        reset_camera();

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        int w, h;
        glfwGetWindowSize(window, &w, &h);

        update_proj(h, w);
        update_view();
    }

    ~Camera() {}

    void reset_camera()
    {
        position = glm::vec3(0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);

        yaw = -90.0f;
        pitch = 0.0f;
        roll = 0.0f;

        goFront = glm::vec3(0.0f, 0.0f, -1.0f);
        goUp = glm::vec3(0.0f, 1.0f, 0.0f);
        goRight = glm::vec3(-1.0f, 0.0f, 0.0f);
    }


    void update_view()
    {
        view = glm::lookAt(position, position + goFront, goUp);
    }
    glm::mat4* getviewmatrix()
    {
        return &view;
    }
    void update_proj(int height, int width)
    {
        proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10000.0f);
    }
    glm::mat4* getprojmatrix()
    {
        return &proj;
    }

    void Move(GLFWwindow* window)
    {

        double current_x = lastx_mouse, current_y = lasty_mouse; 
        glfwGetCursorPos(window, &lastx_mouse, &lasty_mouse);

        float offsetx = 0, offsety = 0;
        if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)))
        {
            offsetx = float(lastx_mouse - current_x);
            offsety = float(current_y - lasty_mouse);
        }


        if (offsetx || offsety)
        {

            offsetx *= mouse_sensitivity;
            offsety *= mouse_sensitivity;

            yaw += offsetx;
            pitch += offsety;

            goFront = glm::normalize(goFront * (float)cos(glm::radians(offsetx)) - goRight * (float)sin(glm::radians(offsetx)));
            goFront = glm::normalize(goFront);

            goFront = glm::normalize(goFront * (float)cos(glm::radians(offsety)) + goUp * (float)sin(glm::radians(offsety)));
            goFront = glm::normalize(goFront);

            goRight = glm::normalize(glm::cross(goUp, goFront));


            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;


        }
        update_view();



        // close window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        // front back
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            position += speed * goFront;

        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            position -= speed * goFront;
        }


        // right left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            position += speed * goRight;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            position -= speed * goRight;
        }

        // up down
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            position += speed * goUp;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            position -= speed * goUp;
        }

        // reset
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            reset_camera();

        // slow down
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            speed = 0.5f;
        // speed up
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            speed = 2;

    }


};