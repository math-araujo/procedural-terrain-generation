#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    set_orientation();
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up_direction):
    position_{position}, target_{target}, world_up_{up_direction}
{
    set_orientation();
}

void Camera::set_orientation()
{
    front_ = glm::normalize(position_ - target_);
    right_ = glm::normalize(glm::cross(world_up_, front_));
    up_ = glm::normalize(glm::cross(front_, right_));
    view_ = glm::lookAt(position_, target_, world_up_);
}

const glm::mat4& Camera::view() const
{
    return view_;
}

void Camera::set_target(glm::vec3 new_target)
{
    target_ = new_target;
    set_orientation();
}

void Camera::set_position(glm::vec3 new_position)
{
    position_ = new_position;
    set_orientation();
}

void Camera::update_position(glm::vec3 delta_position)
{
    position_ += delta_position;
    set_orientation();
}