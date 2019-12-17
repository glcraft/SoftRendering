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
    m_cmdBuffer.draw_triangle({Constants::TextureWidth-1,0}, {Constants::TextureWidth/2, Constants::TextureHeight-1}, {0, Constants::TextureHeight-1}, {0,1,0});
    m_cmdBuffer.draw_triangle({0,0}, {TW/2, 0}, {TW, TH}, {1,0,0});
    cmdTest = m_cmdBuffer.draw_triangle({0,TH*40/100}, {TW*80/100, 0}, {TW*60/100, TH}, {0,0,1});
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
    for (int i=0;i<3;i++)
    {
        float angle = static_cast<float>(i)/3.f*2*glm::pi<float>() + glfwGetTime();
        glm::vec2 pos(glm::cos(angle)*20+40, glm::sin(angle)*10+30);
        cmdTest->data.pos[i]=glm::ivec2(pos);
    }
        

    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    m_cframe->draw_command_buffer(m_cmdBuffer);
    m_cframe->blit(0, {0, 0}, {Constants::WindowWidth, Constants::WindowHeight});
}
