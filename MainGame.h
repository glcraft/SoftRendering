#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <unordered_map>
#include "CustomFrame.h"
#include "CustomTexture.h"
#include "Modele3D.h"

struct Obj3D
{
    std::unique_ptr<CustomTexture> texture;
    std::unique_ptr<VertexBuffer> vbo;
    std::unique_ptr<VertexShader> vShader;
    std::unique_ptr<FragmentShader> fShader;
};

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
    std::unordered_map<std::string, Obj3D> m_objs;
    glm::mat4 m_projmat, m_viewmat, m_modelmat;
};