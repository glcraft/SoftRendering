#include "MainGame.h"
#include <iostream>
#include <vector>
#include "constants.h"
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
void MainGame::init()
{
    glfwInit();
    m_window = glfwCreateWindow(Constants::WindowWidth, Constants::WindowHeight, "Noël démo", NULL, NULL);

    glfwMakeContextCurrent(m_window);
    glewExperimental = true;
    glewInit();

    m_cframe = std::unique_ptr<CustomFrame>(new CustomFrame);
    m_cmdBuffer.clear_image(glm::tvec3<uint8_t>(0,0,0));
    m_cmdBuffer.draw_triangle({1,-1}, {0, 0.8f}, {-1,1}, {0,0.5f,0});
    m_cmdBuffer.draw_triangle({-1,-1}, {0, -0.8f}, {1,1}, {0.5f,0,0});
    VertexBuffer vbo;
    vbo.type=VertexBuffer::Type::Triangles;
    vbo.verts.resize(9);
    
    for (size_t i1=0;i1<3;i1++)
    {
        const float size=0.5f;
        size_t reali1=i1*3;
        float angle1 = static_cast<float>(i1)/3.f*2*glm::pi<float>() + 1.f;
        glm::vec2 pos1(glm::cos(angle1)*size, glm::sin(angle1)*size);

        for (size_t i2=0;i2<3;i2++)
        {
            float angle = static_cast<float>(i2-i1+3)/3.f*2*glm::pi<float>() + 1.f;
            glm::vec2 pos(glm::cos(angle)*size, glm::sin(angle)*size);
            vbo.verts[reali1+i2].pos=glm::vec4(pos1+pos, 0.f, 1.f);
        }
        vbo.verts[reali1+0].color=glm::vec3(1,0,0);
        vbo.verts[reali1+1].color=glm::vec3(0,1,0);
        vbo.verts[reali1+2].color=glm::vec3(0,0,1);
    }
    cmdTest = m_cmdBuffer.draw_buffer(vbo);
    cmdTest->vertShader->m_viewmat = glm::ortho(-16.f/9.f, 16.f/9.f, -1.f, 1.f);
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
    for (int i=0;i<1;i++)
    {
        float angle = static_cast<float>(i)/3.f*2*glm::pi<float>() + static_cast<float>(glfwGetTime())*1.f;
        cmdTest->vertShader->m_modelmat=glm::mat4(1.f);
        cmdTest->vertShader->m_modelmat = glm::rotate(cmdTest->vertShader->m_modelmat, angle*0.2f, {0,1,0});
        cmdTest->vertShader->m_modelmat = glm::rotate(cmdTest->vertShader->m_modelmat, angle, {0,0,1});
    }
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    m_cframe->draw_command_buffer(m_cmdBuffer);
    m_cframe->blit(0, {0, 0}, {Constants::WindowWidth, Constants::WindowHeight});
}
