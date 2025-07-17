/**
 * @file spring.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2025-01-06
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include "../generators.h"
#include <functional>
// Reference:
// https://github.com/motiondivision/motion/blob/main/packages/framer-motion/src/animation/generators/spring

namespace smooth_ui_toolkit {

struct SpringOptions_t {
    float stiffness = 100.0;    // Spring stiffness coefficient
    float damping = 10.0;       // Damping coefficient
    float mass = 1.0;           // Mass
    float velocity = 0.0;       // Initial velocity
    float restSpeed = 0.1;      // Rest speed threshold
    float restDelta = 0.1;      // Rest position threshold
    float duration = 0.0;       // Animation duration in ms
    float bounce = 0.3;         // Bounce coefficient 0.05~1.0
    float visualDuration = 0.0; // Visual duration
};

class Spring : public KeyFrameGenerator {
public:
    Spring() {}
    ~Spring() {}

    SpringOptions_t springOptions;

    /**
     * @brief Set spring options by duration/bounce-based options
     *
     * @param duration in ms
     * @param bounce 0.05~1.0
     * @param visualDuration in seconds
     */
    void setSpringOptions(float duration = 800.0f, float bounce = 0.3f, float visualDuration = 0.3f);
    inline void setSpringOptions(const SpringOptions_t& options)
    {
        springOptions = options;
    }

    virtual void init() override;
    virtual void retarget(const float& start, const float& end) override;
    virtual bool next(const float& t) override;
    virtual animation_type::Type_t type() const override
    {
        return animation_type::spring;
    }

protected:
    float _damping_ratio;                        // Damping ratio
    float _undamped_angular_freq;                // Undamped angular frequency
    float _current_velocity;                     // Current velocity
    std::function<float(float)> _resolve_spring; // Animation calculation formula

    void calc_velocity(const float& t);
    float calc_angular_freq(float undampedFreq, float dampingRatio);
};

} // namespace smooth_ui_toolkit
