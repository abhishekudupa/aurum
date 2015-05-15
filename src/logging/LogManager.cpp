// LogManager.cpp ---
//
// Filename: LogManager.cpp
// Author: Abhishek Udupa
// Created: Fri May 15 13:18:39 2015 (-0400)
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

#include "LogManager.hpp"
#include <iostream>

namespace aurum {
namespace logging {

LogOptionSet& LogManager::get_the_enabled_log_options_obj()
{
    static LogOptionSet s_the_log_options;
    return s_the_log_options;
}

std::ostream*& LogManager::get_the_logging_stream_obj()
{
    static std::ostream* s_the_logging_stream = &std::cout;
    return s_the_logging_stream;
}

LogOptionDescMap& LogManager::get_the_allowed_log_options_obj()
{
    static LogOptionDescMap s_the_log_options_descriptions;
    return s_the_log_options_descriptions;
}

const std::string& LogManager::get_the_no_description_found_string()
{
    static std::string sc_no_description_found_string("No Description Found");
    return sc_no_description_found_string;
}

void LogManager::initialize(const std::string& log_stream_name)
{
    auto& the_logging_stream = get_the_logging_stream_obj();
    if (the_logging_stream != nullptr) {
        finalize();
    }

    if (log_stream_name == "") {
        the_logging_stream = &std::cout;
    } else {
        the_logging_stream = new std::ofstream(log_stream_name.c_str());
    }
}

void LogManager::finalize()
{
    auto& the_logging_stream = get_the_logging_stream_obj();
    if (the_logging_stream == &std::cout) {
        the_logging_stream->flush();
        return;
    }
    the_logging_stream->flush();
    delete the_logging_stream;
    the_logging_stream = nullptr;

    // clear all the options
    get_the_enabled_log_options_obj().clear();
    get_the_allowed_log_options_obj().clear();
}

std::ostream& LogManager::get_logging_stream()
{
    return (*(get_the_logging_stream_obj()));
}

void LogManager::register_logging_option(const std::string& option_name,
                                         const std::string& option_desc)
{
    auto& descriptions = get_the_allowed_log_options_obj();
    descriptions[option_name] = option_desc;
}

const std::string& LogManager::describe_logging_option(const std::string& option_name)
{
    auto const& descriptions = get_the_allowed_log_options_obj();
    auto it = descriptions.find(option_name);
    if (it == descriptions.end()) {
        return get_the_no_description_found_string();
    } else {
        return it->second;
    }
}

const LogOptionDescMap& LogManager::get_logging_option_descriptions()
{
    return get_the_allowed_log_options_obj();
}

void LogManager::enable_logging_option(const std::string& option_name)
{
    auto& enabled_options = get_the_enabled_log_options_obj();
    enabled_options.insert(option_name);
}

void LogManager::disable_logging_option(const std::string& option_name)
{
    auto& enabled_options = get_the_enabled_log_options_obj();
    enabled_options.erase(option_name);
}

__attribute__ ((destructor)) void finalize_logging()
{
    LogManager::finalize();
}

} /* end namespace logging */
} /* end namespace aurum */


//
// LogManager.cpp ends here
