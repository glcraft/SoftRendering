#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "CustomFrame.h"

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
    CommandBuffer::observer_command cmdTest;
};