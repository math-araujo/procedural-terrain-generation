#include "camera.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    update_orientation();
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up_direction):
    position_{position}, target_{target}, world_up_{up_direction}
{
    update_orientation();
}

void Camera::update_orientation()
{
    front_ = glm::normalize(position_ - target_);
    right_ = glm::normalize(glm::cross(world_up_, front_));
    up_ = glm::normalize(glm::cross(front_, right_));
    view_ = glm::lookAt(position_, target_, world_up_);
}

glm::mat4& Camera::view()
{
    return view_;
}

const glm::mat4& Camera::view() const
{
    return view_;
}

void Camera::set_target(glm::vec3 new_target)
{
    target_ = new_target;
    update_orientation();
}

void Camera::set_position(glm::vec3 new_position)
{
    position_ = new_position;
    update_orientation();
}

void Camera::update_position(glm::vec3 delta_position)
{
    position_ += delta_position;
    update_orientation();
}

FPSCamera::FPSCamera()
{
    update_orientation();
}

FPSCamera::FPSCamera(glm::vec3 position, glm::vec2 pitch_yaw_angles, glm::vec3 up_direction):
    position_{position}, euler_angles_{pitch_yaw_angles}, world_up_{up_direction}
{
    update_orientation();
}

void FPSCamera::update_orientation()
{
    // Note: the negative sign in the z-coordinate of the gaze_direction_
    // makes the camera look in the -w-direction, where w is the basis vector
    // of the camera's coordinate system.
    gaze_direction_ = glm::normalize(glm::vec3
    {
        // Pitch component                        // Yaw component
        glm::cos(glm::radians(euler_angles_.x)) * glm::sin(glm::radians(euler_angles_.y)),
        glm::sin(glm::radians(euler_angles_.x)),
        -1.0f * glm::cos(glm::radians(euler_angles_.x)) * glm::cos(glm::radians(euler_angles_.y))
    });

    // The cross product seems inverted because gaze_direction_ has the opposite
    // direction of the w-basis. This works because cross(-w, v) = -cross(v, -w) = -(-u) = u,
    // where uvw (right, up, forward) are the basis for the camera's coordinate system.
    right_ = glm::normalize(glm::cross(gaze_direction_, world_up_));
    up_ = glm::normalize(glm::cross(right_, gaze_direction_));
    update_view_matrix();
}

void FPSCamera::update_view_matrix()
{
    view_ = glm::lookAt(position_, position_ + gaze_direction_, world_up_);
}

glm::vec3& FPSCamera::position()
{
    return position_;
}

const glm::vec3& FPSCamera::position() const
{
    return position_;
}

glm::mat4& FPSCamera::view()
{
    return view_;
}

const glm::mat4& FPSCamera::view() const
{
    return view_;
}

float FPSCamera::zoom() const
{
    return zoom_;
}

void FPSCamera::process_keyboard_input(CameraMovement direction, float delta_time)
{
    const float velocity = speed_ * delta_time;
    glm::vec3 delta_position{0.0f};
    if (direction == CameraMovement::Forward)
    {
        delta_position += gaze_direction_ * velocity;
    }

    if (direction == CameraMovement::Backward)
    {
        delta_position -= gaze_direction_ * velocity;
    }

    if (direction == CameraMovement::Right)
    {
        delta_position += right_ * velocity;
    }

    if (direction == CameraMovement::Left)
    {
        delta_position -= right_ * velocity;
    }

    delta_position.y = 0.0f;

    if (direction == CameraMovement::Up)
    {
        delta_position.y += velocity;
    }

    if (direction == CameraMovement::Down)
    {
        delta_position.y -= velocity;
    }

    position_ += delta_position;
    position_.y = std::max(position_.y, 0.0f);
    
    update_view_matrix();
}

void FPSCamera::process_mouse_movement(float xoffset, float yoffset)
{
    euler_angles_.y += xoffset * mouse_sensitivity_;
    euler_angles_.x += yoffset * mouse_sensitivity_;
    euler_angles_.x = std::max(std::min(euler_angles_.x, 89.9f), -89.9f);
    update_orientation();
}

void FPSCamera::process_mouse_scroll(float yoffset)
{
    zoom_ -= yoffset;
    zoom_ = std::max(std::min(zoom_, 45.0f), 1.0f);
}