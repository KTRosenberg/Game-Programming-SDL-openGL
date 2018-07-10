#ifndef TYPES_H
#define TYPES_H

#include "common_utils.h"
#include "opengl.hpp"

#include <glm/glm.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

// alias, actual type name 
// (I may remove the latter if I don't add any types that require a different name)
#define PROPERTY_KINDS \
    KIND(i64, i64) \
    KIND(u64, u64) \
    KIND(usize, usize) \
    KIND(f64, f64) \
    KIND(i32, i32) \
    KIND(u32, i32) \
    KIND(f32, i32) \
    KIND(i16, i16) \
    KIND(u16, u16) \
    KIND(i8, i8) \
    KIND(u8, u8) \
    KIND(bool, bool) \
    KIND(char, char) \
    KIND(ucharptr, ucharptr) \
    KIND(vec2, vec2) \
    KIND(vec3, vec3) \
    KIND(vec4, vec4) \

enum struct PROPERTY_TYPE {
    #define KIND(a, b) PASTE(PROP_, a) ,
    PROPERTY_KINDS
    COUNT_PROPERTY_TYPE
    #undef KIND
};

#endif
