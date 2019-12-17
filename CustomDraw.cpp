#include "CustomFrame.h"
#include "constants.h"
#include <algorithm>
#include <numeric>
std::vector<std::pair<int, int>> genBresenhamLine(std::pair<glm::ivec2, glm::ivec2> pos)
{
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    // glm::ivec2 pos[2] = {pos_orig[0], pos_orig[1]};
    bool reverse=false;
    if (pos.first.x>pos.second.x)
    {
        std::swap(pos.first, pos.second);
        reverse=true;
    }
    glm::vec2 delta = pos.second-pos.first;
    if (delta.y==0)
        return {{pos.first.x, pos.second.x}};
    
    std::vector<std::pair<int, int>> result(glm::abs(pos.second.y-pos.first.y)+1);
    if (delta.x==0)
    {
        int i=0;
        for (int y=pos.first.y, i=0;y<pos.second.y;++y)
            result[i]={pos.first.x, pos.first.x};
        return result;
    }
    float deltaErr = glm::abs(delta.y/delta.x);
    float error=0;
    result[0].first=pos.first.x;
    for (int x=pos.first.x, i=0; x<pos.second.x;++x)
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
    result.back().second=pos.second.x;
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
    std::pair<glm::ivec2, glm::ivec2> pos={cmd.pos[0], cmd.pos[1]};
    auto bresLine = genBresenhamLine(pos);
    int signDeltaY=glm::sign(pos.second.y-pos.first.y);
    int y=pos.first.y;
    for(const auto& range : bresLine)
    {
        draw_horizontal(pixs, y, range, cmd.color);
        y+=signDeltaY;
    }

}
void CustomFrame::draw_triangle(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    const auto flatTop = [&pixs, &cmd](glm::ivec2 pos[3]){
        glm::ivec2 posx[2]={pos[1], pos[2]};
        if (pos[1].x<=pos[2].x)
            posx[0]=pos[1], posx[1]=pos[2];
        else
            posx[0]=pos[2], posx[1]=pos[1];
        std::vector<std::pair<int, int>> ligne[2]={genBresenhamLine({pos[0], posx[0]}), genBresenhamLine({pos[0], posx[1]})};
        int signDeltaY=glm::sign(pos[1].y-pos[0].y);
        int y=pos[0].y;
        size_t maxy=glm::min(ligne[0].size(), ligne[1].size());
        for(size_t i=0;i<maxy;i++)
        {
            draw_horizontal(pixs, y, {ligne[0][i].first, ligne[1][i].second}, cmd.color);
            y+=signDeltaY;
        }
    };
    const auto flatBottom = [&pixs, &cmd](glm::ivec2 pos[3]){
        int test=0;
    };
    glm::ivec2 pos[3]={cmd.pos[0], cmd.pos[1], cmd.pos[2]};
    std::sort(std::begin(pos), std::end(pos), [](const glm::ivec2& p1, const glm::ivec2& p2) { return p1.y<p2.y; });
    if (pos[1].y==pos[2].y)
        flatTop(pos);
    else if (pos[1].y==pos[0].y)
    {
        std::swap(pos[0], pos[2]);
        flatTop(pos);
    }
    // else
    // {
    //     /* code */
    // }
    
}
void CustomFrame::clear_image(glm::tvec3<uint8_t>* pixs, const Command& cmd)
{
    for(int ix=0;ix<Constants::TextureWidth*Constants::TextureHeight;ix++)
        pixs[ix]=cmd.color;
}