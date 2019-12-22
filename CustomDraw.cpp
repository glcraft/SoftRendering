#include "CustomFrame.h"
#include "constants.h"
#include <algorithm>
#include <numeric>
struct BresenhamLine
{
    std::vector<std::pair<int, int>> line;
    size_t npoints=0;
};
BresenhamLine genBresenhamLine(std::pair<glm::ivec2, glm::ivec2> pos)
{
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    bool reverse=false;
    if (pos.first.x>pos.second.x)
    {
        std::swap(pos.first, pos.second);
        reverse=true;
    }
    glm::vec2 delta = pos.second-pos.first;
    if (delta.y==0)
        return {{{pos.first.x, pos.second.x}}, static_cast<size_t>(glm::abs(pos.first.x-pos.second.x)+1)};
    
    BresenhamLine result;
    result.line.resize(glm::abs(pos.second.y-pos.first.y)+1);
    if (delta.x==0)
    {
        if (pos.first.y>pos.second.y)
        {
            std::swap(pos.first, pos.second);
        }
        int i=0;
        for (int y=pos.first.y, i=0;y<=pos.second.y;++y, ++i)
            result.line[i]={pos.first.x, pos.first.x};
        result.npoints=result.line.size();
        return result;
    }
    float deltaErr = glm::abs(delta.y/delta.x);
    float error=0;
    result.line[0].first=pos.first.x;
    for (int x=pos.first.x, i=0; x<pos.second.x;++x)
    {
        error+=deltaErr;
        if (error>= 0.5f)
            while (error>= 0.5f)
            {
                result.line[i].second=x;
                result.npoints+=result.line[i].second-result.line[i].first+1;
                error-=1.f;
                result.line[++i].first=x+(error< 0.5f);
            }
    }
    result.line.back().second=pos.second.x;
    result.npoints+=result.line.back().second-result.line.back().first+1;
    if (reverse)
        std::reverse(result.line.begin(), result.line.end());
    return result;
}
template <typename VecT, typename T>
VecT interp(VecT first, VecT second, T current, T total)
{
    return first*(total-current)/total+second*(current)/total;
}
void CustomFrame::draw_horizontal(colorraw_t* pixs, int y, std::pair<int, int> xs, std::pair<colorraw_t, colorraw_t> colors)
{
    int yW=y*m_size.x;
    if (xs.second<0)
        return;
    if (xs.second==xs.first)
    {
        if (xs.first>=m_size.x)
            return;
        pixs[xs.first+yW]=colors.first;
        return;
    }
    int total=xs.second-xs.first;
    for (int x=glm::max(xs.first, 0), cx=x-xs.first; x<=xs.second && x<m_size.x;++x, ++cx)
    {
        glm::ivec3 newcolor = interp(glm::ivec3(colors.first), glm::ivec3(colors.second), cx, total);
        pixs[x+yW]=newcolor;
    }
}
void CustomFrame::draw_line(colorraw_t* pixs, const VertexBuffer& vbo)
{
    std::pair<glm::ivec2, glm::ivec2> pos={toScreenSpace(vbo.verts[0].pos), toScreenSpace(vbo.verts[1].pos)};
    auto bresLine = genBresenhamLine(pos);
    int signDeltaY=glm::sign(pos.second.y-pos.first.y);
    int y=pos.first.y;
    int i=0;
    for(int i=0;i<bresLine.line.size();++i)
    {
        if (y<0)
        {
            if (signDeltaY<0)
                break;
            else
            {
                int ecart = -y;
                y=0;
                i+=ecart;
                continue;
            }
        }
        else if (y>=m_size.y)
        {
            if (signDeltaY>0)
                break;
            else
            {
                int ecart = glm::abs(y-m_size.y-1);
                y=m_size.y-1;
                i+=ecart;
                continue;
            }
        }
        draw_horizontal(pixs, y, bresLine.line[i], {colorraw_t(vbo.verts[0].color*255.f), colorraw_t(vbo.verts[1].color*255.f)});
        y+=signDeltaY;
    }

}
void CustomFrame::draw_triangle(colorraw_t* pixs, const DrawCommand& cmd)
{
    // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    const auto flatTop = [&pixs, this](VertexBrut vertsb[3], size_t offset=0){
        size_t indx[2]={1,2};
        
        BresenhamLine ligne[2]={genBresenhamLine({vertsb[0].pos, vertsb[indx[0]].pos}), genBresenhamLine({vertsb[0].pos, vertsb[indx[1]].pos})};
        size_t maxy=glm::min(ligne[0].line.size(), ligne[1].line.size());
        if (ligne[0].line[maxy-1].first>ligne[1].line[maxy-1].first)
        {
            std::swap(ligne[0], ligne[1]);
            indx[0]=2, indx[1]=1;
        }
        int signDeltaY=glm::sign(vertsb[1].pos.y-vertsb[0].pos.y);
        int y=vertsb[0].pos.y;
        size_t i=offset;
        size_t iPoints1=0;
        size_t iPoints2=0;
        for(;i<maxy;i++)
        {
            if (y<0)
            {
                if (signDeltaY<0)
                    break;
                else
                {
                    int ecart = -y;
                    for(int iLine=0;iLine<=ecart;iLine++)
                    {
                        iPoints1+=ligne[0].line[i+iLine].second-ligne[0].line[i+iLine].first+1;
                        iPoints2+=ligne[1].line[i+iLine].second-ligne[1].line[i+iLine].first+1;
                    }
                    y=0;
                    i+=ecart;
                    continue;
                }
            }
            else if (y>=m_size.y)
            {
                if (signDeltaY>0)
                    break;
                else
                {
                    int resultCalc=y-static_cast<int>(m_size.y)-1;
                    int ecart = glm::abs(resultCalc);
                    for(int iLine=0;iLine<=ecart;iLine++)
                        {
                            iPoints1+=ligne[0].line[i+iLine].second-ligne[0].line[i+iLine].first+1;
                            iPoints2+=ligne[1].line[i+iLine].second-ligne[1].line[i+iLine].first+1;
                        }
                    y=m_size.y-1;
                    i+=ecart;
                    continue;
                }
            }
            int left=glm::min(ligne[0].line[i].first, ligne[1].line[i].second);
            int right=glm::max(ligne[0].line[i].first, ligne[1].line[i].second);
            glm::ivec3 
                lColor(interp<glm::ivec3, int>(vertsb[0].color, vertsb[indx[0]].color, iPoints1+glm::max(0,(ligne[0].line[i].second-ligne[0].line[i].first)), ligne[0].npoints)), 
                rColor(interp<glm::ivec3, int>(vertsb[0].color, vertsb[indx[1]].color, iPoints2+glm::max(0,(ligne[1].line[i].second-ligne[1].line[i].first)), ligne[1].npoints));//
            draw_horizontal(pixs, y, {left, right}, {lColor, rColor});
            y+=signDeltaY;
            iPoints1+=ligne[0].line[i].second-ligne[0].line[i].first+1;
            iPoints2+=ligne[1].line[i].second-ligne[1].line[i].first+1;
        }
        return i;
    };
    const VertexBuffer& vbo = cmd.vbo;
    size_t nbVertsMax=vbo.verts.size()-2;
    for (size_t iTri=0;iTri<nbVertsMax;iTri+=3)
    {
        Vertex trans_vpos[3] = {
            cmd.vertShader->get(vbo.verts[iTri+0]), 
            cmd.vertShader->get(vbo.verts[iTri+1]), 
            cmd.vertShader->get(vbo.verts[iTri+2])
        };
        VertexBrut vertsb[3]={
            {toScreenSpace(trans_vpos[0].pos), trans_vpos[0].color*255.f}, 
            {toScreenSpace(trans_vpos[1].pos), trans_vpos[1].color*255.f}, 
            {toScreenSpace(trans_vpos[2].pos), trans_vpos[2].color*255.f}
        };
        std::sort(std::begin(vertsb), std::end(vertsb), [](const VertexBrut& p1, const VertexBrut& p2) { return p1.pos.y<p2.pos.y; });
        if (vertsb[1].pos.y==vertsb[2].pos.y)
            flatTop(vertsb);
        else if (vertsb[1].pos.y==vertsb[0].pos.y)
        {
            std::swap(vertsb[0], vertsb[2]);
            flatTop(vertsb);
        }
        else
        {
            size_t offset = flatTop(vertsb);
            std::swap(vertsb[0], vertsb[2]);
            flatTop(vertsb);
        }
    }
}
void CustomFrame::clear_image(colorraw_t* pixs, const glm::vec3& color)
{
    size_t s=m_size.x*m_size.y;
    for(int ix=0;ix<s;++ix)
        pixs[ix]=color;
}
Vertex VertexShader::get(Vertex vert)
{
    vert.pos=m_projmat*m_viewmat*m_modelmat*vert.pos;
    return vert;
}