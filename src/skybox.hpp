#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <memory>
#include <glm/fwd.hpp>

class Mesh;
class ShaderProgram;
class Texture;

class Skybox
{
public:
    Skybox();

    void render(const glm::mat4& projection, const glm::mat4& view);
private:
    std::unique_ptr<ShaderProgram> shader_{};
    std::unique_ptr<Texture> cubemap_{};
    std::unique_ptr<Mesh> mesh_;
};

#endif // SKYBOX_HPP