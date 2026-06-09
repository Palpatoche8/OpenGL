#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cmath>
#include <filesystem>
#include <vector>

#include "Shader.hpp"
#include "Camera.hpp"
#include "PointLight.hpp"
#include "ParticleSystem.hpp"
#include "SparkSystem.hpp"
#include "SmokeSystem.hpp"
#include "Skybox.hpp"
#include "Mesh.hpp"

static Camera g_cam;
static bool g_torchOn = false;

static void errorCB(int, const char* d) { std::cerr << "GLFW: " << d << '\n'; }

static void keyCB(GLFWwindow* w, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(w, GLFW_TRUE);
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        g_torchOn = !g_torchOn;
}

static void mouseCB(GLFWwindow* w, double xpos, double ypos) {
    g_cam.mouseCallback(w, xpos, ypos);
}

static constexpr int EMBER_COUNT = 28;
static GLuint g_emberVao, g_emberVbo;

static void initEmbers() {
    static constexpr float EMBER_BASE_Y = -1.4f;
    float data[EMBER_COUNT * 3];
    srand(42);
    for (int i = 0; i < EMBER_COUNT; ++i) {
        float angle = (float)i / EMBER_COUNT * 6.2832f + ((float)rand()/RAND_MAX) * 0.4f;
        float r     = 0.06f + ((float)rand()/RAND_MAX) * 0.18f;
        data[i*3+0] = r * std::cos(angle);
        data[i*3+1] = EMBER_BASE_Y;
        data[i*3+2] = r * std::sin(angle);
    }
    glGenVertexArrays(1, &g_emberVao);
    glGenBuffers(1, &g_emberVbo);
    glBindVertexArray(g_emberVao);
    glBindBuffer(GL_ARRAY_BUFFER, g_emberVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

struct TorchVertex { float x, y, z, nx, ny, nz; };
static GLuint g_torchVao, g_torchVbo;
static int    g_torchVertCount;

static std::vector<TorchVertex> makeCylinder(float radius, float height, int slices) {
    std::vector<TorchVertex> verts;
    for (int i = 0; i < slices; ++i) {
        float a0 = (float)i       / slices * 2.f * 3.14159f;
        float a1 = (float)(i + 1) / slices * 2.f * 3.14159f;
        float x0 = std::cos(a0), z0 = std::sin(a0);
        float x1 = std::cos(a1), z1 = std::sin(a1);
        float y0 = -height / 2.f, y1 = height / 2.f;
        verts.push_back({x0*radius, y0, z0*radius, x0, 0, z0});
        verts.push_back({x1*radius, y0, z1*radius, x1, 0, z1});
        verts.push_back({x1*radius, y1, z1*radius, x1, 0, z1});
        verts.push_back({x0*radius, y0, z0*radius, x0, 0, z0});
        verts.push_back({x1*radius, y1, z1*radius, x1, 0, z1});
        verts.push_back({x0*radius, y1, z0*radius, x0, 0, z0});
    }
    return verts;
}

static void initTorch() {
    auto handle = makeCylinder(0.025f, 0.35f, 12);
    auto wrap   = makeCylinder(0.038f, 0.06f, 12);
    float wrapOffset = 0.35f / 2.f + 0.06f / 2.f;
    for (auto& v : wrap) v.y += wrapOffset;
    std::vector<TorchVertex> all;
    all.insert(all.end(), handle.begin(), handle.end());
    all.insert(all.end(), wrap.begin(),   wrap.end());
    g_torchVertCount = (int)all.size();
    glGenVertexArrays(1, &g_torchVao);
    glGenBuffers(1, &g_torchVbo);
    glBindVertexArray(g_torchVao);
    glBindBuffer(GL_ARRAY_BUFFER, g_torchVbo);
    glBufferData(GL_ARRAY_BUFFER, all.size() * sizeof(TorchVertex), all.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TorchVertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TorchVertex), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

static GLuint g_floorVao, g_floorVbo;

static void initFloor() {
    static const float verts[] = {
       //  X       Y     Z      NX      NY  NZ ,
        -50.f,  -1.8f,  -50.f,  0.f,  1.f,  0.f,
         50.f,  -1.8f,  -50.f,  0.f,  1.f,  0.f,
         50.f,  -1.8f,   50.f,  0.f,  1.f,  0.f,

        -50.f,  -1.8f,  -50.f,  0.f,  1.f,  0.f,
         50.f,  -1.8f,   50.f,  0.f,  1.f,  0.f,
        -50.f,  -1.8f,   50.f,  0.f,  1.f,  0.f,
    };

    glGenVertexArrays(1, &g_floorVao);
    glGenBuffers(1, &g_floorVbo);
    glBindVertexArray(g_floorVao);
    glBindBuffer(GL_ARRAY_BUFFER, g_floorVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // location 0 = position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // location 1 = normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

int main(int, char* argv[]) {
    // Set CWD to exe directory so shaders/ and models/ are always found
    try {
        auto exeDir = std::filesystem::path(argv[0]).parent_path();
        if (!exeDir.empty())
            std::filesystem::current_path(exeDir);
    } catch (...) {}

    glfwSetErrorCallback(errorCB);
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Fire Simulation", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }

    glfwSetKeyCallback(window, keyCB);
    glfwSetCursorPosCallback(window, mouseCB);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    g_cam.init(window);   // captures cursor

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n"; return 1;
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';
    std::cout << "Controls: WASD = move | mouse = look | Space/Ctrl = up/down | Esc = quit\n";

    try {
    ParticleSystem fire;
    ParticleSystem torchFire;
    SparkSystem    sparks;
    SmokeSystem    smoke;
    Skybox         skybox;
    Mesh           campfire("models/campFire.obj");

    Shader fireShader ("shaders/fire.vert",  "shaders/fire.frag");
    Shader sparkShader("shaders/spark.vert", "shaders/spark.frag");
    Shader smokeShader("shaders/smoke.vert", "shaders/smoke.frag");
    Shader emberShader("shaders/spark.vert", "shaders/spark.frag");
    Shader meshShader ("shaders/mesh.vert",  "shaders/mesh.frag");

    initEmbers();
    initFloor();
    initTorch();

    // Campfire model: scale from raw OBJ units, sit at y=-1.4
    Mat4 meshModel = Mat4::translate({0.f, -1.4f, 0.f})
                   * Mat4::scale({0.003f, 0.003f, 0.003f});

    // Fire point light flickers just above the campfire base
    PointLight light;
    light.position = {0.f, -1.1f, 0.f};

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    double prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float  dt  = (float)(now - prevTime);
        prevTime   = now;
        float  tm  = (float)now;

        g_cam.processInput(window, dt);

        light.update(tm);
        fire.update(dt);   fire.uploadToGPU();
        sparks.update(dt); sparks.uploadToGPU();
        smoke.update(dt);  smoke.uploadToGPU();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        float aspect = (float)w / (float)h;

        Mat4  view = g_cam.view();
        Mat4  proj = g_cam.proj(aspect);
        Mat4  mvp  = proj * view;  
        float ps   = g_cam.pointScale((float)h);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Skybox (behind everything) 
        glDisable(GL_BLEND);
        skybox.draw(proj, view);

        // Campfire mesh (opaque, Phong lit by fire)
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        meshShader.use();
        meshShader.setMat4 ("uModel",          meshModel.ptr());
        meshShader.setMat4 ("uView",           view.ptr());
        meshShader.setMat4 ("uProjection",     proj.ptr());
        meshShader.setVec3 ("uLightPos",       light.position.x, light.position.y, light.position.z);
        meshShader.setVec3 ("uLightColor",     light.color.x,    light.color.y,    light.color.z);
        meshShader.setFloat("uLightIntensity", light.intensity);
        meshShader.setVec3 ("uCameraPos",      g_cam.pos.x, g_cam.pos.y, g_cam.pos.z);
        meshShader.setVec3 ("uColor",          0.45f, 0.28f, 0.12f);  // wood brown
        campfire.draw();

        // Floor
        meshShader.setMat4("uModel", Mat4::identity().ptr());
        meshShader.setVec3("uColor", 0.2f, 0.15f, 0.08f);
        glBindVertexArray(g_floorVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Smoke (alpha blend, before fire)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        smokeShader.use();
        smokeShader.setMat4 ("uMVP",        mvp.ptr());
        smokeShader.setFloat("uPointScale", ps);
        glBindVertexArray(smoke.vao());
        glDrawArrays(GL_POINTS, 0, smoke.liveCount());

        // Fire + Sparks + Embers (additive glow)
        glBlendFunc(GL_ONE, GL_ONE);

        fireShader.use();
        fireShader.setMat4 ("uMVP",        mvp.ptr());
        fireShader.setFloat("uPointScale", ps);
        glBindVertexArray(fire.vao());
        glDrawArrays(GL_POINTS, 0, fire.liveCount());

        if (g_torchOn) {
            // Torch fire
            Vec3 camRight   = {view.m[0], view.m[4], view.m[8]};
            Vec3 camUp      = {view.m[1], view.m[5], view.m[9]};
            Vec3 camForward = {-view.m[2], -view.m[6], -view.m[10]};
            Vec3 torchTip = g_cam.pos + camRight * 1.1 + camForward * 2 - camUp * 0.1f;

            torchFire.setSpawnOrigin(torchTip);
            torchFire.update(dt);   torchFire.uploadToGPU();
            glBindVertexArray(torchFire.vao());
            glDrawArrays(GL_POINTS, 0, torchFire.liveCount());
        }
        sparkShader.use();
        sparkShader.setMat4 ("uMVP",        mvp.ptr());
        sparkShader.setFloat("uPointScale", ps);
        glBindVertexArray(sparks.vao());
        glDrawArrays(GL_POINTS, 0, sparks.liveCount());

        emberShader.use();
        emberShader.setMat4 ("uMVP",        mvp.ptr());
        emberShader.setFloat("uPointScale", ps);
        glBindVertexArray(g_emberVao);
        glDrawArrays(GL_POINTS, 0, EMBER_COUNT);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        if (g_torchOn) {
            // Torch (camera-space, always on top)
            glClear(GL_DEPTH_BUFFER_BIT);
            Mat4 torchModel = Mat4::translate({0.32f, -0.28f, -0.5f})
                            * Mat4::rotate(-0.4f, {1, 0, 0})
                            * Mat4::rotate( 0.4f, {0, 1, 1})
                            * Mat4::rotate(-0.4f, {0, 0, 1});
            meshShader.use();
            meshShader.setMat4 ("uModel",          torchModel.ptr());
            meshShader.setMat4 ("uView",           Mat4::identity().ptr());
            meshShader.setMat4 ("uProjection",     proj.ptr());
            meshShader.setVec3 ("uLightPos",       0.32f, 0.0f, -0.5f);
            meshShader.setVec3 ("uLightColor",     light.color.x,    light.color.y,    light.color.z);
            meshShader.setFloat("uLightIntensity", light.intensity);
            meshShader.setVec3 ("uCameraPos",      0.f, 0.f, 0.f);
            meshShader.setVec3 ("uColor",          0.45f, 0.28f, 0.12f);
            glBindVertexArray(g_torchVao);
            glDrawArrays(GL_TRIANGLES, 0, g_torchVertCount - 12*6);
            meshShader.setVec3("uColor", 0.15f, 0.10f, 0.08f);
            glDrawArrays(GL_TRIANGLES, g_torchVertCount - 12*6, 12*6);
            glBindVertexArray(0);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    glDeleteVertexArrays(1, &g_torchVao);
    glDeleteBuffers(1, &g_torchVbo);
    glDeleteVertexArrays(1, &g_floorVao);
    glDeleteBuffers(1, &g_floorVbo);
    glDeleteVertexArrays(1, &g_emberVao);
    glDeleteBuffers(1, &g_emberVbo);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
