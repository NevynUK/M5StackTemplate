/**
 * @file Utils.hpp
 * @author Mark Stevens
 * @brief Utility functions.
 * @date 2025-05-25
 * 
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "sdkconfig.h"

#include <string>

namespace Utils
{
    /**
     * @brief Format a number with commas as thousands separators.
     * 
     * @param number The number to format.
     * @return std::string The formatted number.
     */
    std::string NumberWithCommas(uint32_t number);

    /**
     * @brief Format a number with commas as thousands separators.
     * 
     * @param number The number to format.
     * @return std::string The formatted number.
     */
    std::string NumberWithCommas(uint64_t number);
}
