// Stringifiers.hpp ---
//
// Filename: Stringifiers.hpp
// Author: Abhishek Udupa
// Created: Tue Jun 30 18:55:44 2015 (-0400)
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

#if !defined AURUM_STRINGIFICATION_STRINGIFIERS_HPP_
#define AURUM_STRINGIFICATION_STRINGIFIERS_HPP_

#include "../basetypes/AurumTypes.hpp"
#include <type_traits>
#include <sstream>
#include <typeinfo>

namespace aurum {
namespace stringification {

template <typename T>
class Stringifier
{
private:
    inline std::string to_string_(const T& object, i64 verbosity,
                                  const std::true_type& is_stringifiable) const
    {
        return object.to_string(verbosity);
    }

    inline std::string to_string_(const T& object, i64 verbosity,
                                  const std::false_type& is_stringifiable) const
    {
        return (std::string)"object[" + std::to_string(sizeof(T)) + "]";
    }

public:
    inline std::string operator () (const T& object, i64 verbosity) const
    {
        typename std::is_base_of<StringifiableEBC, T>::type is_stringifiable;
        return to_string_(object, verbosity, is_stringifiable);
    }

    inline std::string operator () (const T& object) const
    {
        return (*this)(object, 0);
    }
};

// Specializations

// An ugly macro for creating classes for which predefined to_string methods exist
#define MAKE_PREDEF_STRINGIFIER_(TYPENAME_)                                           \
    template <>                                                                       \
    class Stringifier<TYPENAME_>                                                      \
    {                                                                                 \
    public:                                                                           \
        inline std::string operator () (const TYPENAME_& object, i64 verbosity) const \
        {                                                                             \
            return std::to_string(object);                                            \
        }                                                                             \
        inline std::string operator () (const TYPENAME_& object) const                \
        {                                                                             \
            return std::to_string(object);                                            \
        }                                                                             \
    }

MAKE_PREDEF_STRINGIFIER_(u16);
MAKE_PREDEF_STRINGIFIER_(u32);
MAKE_PREDEF_STRINGIFIER_(u64);
MAKE_PREDEF_STRINGIFIER_(i16);
MAKE_PREDEF_STRINGIFIER_(i32);
MAKE_PREDEF_STRINGIFIER_(i64);
MAKE_PREDEF_STRINGIFIER_(float);
MAKE_PREDEF_STRINGIFIER_(double);

#undef MAKE_PREDEF_STRINGIFIER_

template <>
class Stringifier<bool>
{
public:
    inline std::string operator () (bool object, i64 verbosity) const
    {
        if (object) {
            return "true";
        } else {
            return "false";
        }
    }

    inline std::string operator () (bool object) const
    {
        if (object) {
            return "true";
        } else {
            return "false";
        }
    }
};

template <>
class Stringifier<std::string>
{
public:
    inline std::string operator () (const std::string& object, i64 verbosity) const
    {
        return object;
    }

    inline std::string operator () (const std::string& object) const
    {
        return object;
    }
};

template <>
class Stringifier<char>
{
public:
    inline std::string operator () (char object, i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }

    inline std::string operator () (char object) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }
};

template <>
class Stringifier<unsigned char>
{
public:
    inline std::string operator () (unsigned char object, i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }

    inline std::string operator () (char object) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }
};

template <typename T>
class Stringifier<T*>
{
public:
    inline std::string operator () (const T* object, i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }

    inline std::string operator () (const T* object) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }
};

template <typename T>
class Stringifier<const T*>
{
public:
    inline std::string operator () (const T* object, i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }

    inline std::string operator () (const T* object) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }
};

template <typename T>
class Stringifier<memory::ManagedPointer<T> >
{
public:
    inline std::string operator () (const memory::ManagedPointer<T>& object, i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }

    inline std::string operator () (const memory::ManagedPointer<T>& object) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }
};

template <typename T>
class Stringifier<memory::ManagedConstPointer<T> >
{
public:
    inline std::string operator () (const memory::ManagedConstPointer<T>& object,
                                    i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }

    inline std::string operator () (const memory::ManagedConstPointer<T>& object) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }
};

template <typename T1, typename T2>
class Stringifier<std::pair<T1, T2> >
{
public:
    inline std::string operator () (const std::pair<T1, T2>& object, i64 verbosity) const
    {
        std::ostringstream sstr;
        Stringifier<T1> stringifier1;
        Stringifier<T2> stringifier2;

        sstr << "<" << stringifier1(object, verbosity) << ", "
             << stringifier2(object, verbosity) << ">";
        return sstr.str();
    }

    inline std::string operator () (const std::pair<T1, T2>& object) const
    {
        return (*this)(object, 0);
    }
};

namespace detail {

template <u64 INDEX, typename... TupleTypes>
inline typename std::enable_if<INDEX == sizeof...(TupleTypes), void>::type
stringify_tuple(const std::tuple<TupleTypes...>& the_tuple,
                std::ostringstream& sstr, i64 verbosity)
{
    return;
}

template <u64 INDEX, typename... TupleTypes>
inline typename std::enable_if<INDEX != sizeof...(TupleTypes), void>::type
stringify_tuple(const std::tuple<TupleTypes...>& the_tuple,
                std::ostringstream& sstr, i64 verbosity)
{
    typedef std::tuple<TupleTypes...> TheTupleType;
    typedef typename std::tuple_element<INDEX, TheTupleType>::type ElementType;
    Stringifier<ElementType> stringifier;

    if (INDEX > 0) {
        sstr << ", ";
    }

    sstr << stringifier(std::get<INDEX>(the_tuple), verbosity);
    stringify_tuple<INDEX+1, TupleTypes...>(the_tuple, sstr, verbosity);
}

} /* end namespace detail */

template <typename... TupleTypes>
class Stringifier<std::tuple<TupleTypes...> >
{
public:
    inline std::string operator () (const std::tuple<TupleTypes...>& object, i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << "<";
        detail::stringify_tuple<0, TupleTypes...>(object, sstr, verbosity);
        sstr << ">";
        return sstr.str();
    }

    inline std::string operator () (const std::tuple<TupleTypes...>& object) const
    {
        return (*this)(object, 0);
    }
};

template <typename ContainerType, typename ElementType>
class IterableStringifier
{
public:
    inline std::string operator () (const ContainerType& container, i64 verbosity) const
    {
        std::ostringstream sstr;
        Stringifier<ElementType> stringifier;
        auto first = std::begin(container);
        auto last = std::end(container);
        for (auto it = first; it != last; ++it) {
            if (it != first) {
                sstr << ", ";
            }
            sstr << stringifier(*it);
        }
        return sstr.str();
    }
};

// define some to_string methods
template <typename T>
inline std::string to_string(const T& object)
{
    Stringifier<T> stringifier;
    return stringifier(object);
}

} /* end namespace stringification */
} /* end namespace aurum */

#endif /* AURUM_STRINGIFICATION_STRINGIFIERS_HPP_ */

//
// Stringifiers.hpp ends here
