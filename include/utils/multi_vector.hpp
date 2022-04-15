////////////////////////////////////////////////////////////////////////////////
/*
 * File: multi_vector.hpp
 *
 * @brief Multi dimensional arrays aliases.
 *
 * Created on July 16, 2021, 09:14 AM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_MULTI_VECTOR_HPP
#define UTILS_MULTI_VECTOR_HPP

#include <vector>

namespace utils
{

template <class T>
using Vec2D = std::vector<std::vector<T>>;

template <class T>
using Vec3D = std::vector<Vec2D<T>>;

template <class T>
using Vec4D = std::vector<Vec3D<T>>;

} // namespace utils

#endif // UTILS_MULTI_VECTOR_HPP
