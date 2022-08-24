#include "hermite.hpp"

#include <algorithm>
#include <cassert>

CubicHermiteCurve::CubicHermiteCurve(std::vector<glm::vec2> points, std::vector<glm::vec2> tangents, std::vector<float> ranges):
    points_{std::move(points)}, tangents_{std::move(tangents)}, ranges_{std::move(ranges)}
{
    assert(std::is_sorted(ranges.cbegin(), ranges.cend()));
}

glm::vec2 CubicHermiteCurve::evaluate(float parameter) const
{
    assert(parameter >= ranges_.front());
    assert(parameter <= ranges_.back());

    std::size_t curve_index{0};
    if (parameter == ranges_.front())
    {
        curve_index = 1;
    }
    else
    {
        auto search_iterator = std::upper_bound(ranges_.cbegin(), ranges_.cend(), parameter);
        curve_index = (search_iterator == ranges_.cend() ? ranges_.size() - 1: search_iterator - ranges_.cbegin());
    }

    const float scaled_parameter = (parameter - ranges_[curve_index - 1]) / (ranges_[curve_index] - ranges_[curve_index - 1]);
    return cubic_hermite_interpolation(points_[curve_index - 1], points_[curve_index], tangents_[curve_index - 1], tangents_[curve_index], scaled_parameter);
}

glm::vec2 cubic_hermite_interpolation(glm::vec2 start_point, glm::vec2 end_point, glm::vec2 start_tangent, glm::vec2 end_tangent, float parameter)
{
    const float parameter_squared = parameter * parameter;
    const float parameter_cubed = parameter_squared * parameter;
    return  ((2 * parameter_cubed - 3 * parameter_squared + 1) * start_point)
            + ((parameter_cubed - 2 * parameter_squared + parameter) * start_tangent)
            + ((parameter_cubed - parameter_squared) * end_tangent)
            + ((-2 * parameter_cubed + 3 * parameter_squared) * end_point);
}