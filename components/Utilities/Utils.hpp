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

    /**
     * @brief Clear a specific bit in a byte.
     * 
     * @param value The byte value.
     * @param bit The bit position to clear (0-7).
     */
    void ClearBit(uint8_t *value, uint8_t bit);

    /**
     * @brief Set a specific bit in a byte.
     * @param value The byte value.
     * @param bit The bit position to set (0-7).
     */
    void SetBit(uint8_t *value, uint8_t bit);

    /**
     * @brief Check if a specific bit is set in a byte.
     * @param value The byte value.
     * @param bit The bit position to check (0-7).
     * @return true If the bit is set.
     * @return false If the bit is not set.
     */
    bool IsBitSet(uint8_t value, uint8_t bit);

    /**
     * @brief Toggle a specific bit in a byte.
     * @param value The byte value.
     * @param bit The bit position to toggle (0-7).
     */
    void ToggleBit(uint8_t *value, uint8_t bit);
}
