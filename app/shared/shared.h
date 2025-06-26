/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <queue>
#include <functional>
#include <smooth_ui_toolkit.h>

/**
 * @brief Shared data layer, provides a global shared data singleton with mutex
 */
namespace shared_data
{
    /**
     * @brief Shared data definition
     *
     */
    struct SharedData_t
    {
        smooth_ui_toolkit::Signal<std::string> systemStateEvents;
        smooth_ui_toolkit::Signal<std::string> inputEvents;
    };

    /**
     * @brief Get shared data instance
     *
     * @return SharedData_t&
     */
    SharedData_t *Get();

    /**
     * @brief Destroy shared data instance
     */
    void Destroy();

} // namespace shared_data

/**
 * @brief Get shared data instance
 *
 * @return SharedData_t&
 */
inline shared_data::SharedData_t *GetSharedData()
{
    return shared_data::Get();
}

inline smooth_ui_toolkit::Signal<std::string> &GetSystemStateEvents()
{
    return GetSharedData()->systemStateEvents;
}

inline smooth_ui_toolkit::Signal<std::string> &GetInputEvents()
{
    return GetSharedData()->inputEvents;
}
