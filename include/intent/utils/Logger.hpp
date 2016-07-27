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
#ifndef INTENT_LOGGER_HPP
#define INTENT_LOGGER_HPP

#include <iostream>

#define BOOST_LOG_DYN_LINK

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#endif

#include <string>
#include <sstream>

namespace intent {
namespace log {

class Logger {
 public:
  /**
   * \brief The severity levels handled by the logger
   */
  struct SeverityLevel {
    enum type { TRACE, DEBUG, INFO, WARNING, ERROR, FATAL };
  };

  /**
   * @brief Initialize the logging system with the maximum severity level to
   * use.
   * \param severityLevel     The maximum severity level to use.
   */
  static void initialize(SeverityLevel::type severityLevel);

  /**
   * @brief Return the severity type for string. Return FATAL if nothing is
   * matching.
   * @param severity The string to convert into severity level
   * @return the severity level
   */
  static SeverityLevel::type severityLevelFromString(
      const std::string& severity);

  /**
   * @brief Get the static instance of the logger
   * \return The logger instance
   */
  static Logger& getInstance() {
    static Logger* logger = NULL;
    if (logger == NULL) {
      logger = new Logger();
      SeverityLevel::type initialLevel = SeverityLevel::FATAL;
      if (const char* env_p = std::getenv("LOG_LEVEL")) {
        initialLevel = Logger::severityLevelFromString(env_p);
      }
      initialize(initialLevel);
    }
    return *logger;
  }

  /**
   * \brief Logger by level of severity.
   */
  template <boost::log::trivial::severity_level LogLevel>
  class LoggerWithLevel {
   public:
    /**
     * \brief Log a string message.
     */
    LoggerWithLevel& operator<<(const std::string& message) {
      BOOST_LOG_STREAM_WITH_PARAMS(
          ::boost::log::trivial::logger::get(),
          (::boost::log::keywords::severity = LogLevel))
          << message;
      return *this;
    }

    /**
     * \brief Log a message from a string stream.
     */
    LoggerWithLevel& operator<<(const std::stringstream& message) {
      BOOST_LOG_STREAM_WITH_PARAMS(
          ::boost::log::trivial::logger::get(),
          (::boost::log::keywords::severity = LogLevel))
          << message.str();
      return *this;
    }

    /**
     * \brief Log a message from an object that supports the stream operator.
     */
    template <typename T>
    LoggerWithLevel& operator<<(T& message) {
      BOOST_LOG_STREAM_WITH_PARAMS(
          ::boost::log::trivial::logger::get(),
          (::boost::log::keywords::severity = LogLevel))
          << message;
      return *this;
    }
  };

  /**
   * @brief Get the trace logger
   * \return the trace logger
   */
  inline LoggerWithLevel<boost::log::trivial::trace>& trace() {
    return m_loggerTrace;
  }

  /**
   * @brief Get the debug logger
   * \return the debug logger
   */
  inline LoggerWithLevel<boost::log::trivial::debug>& debug() {
    return m_loggerDebug;
  }

  /**
   * @brief Get the info logger
   * \return the info logger
   */
  inline LoggerWithLevel<boost::log::trivial::info>& info() {
    return m_loggerInfo;
  }

  /**
   * @brief Get the warning logger
   * \return the warning logger
   */
  inline LoggerWithLevel<boost::log::trivial::warning>& warning() {
    return m_loggerWarning;
  }

  /**
   * @brief Get the error logger
   * \return the error logger
   */
  inline LoggerWithLevel<boost::log::trivial::error>& error() {
    return m_loggerError;
  }

  /**
   * @brief Get the fatal logger
   * \return the fatal logger
   */
  inline LoggerWithLevel<boost::log::trivial::fatal>& fatal() {
    return m_loggerFatal;
  }

 private:
  Logger() {}

  LoggerWithLevel<boost::log::trivial::trace> m_loggerTrace;
  LoggerWithLevel<boost::log::trivial::debug> m_loggerDebug;
  LoggerWithLevel<boost::log::trivial::info> m_loggerInfo;
  LoggerWithLevel<boost::log::trivial::warning> m_loggerWarning;
  LoggerWithLevel<boost::log::trivial::error> m_loggerError;
  LoggerWithLevel<boost::log::trivial::fatal> m_loggerFatal;
};
}
}

#define LOG_TRACE() intent::log::Logger::getInstance().trace()
#define LOG_DEBUG() intent::log::Logger::getInstance().debug()
#define LOG_INFO() intent::log::Logger::getInstance().info()
#define LOG_WARNING() intent::log::Logger::getInstance().warning()
#define LOG_ERROR() intent::log::Logger::getInstance().error()
#define LOG_FATAL() intent::log::Logger::getInstance().fatal()
