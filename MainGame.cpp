#include "MainGame.h"
#include <iostream>
#include <vector>
#include "constants.h"
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

class FragmentTextureShader : public FragmentShader
{
public:
    virtual glm::vec3 get(Vertex v) const override
    {
        if (!m_texture)
            return glm::vec3(0.f);
        return m_texture->get(v.uv) * v.color;
    }
    _std::observer_ptr<CustomTexture> m_texture;
};

void MainGame::init()
{
    using namespace std::string_literals;
    glfwInit();
    m_window = glfwCreateWindow(Constants::WindowWidth, Constants::WindowHeight, "Noël démo", NULL, NULL);

    glfwMakeContextCurrent(m_window);
    glewExperimental = true;
    glewInit();

    m_cframe = std::unique_ptr<CustomFrame>(new CustomFrame);
    m_cmdBuffer.clear_image(glm::tvec3<uint8_t>(0,0,0));

    const float size=0.5f;
    const char* lsObj3D[]={"ground","trees","present","present_up"};//
    m_projmat = glm::perspective(glm::radians(90.f), static_cast<float>(Constants::WindowWidth)/static_cast<float>(Constants::WindowHeight), 1.f, 130.f);
    m_viewmat = glm::lookAt(glm::vec3(20.f), glm::vec3(0,0,0), glm::vec3(0,1,0));
    m_modelmat = glm::mat4(1.f);
    for (const char*& name : lsObj3D)
    {
        auto& currentObj = m_objs[name];
        currentObj.texture = std::make_unique<CustomTexture>();
        currentObj.texture->load("res/"s+name+".png");
        currentObj.vShader=std::make_unique<VertexShader>();
        currentObj.vShader->m_projmat = _std::make_observer(&m_projmat);
        currentObj.vShader->m_viewmat = _std::make_observer(&m_viewmat);
        currentObj.vShader->m_modelmat = _std::make_observer(&m_modelmat);
        
        auto fShader = std::make_unique<FragmentTextureShader>();
        fShader->m_texture.reset(currentObj.texture.get());
        currentObj.fShader = std::move(fShader);
        currentObj.vbo=std::make_unique<VertexBuffer>();
        load_model_obj("res/"s+name+".obj",*currentObj.vbo);
        auto cmd = m_cmdBuffer.draw_buffer(util::makeObserver(currentObj.vbo.get()));
        cmd->vertShader=util::makeObserver(currentObj.vShader);
        cmd->fragShader=util::makeObserver(currentObj.fShader);
    }











    // m_texture = std::make_unique<CustomTexture>();
    // m_texture->load("res/tree.png");
    // m_vShader=std::make_unique<VertexShader>();
    // m_vShader->m_projmat = glm::perspective(glm::radians(80.f), static_cast<float>(Constants::WindowWidth)/static_cast<float>(Constants::WindowHeight), 1.f, 30.f);
    // m_vShader->m_viewmat = glm::lookAt(glm::vec3(8.f), glm::vec3(0,0,0), glm::vec3(0,1,0));
    // auto fShader = std::make_unique<FragmentTextureShader>();
    // fShader->m_texture.reset(m_texture.get());
    // m_fShader = std::move(fShader);
    
    // m_vboTri.reset(new VertexBuffer);
    // load_model_obj("res/trees.obj",*m_vboTri);
    // {
    //     auto cmd = m_cmdBuffer.draw_buffer(util::makeObserver(m_vboTri.get()));
    //     cmd->vertShader=util::makeObserver(m_vShader);
    //     cmd->fragShader=util::makeObserver(m_fShader);
    // }
    // m_vboLines.reset(new VertexBuffer);
    // m_vboLines->type=VertexBuffer::Type::LineStrip;
    // m_vboLines->verts.resize(4);
    
    // for (size_t i1=0;i1<=3;i1++)
    // {
    //     const float sizeLine=size*2+0.2f;
    //     float angle = static_cast<float>(i1)/3.f*2*glm::pi<float>() + 1.f;
    //     glm::vec2 pos(glm::cos(angle)*sizeLine, glm::sin(angle)*sizeLine);
    //     m_vboLines->verts[i1].pos=glm::vec4(pos, 0.f, 1.f);
    // }
    // m_vboLines->verts[0].color=glm::vec3(1,0,0);
    // m_vboLines->verts[1].color=glm::vec3(0,0,1);
    // m_vboLines->verts[2].color=glm::vec3(0,1,0);
    // m_vboLines->verts[3].color=glm::vec3(1,0,0);
    // {
    //     auto cmd = m_cmdBuffer.draw_buffer(util::makeObserver(m_vboLines.get()));
    //     cmd->vertShader=util::makeObserver(m_vShader);
    // }
    
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
    {
        float angle = static_cast<float>(glfwGetTime())*1.f;
        m_modelmat = glm::mat4(1.f);
        m_modelmat = glm::rotate(m_modelmat, angle*0.4f, {0,1,0});
    }
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    m_cframe->draw_command_buffer(m_cmdBuffer);
    m_cframe->blit(0, {0, 0}, {Constants::WindowWidth, Constants::WindowHeight});
}
