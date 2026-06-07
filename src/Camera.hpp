#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Math.hpp"
#include <cmath>

struct Camera {
    Vec3  pos   = {0.f, 0.f,  4.f};
    Vec3  front = {0.f, 0.f, -1.f};
    Vec3  up    = {0.f, 1.f,  0.f};

    float yaw   = -90.f;  
    float pitch =  -8.f;  

    float speed      = 3.0f;
    float sensitivity = 0.12f;

    static constexpr float kFov = 0.9f;   // radians (~51°)

    // Call after creating the GLFW window
    void init(GLFWwindow* w) {
        glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        updateVectors();
    }

    Mat4 view() const {
        return Mat4::lookAt(pos, pos + front, up);
    }

    Mat4 proj(float aspect) const {
        return Mat4::perspective(kFov, aspect, 0.05f, 200.f);
    }

    float pointScale(float screenH) const {
        return screenH / (2.f * std::tan(kFov * 0.5f));
    }

    void processInput(GLFWwindow* w, float dt) {
        float v = speed * dt;
        Vec3  right = front.cross(up).normalized();

        if (glfwGetKey(w, GLFW_KEY_W)     == GLFW_PRESS) pos += front * v;
        if (glfwGetKey(w, GLFW_KEY_S)     == GLFW_PRESS) pos -= front * v;
        if (glfwGetKey(w, GLFW_KEY_A)     == GLFW_PRESS) pos -= right * v;
        if (glfwGetKey(w, GLFW_KEY_D)     == GLFW_PRESS) pos += right * v;
        if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS) pos.y += v;
        if (glfwGetKey(w, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) pos.y -= v;
    }

    void mouseCallback(GLFWwindow*, double xpos, double ypos) {
        if (m_firstMouse) {
            m_lastX = (float)xpos;
            m_lastY = (float)ypos;
            m_firstMouse = false;
        }
        float dx = ((float)xpos - m_lastX) * sensitivity;
        float dy = (m_lastY - (float)ypos) * sensitivity;  // inverted Y
        m_lastX = (float)xpos;
        m_lastY = (float)ypos;

        yaw   += dx;
        pitch += dy;
        if (pitch >  89.f) pitch =  89.f;
        if (pitch < -89.f) pitch = -89.f;

        updateVectors();
    }

private:
    float m_lastX = 400.f, m_lastY = 300.f;
    bool  m_firstMouse = true;

    void updateVectors() {
        float yr = yaw   * 3.14159265f / 180.f;
        float pr = pitch * 3.14159265f / 180.f;
        front = Vec3(std::cos(pr) * std::cos(yr),
                     std::sin(pr),
                     std::cos(pr) * std::sin(yr)).normalized();
    }
};
