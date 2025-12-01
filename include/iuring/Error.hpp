#pragma once

/**
 * @file Error.hpp
 * @brief Defines error codes and conversion functions for network operations.
 */

namespace iuring
{
    
enum class Error
{
    OK,
    RANGE,
    MMAP_FAILED,
    FAILED_TO_CREATE_SOCKET,
    FAILED_TO_OPEN_PCM,
    ALSA_FAILURE,
    NO_ALSA_CAPTURE,
    NO_ALSA_PLAYBACK,
    UNKNOWN
};

Error errno_to_error(int err);

}