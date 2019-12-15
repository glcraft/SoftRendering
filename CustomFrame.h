#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
struct Command
{
    enum class Type{
        Clear,
        DrawLine
    } type;
    glm::ivec2 pos1, pos2;
    glm::tvec3<uint8_t> color;
};
class CommandBuffer
{
public:
    void reserve(size_t size);
    void clear_image(glm::vec3 color);
    void draw_line(glm::ivec2 pos1, glm::ivec2 pos2, glm::vec3 color);

    void clear_buffer();
    const std::vector<Command>& get_buffer() const
    {return m_cmdBuffer;}
private:
    std::vector<Command> m_cmdBuffer;
};

class CustomFrame
{
public:
    CustomFrame();
    ~CustomFrame();

    void blit(GLuint fboDestination, glm::ivec2 pos1, glm::ivec2 pos2);
    void draw_command_buffer(const CommandBuffer& cmdBuffer);
    void apply();
private:
    void clear_image(glm::tvec3<uint8_t>* pixs, const Command& cmd);
    void draw_line(glm::tvec3<uint8_t>* pixs, const Command& cmd);
    GLuint PBO;
    GLuint FBO;
    GLuint texture;
};