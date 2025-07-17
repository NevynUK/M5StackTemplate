/**
 * @file cubic_bezier.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2025-01-06
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "cubic_bezier.h"
#include <functional>
#include <cmath>

static const float subdivisionPrecision = 0.0000001;
static const int subdivisionMaxIterations = 12;

// Calculate Bezier curve value
static float calcBezier(float t, float a1, float a2)
{
    return (((1.0 - 3.0 * a2 + 3.0 * a1) * t + (3.0 * a2 - 6.0 * a1)) * t + 3.0 * a1) * t;
}

// Use binary search to approximate and find the t value corresponding to given x
static float binarySubdivide(float x, float lowerBound, float upperBound, float mX1, float mX2)
{
    float currentX, currentT;
    int i = 0;

    do {
        currentT = lowerBound + (upperBound - lowerBound) / 2.0;
        currentX = calcBezier(currentT, mX1, mX2) - x;

        if (currentX > 0.0) {
            upperBound = currentT;
        } else {
            lowerBound = currentT;
        }
    } while (std::abs(currentX) > subdivisionPrecision && ++i < subdivisionMaxIterations);

    return currentT;
}

std::function<float(float)> smooth_ui_toolkit::cubic_bezier(float mX1, float mY1, float mX2, float mY2)
{
    // If the curve is linear (control points form a straight line), return linear function directly
    if (mX1 == mY1 && mX2 == mY2) {
        return [](float t) { return t; };
    }

    // Calculate t corresponding to given x
    auto getTForX = [mX1, mX2](float aX) { return binarySubdivide(aX, 0.0, 1.0, mX1, mX2); };

    // Return Bezier easing function
    return [mX1, mY1, mX2, mY2, getTForX](float t) {
        if (t == 0.0 || t == 1.0) {
            return t;
        }
        float bezierT = getTForX(t);
        return calcBezier(bezierT, mY1, mY2);
    };
}
