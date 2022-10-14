#include "water.hpp"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "light.hpp"

Water::Water(int plane_scale) : plane_scale_{static_cast<float>(plane_scale)}
{
    compute_model_matrix();
}

void Water::compute_model_matrix()
{
    model_ = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, height_, 0.0f});
    model_ = glm::rotate(model_, glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f});
    model_ = glm::scale(model_, glm::vec3{plane_scale_, plane_scale_, 1.0f});
}

void Water::update(float delta_time)
{
    dudv_offset_ += wave_speed_ * delta_time;
    dudv_offset_ = fmodf(dudv_offset_, 1.0f);
}

void Water::render(FPSCamera& camera)
{
    shader_program_.use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader_program_.set_mat4_uniform("mvp", camera.view_projection() * model_);
    shader_program_.set_mat4_uniform("model", model_);
    shader_program_.set_vec3_uniform("camera_position", camera.position());
    shader_program_.set_float_uniform("dudv_offset", dudv_offset_);
    shader_program_.set_float_uniform("near_plane", 0.1f);
    shader_program_.set_float_uniform("far_plane", 1000.0f);
    reflection_fbo_.bind_color(0);
    refraction_fbo_.bind_color(1);
    dudv_map_.bind(2);
    normal_map_.bind(3);
    refraction_fbo_.bind_depth_texture(4);
    mesh_.render();
    glDisable(GL_BLEND);
}

void Water::bind_reflection()
{
    reflection_fbo_.bind();
}

void Water::bind_refraction()
{
    refraction_fbo_.bind();
}

void Water::unbind()
{
    reflection_fbo_.unbind();
}

float Water::height() const
{
    return height_;
}

void Water::set_height(float new_height)
{
    height_ = new_height;
    reflection_clip_plane_ = glm::vec4{0.0f, 1.0f, 0.0f, -height_ + 0.1f};
    refraction_clip_plane_ = glm::vec4{0.0f, -1.0f, 0.0f, height_ + 0.2f};
    compute_model_matrix();
}

float Water::dudv_offset() const
{
    return dudv_offset_;
}

const glm::vec4& Water::reflection_clip_plane() const
{
    return reflection_clip_plane_;
}

const glm::vec4& Water::refraction_clip_plane() const
{
    return refraction_clip_plane_;
}

std::uint32_t Water::reflection_color_attachment() const
{
    return reflection_fbo_.color_id();
}

std::uint32_t Water::refraction_color_attachment() const
{
    return refraction_fbo_.color_id();
}

std::uint32_t Water::refraction_depth_texture() const
{
    return refraction_fbo_.depth_id();
}

void Water::update_light(const DirectionalLight& light)
{
    shader_program_.set_vec3_uniform("light.direction", light.direction);
    shader_program_.set_vec3_uniform("light.ambient", light.ambient);
    shader_program_.set_vec3_uniform("light.diffuse", light.diffuse);
    shader_program_.set_vec3_uniform("light.specular", light.specular);
}