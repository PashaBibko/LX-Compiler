#ifndef UTIL_VEC2_H
#define UTIL_VEC2_H

// Includes neccessary libraries for the class

#include <type_traits>
#include <cstddef>
#include <cmath>

// Define the macro for the operator overloads (if not defined)

#ifndef VEC2_STRUCT_OVERLOADS
#define VEC2_STRUCT_OVERLOADS
#endif

// Defines operator overload macros for the vector

#ifdef VEC2_MATH_OP
#error VEC2_MATH_OP ALREADY DEFINED
#else
#define VEC2_MATH_OP(op) template<typename LHT, typename RHT> Vec2<LHT> operator op(Vec2<LHT> lhs, Vec2<RHT> rhs) { lhs.x = lhs.x op (LHT) rhs.x; lhs.y = lhs.y op (LHT) rhs.y; return lhs; }
#endif

#ifdef VEC2_EQUALITY_OP
#error VEC2_EQUALITY_OP ALREADY DEFINED
#else
#define VEC2_EQUALITY_OP(op, combine) template<typename LHT, typename RHT> bool operator op(Vec2<LHT> lhs, Vec2<RHT> rhs) { return lhs.x op (LHT)rhs.x combine lhs.y op (LHT)rhs.y; }
#endif

// Vector with two components

template <typename ItemType>
struct Vec2
{
    // Checks if the type is arithmetic
    static_assert(std::is_arithmetic<ItemType>::value, "Vec2 can only be used with arithmetic types");

    // Contents of the vector
    ItemType x, y;

    // Adds custom operator overloads defined by the project via a macro
    VEC2_STRUCT_OVERLOADS

    // Constructors

    // Default
    Vec2() : x(ItemType()), y(ItemType()) {}

    // Copy
    Vec2(ItemType xy) : x(xy), y(xy) {}
    Vec2(ItemType x, ItemType y) : x(x), y(y) {}

    // Conversion
    template <typename OtherType>
    Vec2(Vec2<OtherType> vec) : x((ItemType)vec.x), y((ItemType)vec.y) {}

    // Custom conversion from SFML (if included)

    #ifdef SFML_VECTOR2_HPP

    template<typename SFML_Type>
    Vec2(sf::Vector2<SFML_Type> vec) : x((ItemType)vec.x), y((ItemType)vec.y) {}

    template<typename SFML_Type>
    operator sf::Vector2<SFML_Type>() { return sf::Vector2<SFML_Type>((SFML_Type)x, (SFML_Type)y); }

    #endif // SFML_VECTOR2_HPP

    // Custom conversion from Box2D (if included)

    #ifdef B2_MATH_H

    Vec2(b2Vec2 vec) : x((ItemType)vec.x), y((ItemType)vec.y) {}

    operator b2Vec2() { return b2Vec2((float)x, (float)y); }

    #endif // B2_MATH_H
};

// Operator overloads using the macros

VEC2_MATH_OP(+)
VEC2_MATH_OP(-)
VEC2_MATH_OP(*)
VEC2_MATH_OP(/)

VEC2_EQUALITY_OP(==, &&)
VEC2_EQUALITY_OP(!=, ||)

// Standard library overloads

namespace std
{
    // Absolute value (Inlined)
    template<typename ItemType>
    inline Vec2<ItemType> abs(Vec2<ItemType> vec)
    {
        // Returns the absolute value of the vector
        return Vec2<ItemType>(std::abs(vec.x), std::abs(vec.y));
    }

    // Minimum value (Inlined)
    template<typename ItemType>
    inline Vec2<ItemType> min(Vec2<ItemType> lhs, Vec2<ItemType> rhs)
    {
        // Returns the minimum value of the vector
        return Vec2<ItemType>(std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y));
    }

    // Maximum value (Inlined)
    template<typename ItemType>
    inline Vec2<ItemType> max(Vec2<ItemType> lhs, Vec2<ItemType> rhs)
    {
        // Returns the maximum value of the vector
        return Vec2<ItemType>(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y));
    }
};

// End of the header guard

#endif // UTIL_VEC2_H