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
#include "ProgramOptionException.hpp"

namespace aurum {
namespace program_options {
namespace parsers {

namespace ac = aurum::containers;
namespace aa = aurum::allocators;

static inline bool string_contains_white_space(const std::string& the_string)
{
    auto const str_len = the_string.length();

    for (u64 i = 0; i < str_len; ++i) {
        if (the_string[i] == ' ' || the_string[i] == '\n' || the_string[i] == '\t') {
            return true;
        }
    }
    return false;
}

void parse_command_line(int argc, char *argv[],
                        const ProgramOptions& program_options,
                        ParseMap& parse_map)
{
    std::ostringstream sstr;

    for (i32 i = 1; i < argc, ++i) {
        if (string_contains_white_space(argv[i])) {
            sstr << '"' << argv[i] << '"' << " ";
        } else {
            sstr << argv[i];
        }
    }

    parse_option_string(sstr.str(), program_options, parse_map);
}

static inline std::string get_line_from_stream(istream& in_stream)
{

}

// A config file needs to have one option per line
void parse_config_file(const std::string& config_file_name,
                       const ProgramOptions& program_options,
                       ParseMap& parse_map)
{
    std::ifstream in_file(config_file_name.c_str());
    if (in_file.fail()) {
        throw ProgramOptionException((std::string)"Could not open configuration file: " +
                                     config_file_name);
    }

    in_file.seekg(0, in_file.end);
    auto file_length = in_file.tellg();
    in_file.seekg(0, in_file.beg);

    u08* buffer = aa::allocate_raw_cleared(file_length + 1);
    in_file.read(buffer, file_length);

    for (u64 i = 0; i < file_length; ++i) {

    }
}

void parse_option_string(const std::string& option_string,
                         ProgramOptions& program_options,
                         ParseMap& parse_map)
{

}

} /* end namespace parsers */
} /* end namespace program_options */
} /* end namespace aurum */


//
// Parsers.cpp ends here
