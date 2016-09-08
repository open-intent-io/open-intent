/*
|---------------------------------------------------------|
|    ___                   ___       _             _      |
|   / _ \ _ __   ___ _ __ |_ _|_ __ | |_ ___ _ __ | |_    |
|  | | | | '_ \ / _ \ '_ \ | || '_ \| __/ _ \ '_ \| __|   |
|  | |_| | |_) |  __/ | | || || | | | ||  __/ | | | |_    |
|   \___/| .__/ \___|_| |_|___|_| |_|\__\___|_| |_|\__|   |
|        |_|                                              |
|                                                         |
|     - The users first...                                |
|                                                         |
|     Authors:                                            |
|        - Clement Michaud                                |
|        - Sergei Kireev                                  |
|                                                         |
|     Version: 1.0.0                                      |
|                                                         |
|---------------------------------------------------------|

The MIT License (MIT)
Copyright (c) 2016 - Clement Michaud, Sergei Kireev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "intent/utils/Logger.hpp"

namespace intent {
namespace log {

Logger::SeverityLevel::type Logger::severityLevelFromString(
    const std::string &severity) {
  if (severity == "TRACE") {
    return Logger::SeverityLevel::TRACE;
  }

  if (severity == "DEBUG") {
    return Logger::SeverityLevel::DEBUG;
  }

  if (severity == "INFO") {
    return Logger::SeverityLevel::INFO;
  }

  if (severity == "WARNING") {
    return Logger::SeverityLevel::WARNING;
  }

  if (severity == "ERROR") {
    return Logger::SeverityLevel::_ERROR;
  }

  if (severity == "FATAL") {
    return Logger::SeverityLevel::FATAL;
  }

  return Logger::SeverityLevel::FATAL;
}

void Logger::initialize(SeverityLevel::type severityLevel) {
  spdlog::level::level_enum spdlogSeverity = spdlog::level::err;
  switch (severityLevel) {
    case SeverityLevel::TRACE:
      spdlogSeverity = spdlog::level::trace;
      break;
    case SeverityLevel::DEBUG:
      spdlogSeverity = spdlog::level::debug;
      break;
    case SeverityLevel::INFO:
      spdlogSeverity = spdlog::level::info;
      break;
    case SeverityLevel::WARNING:
      spdlogSeverity = spdlog::level::warn;
      break;
    case SeverityLevel::_ERROR:
      spdlogSeverity = spdlog::level::err;
      break;
    case SeverityLevel::FATAL:
      spdlogSeverity = spdlog::level::critical;
      break;
  }
  Logger::getInstance();

  spdlog::stdout_logger_mt("console", false);
  spdlog::set_level(spdlogSeverity);
}
}
}
