// Parsers.cpp ---
// Filename: Parsers.cpp
// Author: Abhishek Udupa
// Created: Wed May 20 20:27:46 2015 (-0400)
//
// Copyright (c) 2013, Abhishek Udupa, University of Pennsylvania
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

#include <sstream>
#include <fstream>

#include "../stringification/StrUtils.hpp"
#include "../allocators/MemoryManager.hpp"
#include "../shell/ShellUtils.hpp"

#include "Parsers.hpp"
#include "ProgramOptionException.hpp"

namespace aurum {
namespace program_options {
namespace parsers {

namespace ac = aurum::containers;
namespace aa = aurum::allocators;

static inline std::string process_escapes(const std::string& the_string)
{
    std::ostringstream sstr;

    bool in_escaped_state = false;
    for (auto const& cur_char : the_string) {
        if (in_escaped_state) {
            switch(cur_char) {
            case '\\': sstr << '\\'; break;
            case 'n': sstr << '\n'; break;
            case 't': sstr << '\t'; break;
            case 'r': sstr << '\r'; break;
            case '\'': sstr << '\''; break;
            case '\"': sstr << '\"'; break;
            case ' ' : sstr << ' '; break;
            default:
                throw ProgramOptionException((std::string)"Bad escaped character: \'\\" +
                                             cur_char + "\' in option string");
            }
        } else if (cur_char == '\\') {
            in_escaped_state = true;
        } else {
            sstr << cur_char;
        }
    }

    return sstr.str();
}

void parse_command_line(int argc, char* argv[],
                        const ProgramOptions& program_options,
                        ParseMap& parse_map,
                        bool do_escape_processing)
{
    std::ostringstream sstr;
    for (int i = 1; i < argc; ++i) {
        const char* current_arg = argv[i];
        const u64 current_len = strlen(current_arg);

        if (strutils::find_next_whitespace(current_arg, 0) < current_len) {
            sstr << "\"" << current_arg << "\"";
        } else {
            sstr << current_arg;
        }

        if (i != argc - 1) {
            sstr << " ";
        }
    }
    parse_option_string(sstr.str(), program_options, parse_map);
}

// We assume that in a config file, each option is
// presented on a separate line. Newlines if present
// in the option strings must be appropriately escaped
void parse_config_file(const std::string& config_file_name,
                       const ProgramOptions& program_options,
                       ParseMap& parse_map,
                       bool do_escape_processing,
                       bool do_shell_expansion)
{
    // read in the entire file
    std::ifstream file_stream(config_file_name.c_str());
    file_stream.seekg(0, file_stream.end);
    auto const file_length = file_stream.tellg();
    file_stream.seekg(0, file_stream.beg);

    char* read_buffer = aa::casted_allocate_raw_cleared<char>(file_length);
    file_stream.read(read_buffer, file_length);

    auto&& config_file_lines = strutils::split(read_buffer, "\n");
    auto const num_lines = config_file_lines.size();
    std::ostringstream sstr;

    for (u64 i = 0; i < num_lines; ++i) {
        sstr << config_file_lines[i];
        if (i != num_lines - 1) {
            sstr << " ";
        }
    }
    parse_option_string(sstr.str(), program_options, parse_map,
                        do_escape_processing, do_shell_expansion);
}

static inline std::string read_until_end_quote(const std::string& the_string,
                                               u64 start_offset, u64& new_offset)
{
    std::ostringstream sstr;
    auto const max_offset = the_string.length();

    u64 current_offset;
    bool is_escaped = false;
    for (current_offset = start_offset; current_offset < max_offset; ++current_offset) {

        auto const cur_char = the_string[current_offset];

        if (cur_char == '\"' && !is_escaped) {
            break;
        }

        if (cur_char == '\\') {
            is_escaped = true;
        } else {
            is_escaped = false;
        }

        sstr << cur_char;
    }

    if (current_offset == max_offset) {
        throw ProgramOptionException((std::string)"Unterminated quoted string in options: " +
                                     the_string);
    }
    new_offset = current_offset + 1;
    return sstr.str();
}

static inline std::string get_next_token(const std::string& the_string,
                                         u64 start_offset, u64& new_offset)
{
    u64 current_offset = start_offset;
    while(the_string[current_offset] == ' ' ||
          the_string[current_offset] == '\n' ||
          the_string[current_offset] == '\t') {
        ++current_offset;
    }

    if (the_string[current_offset] == '\"') {
        return read_until_end_quote(the_string, current_offset, new_offset);
    } else {
        new_offset = strutils::find_next_whitespace(the_string, current_offset);
        if (new_offset == current_offset) {
            return "";
        }
        return the_string.substr(current_offset, new_offset - current_offset - 1);
    }
}

static inline ac::Vector<std::string> tokenize_option_string(const std::string& option_string)
{
    u64 current_offset = 0;
    u64 next_offset = 0;
    auto const max_offset = option_string.length();
    ac::Vector<std::string> retval;

    while (current_offset < max_offset) {
        auto&& next_token = get_next_token(option_string, current_offset, next_offset);
        if (next_token != "") {
            retval.push_back(next_token);
        }
        current_offset = next_offset;
    }

    return retval;
}

ac::Vector<std::string>::iterator
parse_value(const ac::Vector<std::string>::iterator& token_iterator,
            const std::string& option_name,
            const ProgramOptions& program_options,
            std::string& parsed_value)
{
    auto cur_iter = token_iterator;
    auto option_value = program_options.find(option_name);

    if (option_value == OptionValueRef::null_pointer) {
        // unspecified option. just parse until we hit another
        // option
        ++cur_iter;
        std::ostringstream sstr;
        while (!strutils::begins_with(*cur_iter, "--") &&
               !strutils::begins_with(*cur_iter, "-")) {
            if (sstr.str().length() > 0) {
                sstr << " " << *cur_iter;
            } else {
                sstr << *cur_iter;
            }
            ++cur_iter;
        }
        parsed_value = sstr.str();
        return cur_iter;
    }

    if (option_value->has_implicit_value()) {
        auto const& next_token = *(cur_iter + 1);
        if (strutils::begins_with(next_token, "--") ||
            strutils::begins_with(next_token, "-")) {
            parsed_value = option_value->get_textual_implicit_value();
            return (++cur_iter);
        }
    }

    if (!option_value->is_multitoken()) {
        ++cur_iter;
        auto const& next_token = *cur_iter;
        if (strutils::begins_with(next_token, "--") ||
            strutils::begins_with(next_token, "-")) {
            throw ProgramOptionException((std::string)"Option \"" + option_name + "\"" +
                                         "requires an argument, but no argument was provided.");
        }
        parsed_value = *cur_iter;
        return (++cur_iter);
    } else {
        ++cur_iter;
        std::ostringstream sstr;
        auto const& separator = option_value->get_separator();
        while (!strutils::begins_with(*cur_iter, "--") &&
               !strutils::begins_with(*cur_iter, "-")) {
            auto&& split_components = strutils::split(*cur_iter, separator);
            for (auto const& component : split_components) {
                if (sstr.str().length() > 0) {
                    sstr << "," << component;
                } else {
                    sstr << component;
                }
            }
        }
        if (sstr.str().length() == 0) {
            throw ProgramOptionException((std::string)"Option \"" + option_name + "\" " +
                                         "requires an arguments, but no arguments were " +
                                         "provided.");
        }
        parsed_value = sstr.str();
        return cur_iter;
    }
}

void parse_option_string(const std::string& option_string,
                         const ProgramOptions& program_options,
                         ParseMap& parse_map,
                         bool do_escape_processing,
                         bool do_shell_expansion)
{
    auto&& option_tokens = tokenize_option_string(option_string);

    u64 next_positional_option = 1;

    auto token_iter = option_tokens.begin();
    auto tokens_end = option_tokens.end();

    while (token_iter != tokens_end) {
        auto&& current_token = strutils::trim_copy(*token_iter);

        if (strutils::begins_with(current_token, "--") ||
            strutils::begins_with(current_token, "-")) {
            std::string parsed_value;
            token_iter = parse_value(token_iter, current_token, program_options, parsed_value);
            parse_map[current_token].push_back(parsed_value);
        } else {
            std::string option_name = gc_positional_option_prefix_ +
                std::to_string(next_positional_option++);
            parse_map[option_name].push_back(current_token);
            ++token_iter;
        }
    }
    // do shell expansions first
    // followed by escape processing
    if (do_shell_expansion) {
        for (auto& option_value_pair : parse_map) {
            for (auto& option_value : option_value_pair.second) {
                option_value = shell::apply_shell_expansion(option_value);
            }
        }
    }
    if (do_escape_processing) {
        for (auto& option_value_pair : parse_map) {
            for (auto& option_value : option_value_pair.second) {
                option_value = process_escapes(option_value);
            }
        }
    }
    return;
}

} /* end namespace parsers */
} /* end namespace program_options */
} /* end namespace aurum */


//
// Parsers.cpp ends here
