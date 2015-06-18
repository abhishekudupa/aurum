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

#include <cctype>

#include "StrUtils.hpp"

namespace aurum {
namespace strutils {

// requires: position + pat_length < str_length
static inline bool match_on_position(const std::string& the_string,
                                     const std::string& pattern,
                                     u64 str_length, u64 pat_length,
                                     i64 position, bool case_insensitive)
{
    for (u64 i = 0; i < pat_length; ++i) {
        if (case_insensitive) {
            if (std::tolower(the_string[position + i]) != std::tolower(pattern[i])) {
                return false;
            }
        } else {
            if (the_string[position + i] != pattern[i]) {
                return false;
            }
        }
    }
    return true;
}

static inline i64 find_first_match_internal(const std::string& the_string,
                                            const std::string& pattern,
                                            i64 start_offset,
                                            bool case_insensitive)
{
    u64 str_length = the_string.length();
    u64 pat_length = pattern.length();

    auto position = start_offset;

    while (position + pat_length < str_length) {
        if (match_on_position(the_string, pattern, str_length,
                              pat_length, position, case_insensitive)) {
            return position;
        }
    }
    return INT64_MAX;
}

static inline i64 find_last_match_internal(const std::string& the_string,
                                           const std::string& pattern,
                                           i64 start_offset,
                                           bool case_insensitive)
{
    i64 str_length = the_string.length();
    i64 pat_length = pattern.length();

    auto position = std::min(str_length - pat_length, start_offset);

    while (position >= 0) {
        if (match_on_position(the_string, pattern, str_length,
                              pat_length, position, case_insensitive)) {
            return position;
        }
    }
    return INT64_MIN;
}



i64 find_first_match(const std::string& the_string,
                     const std::string& pattern,
                     i64 start_offset)
{
    return find_first_match_internal(the_string, pattern, start_offset, false);
}

i64 find_last_match(const std::string& the_string,
                    const std::string& pattern,
                    i64 start_offset)
{
    return find_last_match_internal(the_string, pattern, start_offset, false);
}

i64 ifind_first_match(const std::string& the_string,
                      const std::string& pattern,
                      i64 start_offset)
{
    return find_first_match_internal(the_string, pattern, start_offset, true);
}

i64 ifind_last_match(const std::string& the_string,
                     const std::string& pattern,
                     i64 start_offset)
{
    return find_last_match_internal(the_string, pattern, start_offset, true);
}

void reverse(std::string& the_string)
{
    u64 start_offset = 0;
    u64 end_offset = the_string.length() - 1;

    while (end_offset > start_offset) {
        std::swap(the_string[start_offset], the_string[end_offset]);
        --end_offset;
        ++start_offset;
    }
    return;
}

std::string reverse_copy(const std::string& the_string)
{
    std::string retval = the_string;
    auto const str_len = the_string.length();

    for (u64 i = 0; i < str_len; ++i) {
        retval[i] = the_string[str_len - i - 1];
    }
    return retval;
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
    std::string copy_string(the_string);
    trim(copy_string);
    return copy_string;
}

ac::Vector<std::string> split(const std::string& the_string,
                              const std::string& separator)
{
    auto const sep_len = separator.length();
    auto const str_len = the_string.length();

    ac::Vector<std::string> retval;

    u64 current_offset = 0;
    u64 prev_match_pos = 0;

    while (current_offset < str_len) {
        auto sep_match_pos = find_first_match(the_string, separator, current_offset);
        // compress successive matches of separator
        if (sep_match_pos - prev_match_pos > 0) {
            retval.push_back(the_string.substr(prev_match_pos, sep_match_pos - prev_match_pos));
        }
        current_offset = sep_match_pos + sep_len;
        prev_match_pos = current_offset;
    }
    return retval;
}

bool begins_with(const std::string& the_string,
                 const std::string& pattern)
{
    return match_on_position(the_string, pattern, the_string.length(),
                             pattern.length(), 0, false);
}

bool ibegins_with(const std::string& the_string,
                  const std::string& pattern)
{
    return match_on_position(the_string, pattern, the_string.length(),
                             pattern.length(), 0, true);
}

bool ends_with(const std::string& the_string,
               const std::string& pattern)
{
    return match_on_position(the_string, pattern, the_string.length(),
                             pattern.length(), the_string.length() - pattern.length(), false);
}

bool iends_with(const std::string& the_string,
                const std::string& pattern)
{
    return match_on_position(the_string, pattern, the_string.length(),
                             pattern.length(), the_string.length() - pattern.length(), true);
}

void to_lowercase(std::string& the_string)
{
    auto const str_len = the_string.length();

    for (u64 i = 0; i < str_len; ++i) {
        the_string[i] = std::tolower(the_string[i]);
    }
    return;
}

std::string to_lowercase_copy(const std::string& the_string)
{
    std::string retval(the_string);
    to_lowercase(retval);
    return retval;
}

void to_uppercase(std::string& the_string)
{
    auto const str_len = the_string.length();

    for (u64 i = 0; i < str_len; ++i) {
        the_string[i] = std::toupper(the_string[i]);
    }
    return;
}

std::string to_uppercase_copy(const std::string& the_string)
{
    std::string retval(the_string);
    to_uppercase(retval);
    return retval;
}

static inline u64 to_number(const std::string& the_string, u32 base)
{
    if (base > 16 || base < 1) {
        throw StringConversionException((std::string)"Base = " + to_string(base) +
                                        " is too large or too small to convert string \"" +
                                        the_string + "\" to a number");
    }

    auto const str_len = the_string.length();
    for (u32 i = 0; i < str_len; ++i) {
        if (base <= 10) {
            if (the_string[i] < '0' || the_string[i] > (unsigned char)('0' + base - 1)) {
                throw StringConversionException((std::string)"The string \"" + the_string +
                                                "contains characters invalid for base " +
                                                to_string(base) + " number.");
            }
        } else {
            if ((the_string[i] < '0' || the_string[i] > '9') &&
                (std::tolower(the_string[i]) < 'a' ||
                 std::tolower(the_string[i]) > 'a' + (unsigned char)(base - 11))) {
                throw StringConversionException((std::string)"The string \"" + the_string +
                                                "contains characters invalid for a base " +
                                                to_string(base) + " number.");
            }
        }
    }
    return std::stoull(the_string, 0, base);
}

i64 to_integer(const std::string& the_string)
{
    std::string trimmed_string(the_string);

    u32 base = 0;
    bool negative = false;
    if (trimmed_string[0] == '-') {
        negative = true;
        trimmed_string = trimmed_string.substr(1, trimmed_string.length() - 1);
    }
    if (ibegins_with(trimmed_string, "0x")) {
        base = 16;
        trimmed_string = trimmed_string.substr(2, trimmed_string.length() - 2);
    } else if (ibegins_with(trimmed_string, "0b")) {
        base = 2;
        trimmed_string = trimmed_string.substr(2, trimmed_string.length() - 2);
    } else {
        base = 10;
    }
    auto num = to_number(trimmed_string, base);
    if (negative && num > INT64_MAX) {
        throw StringConversionException((std::string)"Overflow converting string \"" +
                                        the_string + "\" to an integer.");
    }
    if (negative) {
        return -num;
    } else {
        return num;
    }
}

double to_double(const std::string& the_string)
{
    return std::stod(trim_copy(the_string));
}

u64 to_unsigned(const std::string& the_string)
{
    std::string trimmed_string(the_string);

    u32 base = 0;

    if (ibegins_with(trimmed_string, "0x")) {
        base = 16;
        trimmed_string = trimmed_string.substr(2, trimmed_string.length() - 2);
    } else if (ibegins_with(trimmed_string, "0b")) {
        base = 2;
        trimmed_string = trimmed_string.substr(2, trimmed_string.length() - 2);
    } else {
        base = 10;
    }
    return to_number(trimmed_string, base);
}

void unquote_string(std::string& the_string)
{
    auto const str_length = the_string.length();
    u64 start_offset = 0;
    u64 end_offset = str_length - 1;

    while (start_offset <= end_offset &&
           (the_string[start_offset] == '"' ||
            the_string[start_offset] == '\'')) {
        ++start_offset;
    }

    while (start_offset <= end_offset &&
           (the_string[end_offset] == '"' ||
            the_string[end_offset] == '\'')) {
        --end_offset;
    }
    if (start_offset < end_offset) {
        the_string.clear();
    } else {
        the_string = the_string.substr(start_offset, end_offset - start_offset + 1);
    }
    return;
}

std::string unquote_string_copy(const std::string& the_string)
{
    auto const str_length = the_string.length();
    u64 start_offset = 0;
    u64 end_offset = str_length - 1;

    while (start_offset <= end_offset &&
           (the_string[start_offset] == '"' ||
            the_string[start_offset] == '\'')) {
        ++start_offset;
    }

    while (start_offset <= end_offset &&
           (the_string[end_offset] == '"' ||
            the_string[end_offset] == '\'')) {
        --end_offset;
    }
    if (start_offset < end_offset) {
        return "";
    } else {
        return the_string.substr(start_offset, end_offset - start_offset + 1);
    }
}

u64 find_next_whitespace(const std::string& the_string, u64 start_offset)
{
    auto current_offset = start_offset;
    auto const str_length = the_string.length();

    while (current_offset < str_length) {
        auto const current_char = the_string[current_offset];
        if (current_char == ' ' || current_char == '\t' ||
            current_char == '\n' || current_char == '\r') {
            return current_offset;
        }
        ++current_offset;
    }

    return str_length;
}

u64 find_next_non_whitespace(const std::string& the_string, u64 start_offset)
{
    auto current_offset = start_offset;
    auto const str_length = the_string.length();

    while (current_offset < str_length) {
        auto const current_char = the_string[current_offset];
        if (current_char != ' ' && current_char != '\t' &&
            current_char != '\n' && current_char != '\r') {
            return current_offset;
        }
    }
    return str_length;
}

ac::Vector<std::string> split_on_whitespace(const std::string& the_string)
{
    ac::Vector<std::string> retval;
    auto const str_length = the_string.length();

    u64 begin_position = find_next_non_whitespace(the_string, 0);

    while (begin_position < str_length) {
        u64 end_position = find_next_whitespace(the_string, begin_position);
        retval.push_back(the_string.substr(begin_position, end_position - begin_position));
        begin_position = find_next_non_whitespace(the_string, end_position);
    }

    return retval;
}

} /* end namespace strutils */
} /* end namespace aurum */

//
// StrUtils.cpp ends here
