#include "CustomTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdexcept>
	
void CustomTexture::load (std::string path)
{
    int x, y, comp;
    unsigned char* ucColors = stbi_load(path.c_str(),&x,&y,&comp,3);
    if (!ucColors)
        throw std::runtime_error("Unable to load image file");
    m_size=glm::uvec2(x,y);
    m_colors.resize(x*y);

    for(int i=0, iuc=0;i<m_colors.size();i++, iuc+=3)
        m_colors[i]=glm::vec3(
            static_cast<float>(ucColors[iuc+0])/255.f,
            static_cast<float>(ucColors[iuc+1])/255.f,
            static_cast<float>(ucColors[iuc+2])/255.f
        );
    stbi_image_free(ucColors);
}
glm::vec3 CustomTexture::get(glm::vec2 uv) const
{
    uv = glm::clamp(uv, 0.f, 1.f); // clamp to edge
    glm::uvec2 realPos = uv*glm::vec2(m_size-glm::uvec2(1));
    return m_colors[realPos.x+realPos.y*m_size.x];
}