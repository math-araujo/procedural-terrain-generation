#ifndef HERMITE_HPP
#define HERMITE_HPP

#include <glm/glm.hpp>
#include <vector>

class CubicHermiteCurve
{
public:
    /* 
    Given N points, N tangents and N endpoints values for ranges,
    this class creates N-1 pieciewise curves using Cubic Hermite
    Interpolation. The range ]range[i-1], range[i]] is the range of the
    domain of the i-th curve, which has points[i-1] and points[i] as
    endpoints and tangents[i - 1] and tangents[i]. 
    Note: the range [range[i-1], range[i]] is closed on both sides
    for the first curve i.e. [range[0], range[1]].
    */
    CubicHermiteCurve(std::vector<glm::vec2> points, std::vector<glm::vec2> tangents, std::vector<float> ranges);
    
    CubicHermiteCurve(const CubicHermiteCurve&) = default;
    CubicHermiteCurve(CubicHermiteCurve&&) noexcept = default;
    CubicHermiteCurve& operator=(const CubicHermiteCurve&) = default;
    CubicHermiteCurve& operator=(CubicHermiteCurve&&) noexcept = default;
    ~CubicHermiteCurve() = default;

    glm::vec2 evaluate(float parameter) const;
private:
    std::vector<glm::vec2> points_;
    std::vector<glm::vec2> tangents_;
    std::vector<float> ranges_;
};

glm::vec2 cubic_hermite_interpolation(glm::vec2 start_point, glm::vec2 end_point, glm::vec2 start_tangent, glm::vec2 end_tangent, float parameter);

#endif // HERMITE_HPP