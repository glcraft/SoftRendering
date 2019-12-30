#include "CustomFrame.h"
#include <fstream>
#include "regex_literals.h"
bool comp_start(const std::string& str, const char* start)
{
    for(int i=0;start[i]!='\0' || i>=str.size();i++)
        if (str[i]!=start[i])
            return false;
    return true;
}
bool load_model_obj(std::string path, VertexBuffer& model)
{
    using namespace regex_literals;
    std::ifstream iFile(path, std::ios::binary);
    if (!iFile)
        return false;
    std::string line;
    std::vector<glm::vec3> verts;
    std::vector<glm::vec2> uvs;
    std::smatch sm;
    model.type=VertexBuffer::Type::Triangles;
    model.verts.clear();
    while (std::getline(iFile, line))
    {
        if ("v (-?[\\d.]+) (-?[\\d.]+) (-?[\\d.]+)"_rg.search(line, sm))
            verts.emplace_back(std::stof(sm[1]), std::stof(sm[2]), std::stof(sm[3]));
        else if ("vt (-?[\\d.]+) (-?[\\d.]+)"_rg.search(line, sm))
            uvs.emplace_back(std::stof(sm[1]), 1.f-std::stof(sm[2]));
        else if ("f (\\d+)/(\\d+)(/\\d+)? (\\d+)/(\\d+)(/\\d+)? (\\d+)/(\\d+)(/\\d+)?"_rg.search(line, sm))
        {
            for (int i=0;i<9;i+=3)
            {
                int ipos = std::stoi(sm[i+1])-1;
                int iuv = std::stoi(sm[i+2])-1;
                Vertex v;
                v.pos=glm::vec4(verts[ipos], 1.f);
                v.uv=uvs[iuv];
                v.color=glm::vec3(1.f);
                model.verts.push_back(std::move(v));
            }
                
        }
    }
    return true;
}