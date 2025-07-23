#include <stdexcept>
#include <string>

/**
 * @brief Exception thrown when a function is not implemented.
 */
class NotImplementedException : public std::logic_error
{
private:
    std::string _functionName;
    mutable std::string _message;

    //
    //  Delete the default constructor and copy/move constructors and assignment operators.
    //
    NotImplementedException() = delete;
    NotImplementedException(const NotImplementedException &) = delete;
    NotImplementedException &operator=(const NotImplementedException &) = delete;
    NotImplementedException(NotImplementedException &&) = delete;
    NotImplementedException &operator=(NotImplementedException &&) = delete;

public:
    /**
     * @brief Construct a new NotImplementedException object.
     *
     * @param function Name of the function that is not implemented.
     */
    explicit NotImplementedException(const char *function);

    /**
     * @brief Get the error message.
     *
     * @return const char* Pointer to the error message.
     */
    const char *what() const throw();
};