#include "CustomFrame.h"
#include "constants.h"
#include <algorithm>
#include <numeric>
std::vector<std::pair<int, int>> genBresenhamLine(glm::ivec2* pos)
{
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    if (pos[0].x>pos[1].x)
        std::swap(pos[0], pos[1]);
    glm::vec2 delta = pos[1]-pos[0];
    if (delta.y==0)
        return {{pos[0].x, pos[1].x}};
    
    std::vector<std::pair<int, int>> result(glm::abs(pos[1].x-pos[0].x));
    if (delta.x==0)
    {
        int i=0;
        for (int y=pos[0].y, i=0;y<pos[1].y;++y)
            result[i]={pos[0].x, pos[0].x};
        return result;
    }
    float deltaErr = glm::abs(delta.y/delta.x);
    float error=0;
    result[0].first=pos[0].x;
    for (int x=pos[0].x, i=0; x<pos[1].x;++x)
    {
        error+=deltaErr;
        if (error>= 0.5f)
            while (error>= 0.5f)
        {
                result[i].second=x;
            error-=1.f;
                result[++i].first=x+(error< 0.5f);
            }
    }
    return result;
}
void draw_horizontal(glm::tvec3<uint8_t>* pixs, int y, std::pair<int, int> xs, glm::tvec3<uint8_t> color)
{
    int yW=y*Constants::TextureWidth;
    for (int x=xs.first; x<=xs.second;++x)
        pixs[x+yW]=color;
}
void CustomFrame::draw_line(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    
    glm::ivec2 pos[2]={cmd.pos[0], cmd.pos[1]};
    auto bresLine = genBresenhamLine(pos);
    int signDeltaY=glm::sign(pos[1].y-pos[0].y);
    int y=pos[0].y;
    for(const auto& range : bresLine)
    {
        draw_horizontal(pixs, y, range, cmd.color);
        y+=signDeltaY;
    }

        }
    }
}
void CustomFrame::clear_image(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    for(int ix=0;ix<Constants::TextureWidth*Constants::TextureHeight;ix++)
        pixs[ix]=cmd.color;
}