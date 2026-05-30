#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

#include "ParticleSystem.hpp"
#include "Shader.hpp"

static void errorCallback(int, const char *desc)
{
    std::cerr << "GLFW error: " << desc << '\n';
}

static void keyCallback(GLFWwindow *w, int key, int, int action, int)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(w, GLFW_TRUE);
}

int main()
{
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window =
        glfwCreateWindow(800, 600, "Fire Simulation", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to init GLAD\n";
        return 1;
    }

    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';

    Shader shader("shaders/fire.vert", "shaders/fire.frag");
    ParticleSystem particles;

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); // additive blending for glow

    glEnable(GL_PROGRAM_POINT_SIZE); // let vertex shader set gl_PointSize

    double prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float dt = (float)(now - prevTime);
        prevTime = now;

        particles.update(dt);
        particles.uploadToGPU();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        // Pass window dimensions so the vertex shader can scale point sizes
        // correctly
        shader.setVec2("uResolution", (float)w, (float)h);

        glBindVertexArray(particles.vao());
        glDrawArrays(GL_POINTS, 0, particles.liveCount());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
