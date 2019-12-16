#include "CustomFrame.h"
#include "constants.h"
#include <algorithm>
#include <numeric>
std::vector<std::pair<int, int>> genBresenhamLine(glm::ivec2* pos_orig)
{
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    glm::ivec2 pos[2] = {pos_orig[0], pos_orig[1]};
    bool reverse=false;
    if (pos[0].x>pos[1].x)
    {
        std::swap(pos[0], pos[1]);
        reverse=true;
    }
    glm::vec2 delta = pos[1]-pos[0];
    if (delta.y==0)
        return {{pos[0].x, pos[1].x}};
    
    std::vector<std::pair<int, int>> result(glm::abs(pos[1].y-pos[0].y)+1);
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
    result.back().second=pos[1].x;
    if (reverse)
        return std::vector<std::pair<int, int>>(result.rbegin(), result.rend());
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
void CustomFrame::draw_triangle(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    const auto flatTop = [pixs](glm::ivec2 pos[3]){
        glm::ivec2 posx[2]={pos[1], pos[2]};
        if (pos[1].x<=pos[2].x)
            posx[0]=pos[1], posx[1]=pos[2];
        else
            posx[0]=pos[2], posx[1]=pos[1];
        
        glm::vec2 delta[2] = {posx[0]-pos[0], posx[1]-pos[0]};
    };
    const auto flatBottom = [pixs](glm::ivec2 pos[3]){

    };
    glm::ivec2 pos[3]={cmd.pos[0], cmd.pos[1], cmd.pos[2]};
    std::sort(std::begin(pos), std::end(pos), [](const glm::ivec2& p1, const glm::ivec2& p2) { return p1.y<p2.y; });
    if (pos[1].y==pos[2].y)
        flatTop(pos);
    else if (pos[1].y==pos[0].y)
        flatBottom(pos);
    else
    {
        /* code */
    }
    
}
void CustomFrame::clear_image(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    for(int ix=0;ix<Constants::TextureWidth*Constants::TextureHeight;ix++)
        pixs[ix]=cmd.color;
}