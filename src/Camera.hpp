#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>

#include "Math.hpp"

float radians(float degrees)
{
    return degrees * M_PI / 180.f;
}

struct Camera
{
    Vec3 pos;
    Vec3 front;
    Vec3 up;

    float yaw;
    float pitch;

    bool firstMouse;
    double lastMouseX;
    double lastMouseY;

    Camera()
        : pos(0.0f, 0.0f, 3.0f)
        , front(0.0f, 0.0f, -1.0f)
        , up(0.0f, 1.0f, 0.0f)
        , yaw(-90.0f)
        , pitch(0.0f)
        , firstMouse(true)
        , lastMouseX(400.0)
        , lastMouseY(300.0)
    {}

    void mouseCallback(GLFWwindow *, double xpos, double ypos)
    {
        if (firstMouse)
        {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;
        }

        float xoffset = (float)(xpos - lastMouseX);
        float yoffset = (float)(lastMouseY - ypos);

        lastMouseX = xpos;
        lastMouseY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        front.x = std::cos(radians(yaw)) * std::cos(radians(pitch));
        front.y = std::sin(radians(pitch));
        front.z = std::sin(radians(yaw)) * std::cos(radians(pitch));

        front = front.normalized();
    }

    void processInput(GLFWwindow *window, float dt)
    {
        float speed = 2.0f * dt;

        Vec3 forward(front.x, 0.0f, front.z);
        forward = forward.normalized();

        Vec3 right = forward.cross(up).normalized();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            pos += forward * speed;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            pos += forward * -speed;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            pos += right * -speed;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            pos += right * speed;
    }
};

#endif // CAMERA_HPP