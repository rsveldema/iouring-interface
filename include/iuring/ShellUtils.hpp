#pragma once

#include <string>

#include <iuring/ILogger.hpp>


namespace shell
{
    enum class RunOpt
    {
        LOG_ERROR_AS_WARNING,
        ABORT_ON_ERROR
    };

    void run_cmd(const std::string& cmd, logging::ILogger& logger, RunOpt opt);
}