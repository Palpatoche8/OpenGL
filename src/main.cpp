#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

#include "Shader.hpp"
#include "ParticleSystem.hpp"
#include "Camera.hpp"

static void errorCallback(int, const char* desc) {
    std::cerr << "GLFW error: " << desc << '\n';
}

static void keyCallback(GLFWwindow* w, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(w, GLFW_TRUE);
}

static void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    Camera *camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
    camera->mouseCallback(window, xpos, ypos);
}


int main() {
    Camera camera = Camera();

    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Fire Simulation", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return 1;
    }

    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';

    Shader shader("shaders/fire.vert", "shaders/fire.frag");
    ParticleSystem particles;

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);           // additive blending for glow

    glEnable(GL_PROGRAM_POINT_SIZE);       // let vertex shader set gl_PointSize
    glEnable(GL_DEPTH_TEST);

    double prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float  dt  = (float)(now - prevTime);
        prevTime   = now;

        camera.processInput(window, dt);

        particles.update(dt);
        particles.uploadToGPU();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        // Pass window dimensions so the vertex shader can scale point sizes correctly
        shader.setVec2("uResolution", (float)w, (float)h);

        Mat4 model = identity();
        Mat4 view = lookAt(
            camera.pos,
            camera.pos + camera.front,
            camera.up
        );
        Mat4 projection = perspective(
            45.0f,
            (float)w / (float)h,
            0.1f,
            100.0f
        );

        shader.setMat4("uModel", model.m);
        shader.setMat4("uView", view.m);
        shader.setMat4("uProjection", projection.m);

        glDepthMask(GL_FALSE);
        glDepthMask(GL_FALSE);
        glBindVertexArray(particles.vao());
        glDrawArrays(GL_POINTS, 0, particles.liveCount());
        glDepthMask(GL_TRUE);
        glDepthMask(GL_TRUE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
