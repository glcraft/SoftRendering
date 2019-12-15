#include <gl/glew.h>
#include <glm/glm.hpp>
class CustomFrame
{
public:
    CustomFrame();
    ~CustomFrame();

    void blit(GLuint fboDestination, glm::vec2 pos1, glm::vec2 pos2);

    void clear(glm::vec3 color);
    void clear(glm::tvec3<uint8_t> color);
    void draw_line(glm::ivec2 pos1, glm::ivec2 pos2, glm::tvec3<uint8_t> color);
    void apply();
private:
    GLuint PBO;
    GLuint FBO;
    GLuint texture;
};