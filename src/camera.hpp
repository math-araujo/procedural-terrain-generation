#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up_direction);

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
    void set_orientation();
};

#endif // CAMERA_HPP