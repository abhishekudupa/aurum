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

#include "../strutils/StrUtils.hpp"
#include "../allocators/MemoryManager.hpp"

#include "Parsers.hpp"
#include "ProgramOptions.hpp"
#include "ProgramOptionException.hpp"

namespace aurum {
namespace program_options {
namespace parsers {

namespace ac = aurum::containers;
namespace aa = aurum::allocators;

void parse_command_line(int argc, char* argv[],
                        const ProgramOptions& program_options,
                        ParseMap& parse_map)
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

static inline std::string process_escapes(const std::string& the_string)
{
    std::ostringstream sstr;

    bool in_escaped_state = false;
    for (auto const& cur_char : the_string) {
        if (in_escaped_state) {
            switch(cur_char) {
            case '\\': sstr << '\\';
            case 'n': sstr << '\n';
            case 't': sstr << '\t';
            case 'r': sstr << '\r';
            case '\'': sstr << '\'';
            case '\"': sstr << '\"';
            }
        } else if (cur_char == '\\') {
            in_escaped_state = true;
        } else {
            sstr << cur_char;
        }
    }

    return sstr.str();
}

// We assume that in a config file, each option is
// presented on a separate line. Newlines if present
// in the option strings must be appropriately escaped
void parse_config_file(const std::string& config_file_name,
                       const ProgramOptions& program_options,
                       ParseMap& parse_map)
{
    // read in the entire file
    std::ifstream file_stream(config_file_name.c_str());
    file_stream.seekg(0, file_stream.end);
    auto const file_length = file_stream.tellg();
    file_stream.seekg(0, file_stream.beg);

    char* read_buffer = aa::casted_allocate_raw_cleared<char>(file_length);
    file_stream.read(read_buffer, file_length);

    auto&& config_file_lines = strutils::split(read_buffer, "\n");

    for (auto const& config_file_line : config_file_lines) {
        auto&& escaped_line =
    }
}

void parse_option_string(const std::string& option_string,
                         const ProgramOptions& program_options,
                         ParseMap& parse_map)
{

}

} /* end namespace parsers */
} /* end namespace program_options */
} /* end namespace aurum */


//
// Parsers.cpp ends here
