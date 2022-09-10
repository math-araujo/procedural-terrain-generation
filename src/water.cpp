#include "water.hpp"

#include "framebuffer.hpp"
#include "renderbuffer.hpp"
#include "texture.hpp"

Water::Water()
{
    reflection_fbo_ = std::make_unique<Framebuffer>(
        reflection_width_, reflection_height_,
        Renderbuffer{reflection_width_, reflection_height_, GL_DEPTH_COMPONENT32F},
        Texture{reflection_width_, reflection_height_}  
    );
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

std::uint32_t Water::reflection_color_attachment() const
{
    return reflection_fbo_->color_id();
}

std::uint32_t Water::refraction_color_attachment() const
{
    return refraction_fbo_->color_id();
}