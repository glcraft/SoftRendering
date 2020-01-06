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

void CustomFrame::draw_horizontal(Pixels pixs, int y, std::pair<int, int> xs, std::pair<Vertex, Vertex> verts, const FragmentShader& fshad)
{
    int yW=y*m_size.x;
    if (xs.second<0)
        return;
    if (xs.second==xs.first)
    {
        if (xs.first>=m_size.x)
            return;
        if (verts.first.pos.z>pixs.zbuffer[xs.first+yW])
            return;
        pixs.zbuffer[xs.first+yW] = verts.first.pos.z;
        pixs.colors[xs.first+yW]=fshad.get(verts.first)*255.f;
        return;
    }
    int total=xs.second-xs.first;
    for (int x=glm::max(xs.first, 0), cx=x-xs.first; x<=xs.second && x<m_size.x;++x, ++cx)
    {
        Vertex currentVert = interp(verts.first, verts.second, cx, total);
        if (currentVert.pos.z>pixs.zbuffer[x+yW])
            continue;
        constexpr float vNear=-1, vFar=1;
        bool pass=false;
        for (int iComp=0;iComp<3 && !pass;++iComp)
        {
            if (currentVert.pos[iComp] <= vNear || currentVert.pos[iComp] > vFar)
                pass=true;
        }
        if (pass)
            continue;
        pixs.zbuffer[x+yW] = currentVert.pos.z;
        glm::ivec3 newcolor = fshad.get(currentVert)*255.f;
        pixs.colors[x+yW]=newcolor;
    }
}
void CustomFrame::draw_line(Pixels pixs, const DrawCommand& cmd)
{
    const auto drawLine = [&pixs, &cmd, this](std::pair<Vertex, Vertex> trans_vpos){
        
        if (trans_vpos.first.pos.x>trans_vpos.second.pos.x)
            std::swap(trans_vpos.first, trans_vpos.second);
        VertexBrut vertsb[2]={
            {toScreenSpace(trans_vpos.first.pos), trans_vpos.first.color*255.f}, 
            {toScreenSpace(trans_vpos.second.pos), trans_vpos.second.color*255.f}
        };
        std::pair<glm::ivec2, glm::ivec2> pos={vertsb[0].pos, vertsb[1].pos};
        auto bresLine = genBresenhamLine(pos);
        int signDeltaY=glm::sign(pos.second.y-pos.first.y);
        int y=pos.first.y;
        int i=0;
        size_t iPoints=0;
        for(int i=0;i<bresLine.line.size();++i)
        {
            if (y<0)
            {
                if (signDeltaY<0)
                    break;
                else
                {
                    int ecart = -y-1;
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
                    int ecart = y-static_cast<int>(m_size.y-1)-1;
                    y=m_size.y-1;
                    i+=ecart;
                    continue;
                }
            }
            Vertex 
                lVert=interp(trans_vpos.first, trans_vpos.second, iPoints, bresLine.npoints-1),
                rVert=interp(trans_vpos.first, trans_vpos.second, iPoints+bresLine.line[i].second-bresLine.line[i].first, bresLine.npoints-1);
            draw_horizontal(pixs, y, bresLine.line[i], {lVert, rVert}, *cmd.fragShader);
            y+=signDeltaY;
            iPoints+=bresLine.line[i].second-bresLine.line[i].first+1;
        }
    };
    const VertexBuffer& vbo = *cmd.vbo;
    size_t incr=0, start=0, end=0;
    switch (cmd.vbo->type)
    {
    case VertexBuffer::Type::Lines :
        start=0;
        end=vbo.verts.size()-1;
        incr=2;
        break;
    case VertexBuffer::Type::LineFan :
        start=1;
        end=vbo.verts.size();
        incr=1;
        break;
    case VertexBuffer::Type::LineStrip :
        start=0;
        end=vbo.verts.size()-1;
        incr=1;
        break;
    }
    for (size_t iLine=start;iLine<end;iLine+=incr)
    {
        std::pair<Vertex, Vertex> trans_vpos;
        switch (cmd.vbo->type)
        {
        case VertexBuffer::Type::Lines :
        case VertexBuffer::Type::LineStrip :
            trans_vpos = {
                cmd.vertShader->get(vbo.verts[iLine+0]), 
                cmd.vertShader->get(vbo.verts[iLine+1])
            };
            break;
        case VertexBuffer::Type::LineFan :
            trans_vpos = {
                cmd.vertShader->get(vbo.verts[0]), 
                cmd.vertShader->get(vbo.verts[iLine])
            };
            break;
        }
        trans_vpos.first.pos/=trans_vpos.first.pos.w;
        trans_vpos.second.pos/=trans_vpos.second.pos.w;
        
        VertexBrut vertsb[2]={
            {toScreenSpace(trans_vpos.first.pos), trans_vpos.first.color*255.f}, 
            {toScreenSpace(trans_vpos.second.pos), trans_vpos.second.color*255.f}
        };
        drawLine(trans_vpos);
    }
}
void CustomFrame::draw_triangle(Pixels pixs, const DrawCommand& cmd)
{
    // http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    const auto flatTop = [&pixs, &cmd, this](Vertex trans_vpos[3], VertexBrut vertsb[3], size_t offset=0){
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
        size_t i=0;
        size_t iPoints1=0;
        size_t iPoints2=0;
        for(;i<maxy-offset;i++)
        {
            if (y<0)
            {
                if (signDeltaY<0)
                    break;
                else
                {
                    int ecart = -y-1;
                    if (ecart>=ligne[0].line.size()||ecart>=ligne[1].line.size())
                        break;
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
                    int ecart = y-static_cast<int>(m_size.y-1)-1;
                    if (ecart>=ligne[0].line.size()||ecart>=ligne[1].line.size())
                        break;
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
                lColor(interp<glm::ivec3, int>(vertsb[0].color, vertsb[indx[0]].color, iPoints1+glm::max(0,(ligne[0].line[i].second-ligne[0].line[i].first)), ligne[0].npoints-1)), 
                rColor(interp<glm::ivec3, int>(vertsb[0].color, vertsb[indx[1]].color, iPoints2+glm::max(0,(ligne[1].line[i].second-ligne[1].line[i].first)), ligne[1].npoints-1));//
            Vertex 
                lVert=interp(trans_vpos[0], trans_vpos[indx[0]], iPoints1+glm::max(0,(ligne[0].line[i].second-ligne[0].line[i].first)), ligne[0].npoints-1),
                rVert=interp(trans_vpos[0], trans_vpos[indx[1]], iPoints2+glm::max(0,(ligne[1].line[i].second-ligne[1].line[i].first)), ligne[1].npoints-1);
            draw_horizontal(pixs, y, {left, right}, {lVert, rVert}, *cmd.fragShader);
            y+=signDeltaY;
            iPoints1+=ligne[0].line[i].second-ligne[0].line[i].first+1;
            iPoints2+=ligne[1].line[i].second-ligne[1].line[i].first+1;
        }
        return i;
    };
    const VertexBuffer& vbo = *cmd.vbo;
    size_t nbVertsMax=vbo.verts.size()-2;
    for (size_t iTri=0;iTri<nbVertsMax;iTri+=3)
    {
        Vertex trans_vpos[3] = {
            cmd.vertShader->get(vbo.verts[iTri+0]), 
            cmd.vertShader->get(vbo.verts[iTri+1]), 
            cmd.vertShader->get(vbo.verts[iTri+2])
        };
        for (auto& v: trans_vpos)
        {
            v.w=v.pos.w;
            v.pos/=std::abs(v.pos.w);
        }
        {
            //BACKFACE CULLING
            glm::vec3 v0v1 = (trans_vpos[1].pos-trans_vpos[0].pos);
            glm::vec3 v0v2 = (trans_vpos[2].pos-trans_vpos[0].pos);
            if ((v0v1.x * v0v2.y - v0v2.x * v0v1.y)<0)
                continue;
            constexpr float vNear=-1, vFar=1;
            bool pass=false;
            for (int iComp=0;iComp<3 && !pass;++iComp)
            {
                if (trans_vpos[0].pos[iComp] <= vNear && trans_vpos[1].pos[iComp] <= vNear && trans_vpos[2].pos[iComp] <= vNear)
                    pass=true;
                if (trans_vpos[0].pos[iComp] > vFar && trans_vpos[1].pos[iComp] > vFar && trans_vpos[2].pos[iComp] > vFar)
                    pass=true;
            }
            if (pass)
                continue;
        }
        std::sort(std::begin(trans_vpos), std::end(trans_vpos), [](const Vertex& p1, const Vertex& p2) { return p1.pos.y<p2.pos.y; });
        VertexBrut vertsb[3]={
            {toScreenSpace(trans_vpos[0].pos), colorraw_t(0)}, 
            {toScreenSpace(trans_vpos[1].pos), colorraw_t(0)}, 
            {toScreenSpace(trans_vpos[2].pos), colorraw_t(0)}
        };
        if (trans_vpos[1].pos.y==trans_vpos[2].pos.y)
            flatTop(trans_vpos, vertsb);
        else if (trans_vpos[1].pos.y==trans_vpos[0].pos.y)
        {
            std::swap(trans_vpos[0], trans_vpos[2]);
            std::swap(vertsb[0], vertsb[2]);
            flatTop(trans_vpos, vertsb);
        }
        else
        {
            size_t offset = flatTop(trans_vpos, vertsb);
            std::swap(trans_vpos[0], trans_vpos[2]);
            std::swap(vertsb[0], vertsb[2]);
            flatTop(trans_vpos, vertsb, 1);
        }
    }
}
void CustomFrame::clear_image(Pixels pixs, const glm::vec3& color)
{
    size_t s=m_size.x*m_size.y;
    for(int ix=0;ix<s;++ix)
    {
        pixs.colors[ix]=color;
        pixs.zbuffer[ix]=1.f;
    }
}
Vertex VertexShader::get(Vertex vert) const
{
    const auto& projmat = get_valueref(m_projmat);
    const auto& viewmat = get_valueref(m_viewmat);
    const auto& modelmat = get_valueref(m_modelmat);
    vert.pos=projmat*viewmat*modelmat*vert.pos;
    return vert;
}
glm::vec3 FragmentShader::get(Vertex v) const
{
    return v.color;
}
