#include "CustomFrame.h"
#include "constants.h"
#include <iostream>

CustomFrame::CustomFrame(glm::uvec2 size)
{
    glGenBuffers(1, &PBO);
    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if (auto status = glCheckFramebufferStatus(FBO))
        std::cout << "FrameBuffer error " << status << std::endl;
    setImageSize(size);
}
CustomFrame::~CustomFrame()
{
    glDeleteBuffers(1, &PBO);

    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &FBO);
}

void CustomFrame::blit(GLuint fboDestination, glm::ivec2 pos1, glm::ivec2 pos2)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboDestination);

    glBlitFramebuffer(0, 0, m_size.x, m_size.y, pos1.x, pos1.y, pos2.x, pos2.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void CustomFrame::apply()
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_size.x, m_size.y, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}


void CustomFrame::draw_command_buffer(const CommandBuffer& cmdBuffer)
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glm::tvec3<uint8_t>* pixs = static_cast<glm::tvec3<uint8_t>*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
    const auto& cmdData = cmdBuffer.get_buffer();
    for (auto& cmd : cmdData)
    {
        switch(cmd->type)
        {
            case Command::Type::Clear:
                clear_image(pixs, reinterpret_cast<ClearCommand*>(cmd.get())->color);
                break;
            case Command::Type::DrawBuffer:
            {
                auto& vbo = reinterpret_cast<DrawCommand*>(cmd.get())->vbo;
                if (vbo.type>=VertexBuffer::Type::Lines)
                    draw_line(pixs, vbo);
                else
                    draw_triangle(pixs, vbo);
            }
                break;
            default:
                break;
        }
    }
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    apply();
}


void CommandBuffer::reserve(size_t size)
{
    m_cmdBuffer.reserve(size);
}
_std::observer_ptr<ClearCommand> CommandBuffer::clear_image(glm::vec3 color)
{
    std::unique_ptr<ClearCommand> cmd(new ClearCommand(color));
    _std::observer_ptr<ClearCommand> res(cmd.get());
    m_cmdBuffer.push_back(std::move(cmd));
    return res;
}
_std::observer_ptr<DrawCommand>  CommandBuffer::draw_line(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color)
{
    std::unique_ptr<DrawCommand> cmd(new DrawCommand);
    cmd->vbo.verts.resize(2);
    cmd->vbo.verts[0]=Vertex{glm::vec4(pos1,0.f,1.f), color};
    cmd->vbo.verts[1]=Vertex{glm::vec4(pos2,0.f,1.f), color};
    m_cmdBuffer.push_back(std::move(cmd));
    _std::observer_ptr<DrawCommand> res(cmd.get());
    m_cmdBuffer.push_back(std::move(cmd));
    return res;
}
DrawCommand::observer CommandBuffer::draw_triangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec3 color)
{
    std::unique_ptr<DrawCommand> cmd(new DrawCommand);
    cmd->vbo.verts.resize(3);
    cmd->vbo.verts[0]=Vertex{glm::vec4(pos1,0.f,1.f), color};
    cmd->vbo.verts[1]=Vertex{glm::vec4(pos2,0.f,1.f), color};
    cmd->vbo.verts[2]=Vertex{glm::vec4(pos3,0.f,1.f), color};
    DrawCommand::observer res(cmd.get());
    m_cmdBuffer.push_back(std::move(cmd));
    return res;
}
DrawCommand::observer CommandBuffer::draw_buffer(VertexBuffer vbo)
{
    std::unique_ptr<DrawCommand> cmd(new DrawCommand);
    std::swap(vbo, cmd->vbo);
    DrawCommand::observer res(cmd.get());
    m_cmdBuffer.push_back(std::move(cmd));
    return res;
}
void CustomFrame::setImageSize(glm::uvec2 size)
{
    m_size=size;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, size.x*size.y*3, nullptr, GL_STREAM_DRAW);
#ifndef NDEBUG
    {
        uint8_t* pixs = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
        size_t totalSize=size.x*size.y*3;
        for(int i=0;i<totalSize;i++)
            pixs[i] = rand()%256;
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
#endif
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}