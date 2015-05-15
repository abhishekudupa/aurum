// StrUtils.cpp ---
//
// Filename: StrUtils.cpp
// Author: Abhishek Udupa
// Created: Fri May 15 17:45:19 2015 (-0400)
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

#include "StrUtils.hpp"

namespace aurum {
namespace strutils {

static inline bool match_on_position(const std::string& the_string,
                                     const std::string& pattern,
                                     i64 position)
{
    auto const pat_length = pattern.length();
    auto const str_length = the_string.length();

    if (position + pat_length >= str_length) {
        return false;
    }
    for (u64 i = 0; i < pat_length; ++i) {
        if (the_string[position + i] != pattern[i]) {
            return false;
        }
    }
}

i64 find_first_match(const std::string& the_string,
                     const std::string& pattern,
                     i64 start_offset)
{
    if (start_offset >= the_string.length()) {
        return INT64_MAX;
    }
}

void trim(std::string& the_string)
{
    u64 begin_pos = 0;
    u64 end_pos = the_string.length() - 1;

    while (the_string[begin_pos] == ' ' ||
           the_string[begin_pos] == '\t' ||
           the_string[begin_pos] == '\n') {
        ++begin_pos;
    }

    while (the_string[end_pos] == ' ' ||
           the_string[end_pos] == '\t' ||
           the_string[end_pos] == '\n') {
        --end_pos;
    }

    if (end_pos <= begin_pos) {
        the_string = "";
    }

    the_string = the_string.substr(begin_pos, end_pos - begin_pos + 1);
}

std::string trim_copy(const std::string& the_string)
{
    auto copy_string = the_string;
    trim(copy_string);
    return copy_string;
}

ac::Vector<std::string> split(const std::string& the_string,
                              const std::string& separator)
{

}

} /* end namespace strutils */
} /* end namespace aurum */

//
// StrUtils.cpp ends here
