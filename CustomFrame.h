#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <experimental/memory>
#include "constants.h"
struct Command
{
    enum class Type{
        Clear,
        DrawLine,
        DrawTriangle,
    };
    const Type type;
    struct Data
    {
        glm::ivec2 pos[3];
        glm::tvec3<uint8_t> color;
    } data;
};
class CommandBuffer
{
public:
    using observer_command=std::experimental::observer_ptr<Command>;
    void reserve(size_t size);
    observer_command clear_image(glm::vec3 color);
    observer_command draw_line(glm::ivec2 pos1, glm::ivec2 pos2, glm::vec3 color);
    observer_command draw_triangle(glm::ivec2 pos1, glm::ivec2 pos2, glm::ivec2 pos3, glm::vec3 color);

    void clear_buffer();
    const std::vector<std::unique_ptr<Command>>& get_buffer() const
    {return m_cmdBuffer;}
private:
    std::vector<std::unique_ptr<Command>> m_cmdBuffer;
};

class CustomFrame
{
public:
    CustomFrame(glm::uvec2 = glm::uvec2(Constants::TextureWidth, Constants::TextureHeight));
    ~CustomFrame();
    void setImageSize(glm::uvec2);
    glm::uvec2 getImageSize();

    void blit(GLuint fboDestination, glm::ivec2 pos1, glm::ivec2 pos2);
    void draw_command_buffer(const CommandBuffer& cmdBuffer);
    void apply();
private:
    void clear_image(glm::tvec3<uint8_t>* pixs, const Command::Data& cmd);
    void draw_line(glm::tvec3<uint8_t>* pixs, const Command::Data& cmd);
    void draw_triangle(glm::tvec3<uint8_t>* pixs, const Command::Data& cmd);

    void draw_horizontal(glm::tvec3<uint8_t>* pixs, int y, std::pair<int, int> xs, glm::tvec3<uint8_t> color);

    glm::uvec2 m_size;
    GLuint PBO;
    GLuint FBO;
    GLuint texture;
};