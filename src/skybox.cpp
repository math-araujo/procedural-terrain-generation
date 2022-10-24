#include <vector>

#include <glm/glm.hpp>

#include "mesh.hpp"
#include "shader.hpp"
#include "skybox.hpp"
#include "texture.hpp"

Skybox::Skybox()
{
    shader_ = std::make_unique<ShaderProgram>(std::initializer_list<std::pair<std::string_view, Shader::Type>>{
        {"assets/shaders/skybox/vertex_shader.vs", Shader::Type::Vertex},
        {"assets/shaders/skybox/fragment_shader.fs", Shader::Type::Fragment},
    });

    cubemap_ = std::make_unique<Texture>(1024, 1024, Texture::Attributes{.target = GL_TEXTURE_CUBE_MAP});

    std::vector<std::string_view> filenames{
        "assets/textures/cubemap/right.png",  "assets/textures/cubemap/left.png", "assets/textures/cubemap/top.png",
        "assets/textures/cubemap/bottom.png", "assets/textures/cubemap/back.png", "assets/textures/cubemap/front.png",
    };
    /*std::vector<std::string_view> filenames{
        "assets/textures/cloudy_cubemap/right.png",  "textures/cloudy_cubemap/left.png", "textures/cloudy_cubemap/top.png",
        "assets/textures/cloudy_cubemap/bottom.png", "textures/cloudy_cubemap/back.png", "textures/cloudy_cubemap/front.png",
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