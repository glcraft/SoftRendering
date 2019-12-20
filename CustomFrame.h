#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <experimental/memory>
#include "constants.h"
using colorraw_t = glm::tvec3<uint8_t>;
struct VertexBrut
{
    glm::ivec2 pos;
    colorraw_t color;
};
struct Vertex
{
    Vertex(){}
    Vertex(glm::vec4 _pos, glm::vec3 _color) : pos(_pos), color(_color) {}
    virtual glm::vec4 getPosition() const {return pos;};
    glm::vec4 pos;
    glm::vec3 color;
};
struct VertexBuffer
{
    enum class Type{
        Triangles,
        TriangleStrip,
        TriangleFan,
        Lines,
        LineFan
    };
    Type type;
    std::vector<Vertex> verts;
};
struct Command
{
    enum class Type{
        Clear,
        DrawBuffer,
    };
    Command(Type _type) : type(_type){}
    const Type type;
};
struct ClearCommand : public Command
{
    using observer=std::experimental::observer_ptr<ClearCommand>;
    ClearCommand(glm::vec3 _color=glm::vec3(0.f)) : Command(Command::Type::Clear), color(_color){}
    glm::vec3 color;
};
struct DrawCommand : public Command
{
    using observer=std::experimental::observer_ptr<DrawCommand>;
    DrawCommand() : Command(Command::Type::DrawBuffer){}
    VertexBuffer vbo;
};
class CommandBuffer
{
public:
    
    void reserve(size_t size);
    ClearCommand::observer clear_image(glm::vec3 color);
    DrawCommand::observer draw_line(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color);
    DrawCommand::observer draw_triangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec3 color);
    DrawCommand::observer draw_buffer(VertexBuffer);
    
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
    void clear_image(glm::tvec3<uint8_t>* pixs, const glm::vec3& cmd);
    void draw_line(glm::tvec3<uint8_t>* pixs, const VertexBuffer& cmd);
    void draw_triangle(glm::tvec3<uint8_t>* pixs, const VertexBuffer& cmd);

    void draw_horizontal(glm::tvec3<uint8_t>* pixs, int y, std::pair<int, int> xs, std::pair<colorraw_t, colorraw_t> color);
    glm::ivec2 toScreenSpace(glm::vec2 p) 
    {
        return (p+1.f)*0.5f*glm::vec2(m_size);
    }

    glm::uvec2 m_size;
    GLuint PBO;
    GLuint FBO;
    GLuint texture;
};