
#include <glm/glm.hpp>
#include <string>
#include <vector>
class CustomTexture
{
public:
    void load (std::string path);
    glm::vec3 get(glm::vec2 uv) const;
    glm::uvec2 getSize() const {return m_size;}
    const std::vector<glm::vec3>& getPixels() const {return m_colors;}
private:
    std::vector<glm::vec3> m_colors;
    glm::uvec2 m_size=glm::uvec2(0);
};