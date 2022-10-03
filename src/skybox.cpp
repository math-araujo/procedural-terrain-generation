#include <vector>

#include <glm/glm.hpp>

#include "mesh.hpp"
#include "shader.hpp"
#include "skybox.hpp"
#include "texture.hpp"

Skybox::Skybox()
{
    shader_ = std::make_unique<ShaderProgram>(std::initializer_list<std::pair<std::string_view, Shader::Type>>{
        {"shaders/skybox/vertex_shader.vs", Shader::Type::Vertex},
        {"shaders/skybox/fragment_shader.fs", Shader::Type::Fragment},
    });

    cubemap_ = std::make_unique<Texture>(1024, 1024, Texture::Attributes{.target = GL_TEXTURE_CUBE_MAP});

    std::vector<std::string_view> filenames{
        "textures/cubemap/right.png",  "textures/cubemap/left.png", "textures/cubemap/top.png",
        "textures/cubemap/bottom.png", "textures/cubemap/back.png", "textures/cubemap/front.png",
    };
    /*std::vector<std::string_view> filenames{
        "textures/cloudy_cubemap/right.png",  "textures/cloudy_cubemap/left.png", "textures/cloudy_cubemap/top.png",
        "textures/cloudy_cubemap/bottom.png", "textures/cloudy_cubemap/back.png", "textures/cloudy_cubemap/front.png",
    };*/

    cubemap_->load_cubemap(filenames, false);

    // clang-format off
    mesh_ = std::make_unique<Mesh>(
        std::vector<float>
        {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        },
        std::vector<int>{3}
    );
    // clang-format on
}

void Skybox::render(const glm::mat4& projection, const glm::mat4& view)
{
    glDepthFunc(GL_LEQUAL);
    shader_->use();
    shader_->set_mat4_uniform("view_projection", projection * glm::mat4{glm::mat3{view}});
    cubemap_->bind(0);
    mesh_->render();
    glDepthFunc(GL_LESS);
}