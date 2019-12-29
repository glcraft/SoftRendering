#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "memory.hpp"
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

template <typename VecT, typename T>
inline VecT interp(VecT first, VecT second, T current, T total)
{
    if (total==0)
        return first;
    return first*(total-current)/total+second*(current)/total;
}
template <typename T>
inline Vertex interp(Vertex first, Vertex second, T current, T total)
{
    if (total==0)
        return first;
    Vertex vres;
    vres.pos = interp(first.pos, second.pos, static_cast<float>(current), static_cast<float>(total));
    vres.color = interp(first.color, second.color, static_cast<float>(current), static_cast<float>(total));
    return vres;
}
template <typename InputType, typename OutputType>
class Shader
{
public:
    virtual OutputType get(InputType) const {return OutputType();};
};
class VertexShader : public Shader<Vertex, Vertex>
{
public:
    virtual Vertex get(Vertex) const override;
    glm::mat4 m_modelmat=glm::mat4(1.f);
    glm::mat4 m_viewmat=glm::mat4(1.f);
    glm::mat4 m_projmat=glm::mat4(1.f);
};
class FragmentShader : public Shader<Vertex, glm::vec3>
{
public:
    virtual glm::vec3 get(Vertex) const override ;
};
struct VertexBuffer
{
    enum class Type{
        Triangles,
        TriangleStrip,
        TriangleFan,
        Lines,
        LineFan,
        LineStrip
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
    using observer=_std::observer_ptr<ClearCommand>;
    ClearCommand(glm::vec3 _color=glm::vec3(0.f)) : Command(Command::Type::Clear), color(_color){}
    glm::vec3 color;
};
struct DrawCommand : public Command
{
    using observer=_std::observer_ptr<DrawCommand>;
    DrawCommand() : Command(Command::Type::DrawBuffer){}
    _std::observer_ptr<VertexBuffer> vbo;
    _std::observer_ptr<VertexShader> vertShader;
    _std::observer_ptr<FragmentShader> fragShader;
};
struct DrawInternalBufferCommand : public DrawCommand
{
    using observer=_std::observer_ptr<DrawCommand>;
    DrawInternalBufferCommand() : DrawCommand(){}
    std::unique_ptr<VertexBuffer> m_internatBuffer;
};
class CommandBuffer
{
public:
    void reserve(size_t size);
    ClearCommand::observer clear_image(glm::vec3 color);
    DrawCommand::observer draw_line(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color);
    DrawCommand::observer draw_triangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec3 color);
    DrawCommand::observer draw_buffer(_std::observer_ptr<VertexBuffer>);
    
    void clear_buffer();
    const std::vector<std::unique_ptr<Command>>& get_buffer() const
    {return m_cmdBuffer;}
private:
    std::vector<std::unique_ptr<Command>> m_cmdBuffer;
};
struct Pixels
{
    colorraw_t* colors=nullptr;
    float* zbuffer=nullptr;
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
    void clear_image(Pixels pixs, const glm::vec3& cmd);
    void draw_line(Pixels pixs, const DrawCommand& cmd);
    void draw_triangle(Pixels pixs, const DrawCommand& cmd);

    void draw_horizontal(Pixels pixs, int y, std::pair<int, int> xs, std::pair<Vertex, Vertex> verts, const FragmentShader& fshad);
    glm::ivec2 toScreenSpace(glm::vec2 p) 
    {
        return (p+1.f)*0.5f*glm::vec2(m_size);
    }

    glm::uvec2 m_size;
    std::vector<float> m_zbuffer;
    GLuint PBO;
    GLuint FBO;
    GLuint texture;
};