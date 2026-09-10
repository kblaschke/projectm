#pragma once

#include <exception>
#include <string>

namespace libprojectM {
namespace UserSprites {

/**
 * @brief Exception for sprite loading errors.
 */
class SpriteException : public std::exception
{
public:
    explicit SpriteException(std::string message)
        : m_message(std::move(message))
    {
    }

    ~SpriteException() override = default;

    const char* what() const noexcept override
    {
        return m_message.c_str();
    }

    const std::string& message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};


} // namespace UserSprites
} // namespace libprojectM
