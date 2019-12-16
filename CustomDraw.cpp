#include "CustomFrame.h"
#include "constants.h"
void CustomFrame::draw_line(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    glm::vec2 delta = cmd.pos2-cmd.pos1;
    float deltaErr = glm::abs(delta.y/delta.x);
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
void CustomFrame::clear_image(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    for(int ix=0;ix<Constants::TextureWidth*Constants::TextureHeight;ix++)
        pixs[ix]=cmd.color;
}