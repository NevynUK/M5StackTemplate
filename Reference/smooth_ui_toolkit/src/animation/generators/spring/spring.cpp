/**
 * @file spring.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2025-01-06
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "spring.h"
#include <cmath>
#include <algorithm>

using namespace smooth_ui_toolkit;

void Spring::setSpringOptions(float duration, float bounce, float visualDuration)
{
    // Default mass
    springOptions.mass = 1.0f;

    // Clamp function to ensure bounce is within reasonable range
    auto clamp = [](float value, float min, float max) { return std::max(min, std::min(value, max)); };

    // Ensure bounce is between 0.05 and 1
    bounce = clamp(bounce, 0.05f, 1.0f);

    // Calculate stiffness and damping based on visualDuration
    if (visualDuration > 0) {
        float root = (2 * M_PI) / (visualDuration * 1.2f); // Visual cycle
        springOptions.stiffness = root * root;             // Stiffness
        springOptions.damping =
            2 * (1.0f - bounce) * std::sqrt(springOptions.stiffness * springOptions.mass); // Damping coefficient
    } else {
        // If visualDuration is not provided, use default duration and bounce to derive
        springOptions.stiffness = (36 / (duration / 1000.0f) / (duration / 1000.0f)); // Stiffness
        springOptions.damping =
            2 * (1.0f - bounce) * std::sqrt(springOptions.stiffness * springOptions.mass); // Damping coefficient
    }
}

void Spring::init()
{
    done = false;
    value = start;

    // If or duration or visualDuration is provided, set spring options by duration/bounce-based options
    if (springOptions.duration > 0 || springOptions.visualDuration > 0) {
        setSpringOptions(springOptions.duration, springOptions.bounce, springOptions.visualDuration);
    }

    _undamped_angular_freq = std::sqrt(springOptions.stiffness / springOptions.mass);
    _damping_ratio = springOptions.damping / (2 * std::sqrt(springOptions.stiffness * springOptions.mass));

    float initialDelta = end - start;

    // Choose different formulas based on damping ratio
    if (_damping_ratio < 1) {
        float angularFreq = calc_angular_freq(_undamped_angular_freq, _damping_ratio);

        // Underdamped
        // Underdamped spring
        _resolve_spring = [=](float t) {
            float envelope = std::exp(-_damping_ratio * _undamped_angular_freq * t);

            return (end -
                    envelope * (((springOptions.velocity + _damping_ratio * _undamped_angular_freq * initialDelta) /
                                 angularFreq) *
                                    std::sin(angularFreq * t) +
                                initialDelta * std::cos(angularFreq * t)));
        };
    } else if (_damping_ratio == 1) {
        // Critically damped
        // Critically damped spring
        _resolve_spring = [=](float t) {
            return end - std::exp(-_undamped_angular_freq * t) *
                             (initialDelta + (springOptions.velocity + _undamped_angular_freq * initialDelta) * t);
        };
    } else {
        // Overdamped
        // Overdamped spring
        float dampedAngularFreq = _undamped_angular_freq * std::sqrt(_damping_ratio * _damping_ratio - 1);
        _resolve_spring = [=](float t) {
            float envelope = std::exp(-_damping_ratio * _undamped_angular_freq * t);

            // When performing sinh or cosh values can hit Infinity so we cap them here
            float freqForT = std::min(dampedAngularFreq * t, 300.0f);

            return (end -
                    (envelope * ((springOptions.velocity + _damping_ratio * _undamped_angular_freq * initialDelta) *
                                     std::sinh(freqForT) +
                                 dampedAngularFreq * initialDelta * std::cosh(freqForT))) /
                        dampedAngularFreq);
        };
    }
}

void Spring::retarget(const float& start, const float& end)
{
    springOptions.velocity = -_current_velocity;
    this->start = start;
    this->end = end;
    init();
}

bool Spring::next(const float& t)
{
    if (done) {
        return done;
    }

    // Calculate spring value based on current time
    if (_resolve_spring) {
        value = _resolve_spring(t);
    } else {
        init();
    }

    // Check if near stationary
    calc_velocity(t);
    bool isBelowVelocityThreshold = std::abs(_current_velocity) <= springOptions.restSpeed;
    bool isBelowDisplacementThreshold = std::abs(end - value) <= springOptions.restDelta;
    done = isBelowVelocityThreshold && isBelowDisplacementThreshold;

    return done;
}

void Spring::calc_velocity(const float& t)
{
    // Simple approximate velocity calculation
    const float dt = 1e-5; // Small time increment
    _current_velocity = (_resolve_spring(t + dt) - _resolve_spring(t)) / dt;
}

float Spring::calc_angular_freq(float undampedFreq, float dampingRatio)
{
    return undampedFreq * std::sqrt(1 - dampingRatio * dampingRatio);
}
