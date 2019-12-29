#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "CustomFrame.h"
#include "CustomTexture.h"
#include "Modele3D.h"

class MainGame
{
public:
    void init();
    void display();
    void clear();

private:
    void render();
    GLFWwindow* m_window;
    std::unique_ptr<CustomFrame> m_cframe;
    CommandBuffer m_cmdBuffer;
    std::unique_ptr<VertexBuffer> m_vboTri, m_vboLines;
    std::unique_ptr<VertexShader> m_vShader;
    std::unique_ptr<FragmentShader> m_fShader;
    std::unique_ptr<CustomTexture> m_texture;
};