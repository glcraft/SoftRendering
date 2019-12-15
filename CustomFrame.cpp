#include "CustomFrame.h"
#include "constants.h"
#include <iostream>

CustomFrame::CustomFrame()
{
    glGenBuffers(1, &PBO);
    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    

    
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Constants::TextureWidth, Constants::TextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    if (auto status = glCheckFramebufferStatus(FBO))
        std::cout << "FrameBuffer error " << status << std::endl;
    
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, Constants::TextureWidth*Constants::TextureHeight*3, nullptr, GL_STREAM_DRAW);
    {
        // std::vector<uint8_t> imgsrc(Constants::TextureWidth*Constants::TextureHeight*3);
        uint8_t* pixs = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
        constexpr int totalSize=Constants::TextureWidth*Constants::TextureHeight*3;
        for(int i=0;i<totalSize;i++)
            pixs[i] = rand()%256;
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Constants::TextureWidth, Constants::TextureHeight, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

    glBlitFramebuffer(0, 0, Constants::TextureWidth, Constants::TextureHeight, pos1.x, pos1.y, pos2.x, pos2.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
void CustomFrame::clear_image(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    for(int ix=0;ix<Constants::TextureWidth*Constants::TextureHeight;ix++)
        pixs[ix]=cmd.color;
}
void CustomFrame::apply()
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Constants::TextureWidth, Constants::TextureHeight, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
void CustomFrame::draw_line(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    glm::vec2 delta = cmd.pos2-cmd.pos1;
    float deltaErr = abs(delta.y/delta.x);
    float error=0;
    int y=0;
    for (int x=cmd.pos1.x; x<cmd.pos2.x;++x)
    {
        pixs[x+y*Constants::TextureWidth] = cmd.color;
        error+=deltaErr;
        if (error>= 0.5f)
        {
            y+=glm::sign(delta.y);
            error-=1.f;
        }
    }
}

void CustomFrame::draw_command_buffer(const CommandBuffer& cmdBuffer)
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glm::tvec3<uint8_t>* pixs = static_cast<glm::tvec3<uint8_t>*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
    const auto& cmdData = cmdBuffer.get_buffer();
    for (auto& cmd : cmdData)
    {
        switch(cmd.type)
        {
            case Command::Type::Clear:
                clear_image(pixs, cmd);
                break;
            case Command::Type::DrawLine:
                draw_line(pixs, cmd);
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
void CommandBuffer::clear_image(glm::vec3 color)
{
    Command cmd;
    cmd.type = Command::Type::Clear;
    cmd.color=color*255.f;
    m_cmdBuffer.push_back(cmd);
}
void CommandBuffer::draw_line(glm::ivec2 pos1, glm::ivec2 pos2, glm::vec3 color)
{
    Command cmd{Command::Type::DrawLine, pos1, pos2, color*255.f};
    m_cmdBuffer.push_back(cmd);
}