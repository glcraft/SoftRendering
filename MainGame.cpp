#include "MainGame.h"
#include <iostream>
#include <vector>
#include "constants.h"
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
void MainGame::init()
{
    glfwInit();
    m_window = glfwCreateWindow(Constants::WindowWidth, Constants::WindowHeight, "Noël démo", NULL, NULL);

    glfwMakeContextCurrent(m_window);
    glewExperimental = true;
    glewInit();

    constexpr int TW=Constants::TextureWidth-1, TH = Constants::TextureHeight-1;
    m_cframe = std::unique_ptr<CustomFrame>(new CustomFrame);
    m_cmdBuffer.clear_image(glm::tvec3<uint8_t>(0,0,0));
    m_cmdBuffer.draw_triangle({1,-1}, {0, 0.8f}, {-1,1}, {0,1,0});
    m_cmdBuffer.draw_triangle({-1,-1}, {0, -0.8f}, {1,1}, {1,0,0});
    VertexBuffer vbo;
    vbo.type=VertexBuffer::Type::Triangles;
    vbo.verts.resize(3);
    for (int i=0;i<3;i++)
        {
            float angle = static_cast<float>(i)/3.f*2*glm::pi<float>() + 1.f;
            glm::vec2 pos(glm::cos(angle)*0.8f, glm::sin(angle)*0.8f);
            vbo.verts[i].pos=glm::vec4(pos, 0.f, 1.f);
        }
    vbo.verts[0].color=glm::vec3(1,0,0);
    vbo.verts[1].color=glm::vec3(0,1,0);
    vbo.verts[2].color=glm::vec3(0,0,1);
    cmdTest = m_cmdBuffer.draw_buffer(vbo);
}
void MainGame::display()
{
    while (!glfwWindowShouldClose(m_window))
    {
        render();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}
void MainGame::clear()
{
    m_cframe.reset();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void MainGame::render()
{
    if (cmdTest && 0)
        for (int i=0;i<3;i++)
        {
            float angle = static_cast<float>(i)/3.f*2*glm::pi<float>() + glfwGetTime()*5.f;
            float angle2 = glfwGetTime()*2;
            glm::vec2 pos(glm::cos(angle)*0.4f+glm::cos(angle2), glm::sin(angle)*0.4f+glm::sin(angle2));
            cmdTest->vbo.verts[i].pos=glm::vec4(pos, 0.f, 1.f);
        }
    for (int i=0;i<3;i++)
    {
        float angle = static_cast<float>(i)/3.f*2*glm::pi<float>() + glfwGetTime()*0.5f;
        glm::vec2 pos(glm::cos(angle)*0.8f, glm::sin(angle)*0.8f);
        cmdTest->vbo.verts[i].pos=glm::vec4(pos, 0.f, 1.f);
    }
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    m_cframe->draw_command_buffer(m_cmdBuffer);
    m_cframe->blit(0, {0, 0}, {Constants::WindowWidth, Constants::WindowHeight});
}
