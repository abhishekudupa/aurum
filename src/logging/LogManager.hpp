// LogManager.hpp ---
//
// Filename: LogManager.hpp
// Author: Abhishek Udupa
// Created: Fri May 15 11:53:03 2015 (-0400)
//
//
// Copyright (c) 2015, Abhishek Udupa, University of Pennsylvania
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by The University of Pennsylvania
// 4. Neither the name of the University of Pennsylvania nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//

// Code:

#if !defined AURUM_LOGGING_LOG_MANAGER_HPP_
#define AURUM_LOGGING_LOG_MANAGER_HPP_

#include "../containers/UnorderedSet.hpp"
#include "../containers/UnorderedMap.hpp"

#include <fstream>
#include <string>

namespace aurum {
namespace logging {

namespace ac = aurum::containers;

typedef ac::UnifiedUnorderedSet<std::string> LogOptionSet;
typedef ac::UnifiedUnorderedMap<std::string, std::string> LogOptionDescMap;

class LogManager
{
private:
    static LogOptionSet& get_the_enabled_log_options_obj();
    static LogOptionDescMap& get_the_allowed_log_options_obj();
    static std::ostream*& get_the_logging_stream_obj();
    static const std::string& get_the_no_description_found_string();

    LogManager() = delete;
    LogManager(const LogManager& other) = delete;
    LogManager(LogManager&& other) = delete;

    LogManager& operator = (const LogManager& other) = delete;
    LogManager& operator = (LogManager&& other) = delete;

public:
    static void initialize(const std::string& log_stream_name = "");
    static void finalize();

    static std::ostream& get_logging_stream();

    static void register_logging_option(const std::string& option_name,
                                        const std::string& option_desc);
    static const std::string& describe_logging_option(const std::string& option_name);
    static const LogOptionDescMap& get_logging_option_descriptions();

    static void enable_logging_option(const std::string& option_name);
    static void disable_logging_option(const std::string& option_name);

    template<typename InputIterator>
    static inline void enable_logging_options(const InputIterator& first,
                                              const InputIterator& last)
    {
        for (auto it = first; it != last; ++it) {
            enable_logging_option(*it);
        }
    }

    template<typename InputIterator>
    static inline void disable_logging_options(const InputIterator& first,
                                               const InputIterator& last)
    {
        for (auto it = first; it != last; ++it) {
            disable_logging_option(*it);
        }
    }

    static const LogOptionSet& get_enabled_logging_options();
    static bool is_logging_option_enabled(const std::string& option_name);
};

} /* end namespace logging */
} /* end namespace aurum */

#ifdef AURUM_CFG_LOGGING_ENABLED_
#define AURUM_LOG_CODE(CODE_) { CODE_ } ((void)0)
#else /* !AURUM_CFG_LOGGING_ENABLED_ */
#define AURUM_LOG_CODE(CODE_) ((void)0)
#endif /* AURUM_CFG_LOGGING_ENABLED_ */

#define AURUM_LOG_FULL(TAG_, CODE_)                                            \
    AURUM_LOG_CODE(                                                            \
    if (aurum::logging::LogManager::is_logging_option_enabled(TAG_)) {         \
        ostream& logstream_ = aurum::logging::LogManager::get_logging_stream();\
        logstream_ << "------------[" << TAG_ << "], at " << __FUNCTION__      \
                   << ", " << __FILE__ << ":" << __LINE__ << "------------"    \
                   << endl;                                                    \
        CODE_                                                                  \
        logstream_ << "------------------------------------------------------" \
                   << endl;                                                    \
        logstream_.flush()                                                     \
    })

#define AURUM_LOG_SHORT(TAG_, CODE_)                                           \
    AURUM_LOG_CODE(                                                            \
    if (aurum::logging::LogManager::is_logging_option_enabled(TAG_)) {         \
        ostream& logstream_ = aurum::logging::LogManager::get_logging_stream();\
        CODE_                                                                  \
        logstream_.flush();                                                    \
    })

#define AURUM_LOG_FULL_COND(TAG_, CODE_, COND_)                                \
    AURUM_LOG_CODE(                                                            \
    if (aurum::logging::LogManager::is_logging_option_enabled(TAG_) &&         \
        (COND_)) {                                                             \
        ostream& logstream_ = aurum::logging::LogManager::get_logging_stream();\
        logstream_ << "------------[" << TAG_ << "], at " << __FUNCTION__      \
                   << ", " << __FILE__ << ":" << __LINE__ << "------------"    \
                   << endl;                                                    \
        CODE_                                                                  \
        logstream_ << "------------------------------------------------------" \
                   << endl;                                                    \
        logstream_.flush()                                                     \
    })

#define AURUM_LOG_SHORT_COND(TAG_, CODE_, COND_)                               \
    AURUM_LOG_CODE(                                                            \
    if (aurum::logging::LogManager::is_logging_option_enabled(TAG_) &&         \
        (COND_)) {                                                             \
        ostream& logstream_ = aurum::logging::LogManager::get_logging_stream();\
        CODE_                                                                  \
        logstream_.flush();                                                    \
    })

#endif /* AURUM_LOGGING_LOG_MANAGER_HPP_ */

//
// LogManager.hpp ends here
