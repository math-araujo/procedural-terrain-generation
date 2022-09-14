#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool to_update;
};

#endif // LIGHT_HPP