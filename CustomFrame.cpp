#include "CustomFrame.h"
#include "constants.h"
#include <iostream>

std::unique_ptr<VertexShader> DefaultVertexShader=std::make_unique<VertexShader>();
std::unique_ptr<FragmentShader> DefaultFragmentShader=std::make_unique<FragmentShader>();;

CustomFrame::CustomFrame(glm::uvec2 size)
{
    glGenBuffers(1, &PBO);
    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
    
    if (auto status = glCheckFramebufferStatus(FBO))
        std::cout << "FrameBuffer error " << status << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    Pixels pixs;
    pixs.colors = static_cast<colorraw_t*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
    pixs.zbuffer = m_zbuffer.data();
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
                auto drawcmd = reinterpret_cast<DrawCommand*>(cmd.get());
                if (drawcmd->vbo->type>=VertexBuffer::Type::Lines)
                    draw_line(pixs, *drawcmd);
                else
                    draw_triangle(pixs, *drawcmd);
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
    std::unique_ptr<DrawInternalBufferCommand> cmd(new DrawInternalBufferCommand);
    cmd->m_internatBuffer=std::unique_ptr<VertexBuffer>(new VertexBuffer);
    cmd->m_internatBuffer->verts.resize(2);
    cmd->m_internatBuffer->verts[0]=Vertex{glm::vec4(pos1,0.f,1.f), color};
    cmd->m_internatBuffer->verts[1]=Vertex{glm::vec4(pos2,0.f,1.f), color};
    cmd->vbo = util::makeObserver(cmd->m_internatBuffer.get());
    cmd->vertShader.reset(DefaultVertexShader.get());
    cmd->fragShader.reset(DefaultFragmentShader.get());
    m_cmdBuffer.push_back(std::move(cmd));
    _std::observer_ptr<DrawCommand> res(cmd.get());
    m_cmdBuffer.push_back(std::move(cmd));
    return res;
}
DrawCommand::observer CommandBuffer::draw_triangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec3 color)
{
    std::unique_ptr<DrawInternalBufferCommand> cmd(new DrawInternalBufferCommand);
    cmd->m_internatBuffer=std::unique_ptr<VertexBuffer>(new VertexBuffer);
    cmd->m_internatBuffer->verts.resize(3);
    cmd->m_internatBuffer->verts[0]=Vertex{glm::vec4(pos1,0.f,1.f), color};
    cmd->m_internatBuffer->verts[1]=Vertex{glm::vec4(pos2,0.f,1.f), color};
    cmd->m_internatBuffer->verts[2]=Vertex{glm::vec4(pos3,0.f,1.f), color};
    cmd->vbo = util::makeObserver(cmd->m_internatBuffer.get());
    cmd->vertShader.reset(DefaultVertexShader.get());
    cmd->fragShader.reset(DefaultFragmentShader.get());
    DrawCommand::observer res(cmd.get());
    m_cmdBuffer.push_back(std::move(cmd));
    return res;
}
DrawCommand::observer CommandBuffer::draw_buffer(_std::observer_ptr<VertexBuffer> vbo)
{
    std::unique_ptr<DrawCommand> cmd(new DrawCommand);
    std::swap(vbo, cmd->vbo);
    DrawCommand::observer res(cmd.get());
    cmd->vertShader.reset(DefaultVertexShader.get());
    cmd->fragShader.reset(DefaultFragmentShader.get());
    m_cmdBuffer.push_back(std::move(cmd));
    return res;
}
void CustomFrame::setImageSize(glm::uvec2 size)
{
    size+=size%glm::uvec2(2);
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
#endif
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_zbuffer.resize(size.x*size.y);
}