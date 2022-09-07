#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

enum class CameraMovement
{
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down,
};

class Camera
{
public:
    Camera();
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up_direction);

    glm::mat4& view();
    const glm::mat4& view() const;
    void set_target(glm::vec3 new_target);
    void set_position(glm::vec3 new_position);
    void update_position(glm::vec3 delta_position);
private:
    glm::vec3 position_{0.0f, 0.0f, 3.0f};
    glm::vec3 target_{0.0f, 0.0f, 0.0f};
    glm::vec3 world_up_{0.0f, 1.0f, 0.0f};
    
    // The basis vectors for the camera coordinate system
    glm::vec3 right_{1.0f, 0.0f, 0.0f}; // The basis for the x (or u) direction
    glm::vec3 up_{0.0f, 1.0f, 0.0f}; // The basis for the y (or v) direction
    glm::vec3 front_{0.0f, 0.0f, 1.0f}; // The basis for the z (or w) direction

    glm::mat4 view_{1.0f};
    void update_orientation();
};

class FPSCamera
{
public:
    FPSCamera();
    FPSCamera(glm::vec3 position);
    FPSCamera(glm::vec3 position, glm::vec2 pitch_yaw_angles, glm::vec3 up_direction = glm::vec3{0.0f});

    glm::vec3& position();
    const glm::vec3& position() const;
    glm::mat4& view();
    const glm::mat4& view() const;
    float zoom() const;
    void process_keyboard_input(CameraMovement direction, float delta_time);
    void process_mouse_movement(float xoffset, float yoffset);
    void process_mouse_scroll(float yoffset);
private:
    glm::vec3 position_{0.0f, 0.0f, 3.0f};
    // Pitch and yall Euler angles; no support for Roll on this class
    glm::vec2 euler_angles_{0.0f, 0.0f};
    
    glm::vec3 world_up_{0.0f, 1.0f, 0.0f};
    glm::vec3 right_{1.0f, 0.0f, 0.0f}; // Parallel to the u-basis of the camera's coordinate system
    glm::vec3 up_{0.0f, 1.0f, 0.0f}; // Parallel to the v-basis of the camera's coordinate system
    glm::vec3 gaze_direction_{0.0f, 0.0f, -1.0f}; // Parallel but opposite of the w-basis of the camera's coordinate system i.e. gaze_direction_ = -w
    
    glm::mat4 view_{1.0f};

    // Mouse movement settings
    float speed_{5.0f};
    float mouse_sensitivity_{0.1f};
    float zoom_{45.0f};

    void update_orientation();
    void update_view_matrix();
};

#endif // CAMERA_HPP