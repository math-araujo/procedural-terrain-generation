#include "water.hpp"

#include <cmath>

#include "framebuffer.hpp"
#include "renderbuffer.hpp"
#include "texture.hpp"

Water::Water()
{
    reflection_fbo_ = std::make_unique<Framebuffer>(
        reflection_width_, reflection_height_,
        Renderbuffer{reflection_width_, reflection_height_, GL_DEPTH_COMPONENT32F},
        Texture
        {
            reflection_width_, reflection_height_,
            Texture::Attributes{.wrap_s = GL_REPEAT, .wrap_t = GL_REPEAT}
        }  
    );

    refraction_fbo_ = std::make_unique<Framebuffer>(
        refraction_width_, refraction_height_,
        Texture
        {refraction_width_, refraction_height_, 
        Texture::Attributes
        {
            .internal_format = GL_DEPTH_COMPONENT32F, 
            .pixel_data_format = GL_DEPTH_COMPONENT,
            .pixel_data_type = GL_FLOAT
        }
        },
        Texture
        {
            refraction_width_, refraction_height_,
            Texture::Attributes{.wrap_s = GL_REPEAT, .wrap_t = GL_REPEAT}
        }
    );
}

void Water::update(float delta_time)
{
    dudv_offset_ += wave_speed_ * delta_time;
    dudv_offset_ = fmodf(dudv_offset_, 1.0f);
}

void Water::bind_reflection()
{
    reflection_fbo_->bind();
}

void Water::bind_refraction()
{
    refraction_fbo_->bind();
}

void Water::unbind()
{
    reflection_fbo_->unbind();
}

float Water::height() const
{
    return height_;
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
    return reflection_fbo_->color_id();
}

std::uint32_t Water::refraction_color_attachment() const
{
    return refraction_fbo_->color_id();
}

std::uint32_t Water::refraction_depth_texture() const
{
    return refraction_fbo_->depth_id();
}

void Water::bind_textures()
{
    reflection_fbo_->bind_color(0);
    refraction_fbo_->bind_color(1);
    refraction_fbo_->bind_depth_texture(4);
}