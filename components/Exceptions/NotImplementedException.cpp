#include <stdexcept>
#include <string>

#include "NotImplementedException.hpp"

/**
 * @brief Construct a new NotImplementedException object.
 *
 * @param function Name of the function that is not implemented.
 */
NotImplementedException::NotImplementedException(const char* function) : std::logic_error("Not Implemented")
{
    _functionName = function;
};

/**
 * @brief Get the error message.
 *
 * @return const char* Pointer to the error message.
 */
const char *NotImplementedException::what() const throw()
{
    _message = "Not Implemented: ";
    _message += _functionName;
    return _message.c_str();
}
