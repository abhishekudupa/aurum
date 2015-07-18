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

#if !defined AURUM_STRINGIFICATION_STR_UTILS_HPP_
#define AURUM_STRINGIFICATION_STR_UTILS_HPP_

#include <string>
#include <sstream>
#include <typeinfo>

#include "../containers/Vector.hpp"
#include "../basetypes/AurumTypes.hpp"

#include "Stringifiers.hpp"

namespace ac = aurum::containers;

namespace aurum {
namespace strutils {

class StringConversionException : public AurumException
{
public:
    inline StringConversionException(const std::string& exception_info) noexcept
        : AurumException((std::string)"StringConversionException: " + exception_info)
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

extern i64 find_first_unescaped_match(const std::string& the_string,
                                      char pattern, i64 start_offset = 0);
extern i64 find_last_unescaped_match(const std::string& the_string,
                                     char pattern, i64 start_offset = 0);

extern void reverse(std::string& the_string);
extern std::string reverse_copy(const std::string& the_string);

extern void trim(std::string& the_string);
extern std::string trim_copy(const std::string& the_string);

extern ac::Vector<std::string> split(const std::string& the_string,
                                     const std::string& separator);

extern ac::Vector<std::string> split_on_unescaped(const std::string& the_string,
                                                  char separator);

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

extern void unquote_string(std::string& the_string);
extern std::string unquote_string_copy(const std::string& the_string);

extern u64 find_next_whitespace(const std::string& the_string, u64 start_offset);
extern u64 find_next_unescaped_whitespace(const std::string& the_string, u64 start_offset);

extern u64 find_next_non_whitespace(const std::string& the_string, u64 start_offset);
extern ac::Vector<std::string> split_on_whitespace(const std::string& the_string);
extern ac::Vector<std::string> split_on_unescaped_whitespace(const std::string& the_string);

// caster classes
template <typename T>
class StringCaster
{
public:
    inline T operator () (const std::string& the_string,
                          char separator = ',') const
    {
        std::istringstream istr(the_string);
        T retval;
        istr >> retval;
        if (istr.get() != EOF) {
            throw StringConversionException((std::string)"Leftover characters in string_cast " +
                                            "of string \"" + the_string + "\" to type: " +
                                            type_name<T>());
        }
        return retval;
    }
};

template <>
class StringCaster<bool>
{
public:
    inline bool operator () (const std::string& the_string,
                             char separator = ',') const
    {
        auto&& local_string = trim_copy(the_string);
        to_lowercase(local_string);
        if (local_string == "true") {
            return true;
        } else if (local_string == "false") {
            return false;
        } else {
            throw StringConversionException((std::string)"Error casting string \"" +
                                            the_string + "\" to a boolean value");
        }
    }
};

template <>
class StringCaster<i64>
{
public:
    inline i64 operator () (const std::string& the_string,
                            char separator = ',') const
    {
        return to_integer(the_string);
    }
};

template <>
class StringCaster<u64>
{
public:
    inline u64 operator () (const std::string& the_string,
                            char separator = ',') const
    {
        return to_integer(the_string);
    }
};

template <>
class StringCaster<i32>
{
public:
    inline i32 operator () (const std::string& the_string,
                            char separator = ',') const
    {
        return to_integer(the_string);
    }
};

template <>
class StringCaster<u32>
{
public:
    inline u32 operator () (const std::string& the_string,
                            char separator = ',') const
    {
        return to_integer(the_string);
    }
};

template <>
class StringCaster<i16>
{
public:
    inline i16 operator () (const std::string& the_string,
                            char separator = ',') const
    {
        return to_integer(the_string);
    }
};

template <>
class StringCaster<u16>
{
public:
    inline u16 operator () (const std::string& the_string,
                            char separator = ',') const
    {
        return to_integer(the_string);
    }
};

template <>
class StringCaster<i08>
{
public:
    inline i08 operator () (const std::string& the_string,
                            char separator = ',') const
    {
        return to_integer(the_string);
    }
};

template <>
class StringCaster<u08>
{
public:
    inline u08 operator () (const std::string& the_string,
                            char separator = ',') const
    {
        return to_integer(the_string);
    }
};

template <>
class StringCaster<float>
{
public:
    inline float operator () (const std::string& the_string,
                              char separator = ',') const
    {
        return (float)to_double(the_string);
    }
};

template <>
class StringCaster<double>
{
public:
    inline double operator () (const std::string& the_string,
                               char separator = ',') const
    {
        return to_double(the_string);
    }
};

template <>
class StringCaster<std::string>
{
public:
    inline std::string operator () (const std::string& the_string,
                                    char separator = ',') const
    {
        return the_string;
    }
};

template <typename... TupleTypes>
class StringCaster<std::tuple<TupleTypes...> >
{
private:
    typedef std::tuple<TupleTypes...> TheTupleType;

    template <u64 INDEX>
    inline typename std::enable_if<INDEX == sizeof...(TupleTypes), void>::type
    populate_tuple(TheTupleType& the_tuple,
                   const ac::Vector<std::string>& split_components) const
    {
        return;
    }

    template <u64 INDEX>
    inline typename std::enable_if<INDEX != sizeof...(TupleTypes), void>::type
    populate_tuple(TheTupleType& the_tuple,
                   const ac::Vector<std::string>& split_components) const
    {
        typedef std::tuple<TupleTypes...> TheTupleType;
        typedef typename std::tuple_element<INDEX, TheTupleType>::type ElemType;
        strutils::StringCaster<ElemType> caster;

        try {
            std::get<INDEX>(the_tuple) = caster(split_components[INDEX]);
        } catch (const StringConversionException& e) {
            throw StringConversionException(e.get_exception_info() +
                                            "\nWhen parsing tuple type: " +
                                            type_name<TheTupleType>() + ", and while parsing " +
                                            "value of type: " + type_name<ElemType>() + " at " +
                                            "tuple index: " + std::to_string(INDEX) + ". Value " +
                                            " string: \"" + split_components[INDEX] + "\".");

        }
        populate_tuple<(u64)(INDEX+1)>(the_tuple, split_components);
    }

public:
    inline std::tuple<TupleTypes...> operator () (const std::string& the_string,
                                                  char separator = ',') const
    {
        ac::Vector<std::string> split_components;

        split_components = strutils::split_on_unescaped(the_string, separator);

        if (split_components.size() != sizeof...(TupleTypes)) {
            throw StringConversionException((std::string)"Wrong number of components in string " +
                                            "for conversion to a tuple. Expected " +
                                            std::to_string(sizeof...(TupleTypes)) +
                                            " components, but got " +
                                            stringification::to_string(split_components.size()) +
                                            " components.");
        }

        TheTupleType retval;
        populate_tuple<0>(retval, split_components);
        return retval;
    }
};

template <typename T1, typename T2>
class StringCaster<std::pair<T1, T2> >
{
public:
    inline std::pair<T1, T2> operator () (const std::string& the_string,
                                          char separator = ',') const
    {
        StringCaster<std::tuple<T1, T2> > caster;
        std::pair<T1, T2> retval;
        std::tuple<T1, T2> the_tuple = caster(the_string, separator);
        retval.first = std::get<0>(the_tuple);
        retval.second = std::get<1>(the_tuple);
        return retval;
    }
};

template <typename ElemType>
class StringCaster<ac::Vector<ElemType> >
{
public:
    inline ac::Vector<ElemType> operator () (const std::string& the_string,
                                             char separator = ',') const
    {
        ac::Vector<std::string> split_components;

        split_components = strutils::split_on_unescaped(the_string, separator);

        auto const num_components = split_components.size();

        ac::Vector<ElemType> retval(num_components);
        StringCaster<ElemType> caster;

        for (u64 i = 0; i < num_components; ++i) {
            try {
                retval[i] = caster(split_components[i]);
            } catch (const StringConversionException& e) {
                throw StringConversionException(e.get_exception_info() + "\nWhen parsing " +
                                                "value of vector type: " +
                                                type_name<decltype(retval)>() +
                                                " at index: " + std::to_string(i) +
                                                ".\nValue string: \"" + the_string + "\".");
            }
        }
        return retval;
    }
};

} /* end namespace strutils */
} /* end namespace aurum */

#endif /* AURUM_STRINGIFICATION_STR_UTILS_HPP_ */

//
// StrUtils.hpp ends here
