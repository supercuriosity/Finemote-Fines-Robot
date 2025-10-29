/*******************************************************************************
* Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_CLAMP_HPP
#define FINEMOTE_CLAMP_HPP

#include <utility>

template <typename T>
T& Clamp(T& value, const T& min, const T& max) {
 static_assert(std::is_lvalue_reference<T&>::value, "value must be a reference (lvalue)");
 if (value < min) value = min;
 else if (value > max) value = max;
 return value;
}

template <typename T>
T Clamp(T&& value, const T& min, const T& max) {
 static_assert(!std::is_lvalue_reference<T&&>::value, "value must be temporary (rvalue)");
 if (value < min) value = min;
 else if (value > max) value = max;
 return value;
}

#endif
