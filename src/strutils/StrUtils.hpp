// StrUtils.hpp ---
//
// Filename: StrUtils.hpp
// Author: Abhishek Udupa
// Created: Fri May 15 16:52:44 2015 (-0400)
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

#if !defined AURUM_STRUTILS_STR_UTILS_HPP_
#define AURUM_STRUTILS_STR_UTILS_HPP_

#include <string>

#include "../containers/Vector.hpp"
#include "../basetypes/AurumTypes.hpp"

namespace ac = aurum::containers;

namespace aurum {
namespace strutils {

class StringConversionException : public AurumException
{
public:
    inline StringConversionException(const std::string& exception_info) noexcept
        : AurumException(exception_info)
    {
        // Nothing here
    }

    inline StringConversionException(const StringConversionException& other) noexcept
        : AurumException(other.m_exception_info)
    {
        // Nothing here
    }

    inline StringConversionException(StringConversionException&& other) noexcept
        : AurumException(std::move(other.m_exception_info))
    {
        // Nothing here
    }

    virtual ~StringConversionException() {}
    inline StringConversionException& operator = (const StringConversionException& other) noexcept
    {
        AurumException::operator=(other);
        return *this;
    }

    inline StringConversionException& operator = (StringConversionException&& other) noexcept
    {
        AurumException::operator=(std::move(other));
        return *this;
    }
};

extern i64 find_first_match(const std::string& the_string,
                            const std::string& pattern,
                            i64 start_offset = 0);
extern i64 find_last_match(const std::string& the_string,
                           const std::string& pattern,
                           i64 start_offset = INT64_MAX);
extern i64 ifind_first_match(const std::string& the_string,
                             const std::string& pattern,
                             i64 start_offset = 0);
extern i64 ifind_last_match(const std::string& the_string,
                            const std::string& pattern,
                            i64 start_offset = INT64_MAX);

extern void reverse(std::string& the_string);
extern std::string reverse_copy(const std::string& the_string);

extern void trim(std::string& the_string);
extern std::string trim_copy(const std::string& the_string);

extern ac::Vector<std::string> split(const std::string& the_string,
                                     const std::string& separator);

extern bool begins_with(const std::string& the_string,
                        const std::string& pattern);
extern bool ibegins_with(const std::string& the_string,
                         const std::string& pattern);
extern bool ends_with(const std::string& the_string,
                      const std::string& pattern);
extern bool iends_with(const std::string& the_string,
                       const std::string& pattern);

extern void to_lowercase(std::string& the_string);
extern std::string to_lowercase_copy(const std::string& the_string);
extern void to_uppercase(std::string& the_string);
extern std::string to_uppercase_copy(const std::string& the_string);

extern i64 to_integer(const std::string& the_string);
extern double to_double(const std::string& the_string);
extern u64 to_unsigned(const std::string& the_string);

} /* end namespace strutils */
} /* end namespace aurum */

#endif /* AURUM_STRUTILS_STR_UTILS_HPP_ */

//
// StrUtils.hpp ends here
