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
#include "../basetypes/AurumTraits.hpp"

#include <sstream>
#include <string>
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
        typename std::is_base_of<detail_::StringifiableEBC, T>::type is_stringifiable;
        return to_string_(object, verbosity, is_stringifiable);
    }

    inline std::string operator () (const T& object) const
    {
        return (*this)(object, 0);
    }
};

// Specializations

namespace detail_ {

template <typename T>
class ToStringStringifier
{
public:
    inline std::string operator () (const T& object, i64 verbosity) const
    {
        return std::to_string(object);
    }

    inline std::string operator () (const T& object) const
    {
        return (*this)(object, 0);
    }
};

template <typename T>
class StreamingStringifier
{
public:
    inline std::string operator () (const T& object, i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << object;
        return sstr.str();
    }

    inline std::string operator () (const T& object) const
    {
        return (*this)(object, 0);
    }
};

} /* end namespace detail_ */

template <>
class Stringifier<u16> : public detail_::ToStringStringifier<u16>
{};

template <>
class Stringifier<u32> : public detail_::ToStringStringifier<u32>
{};

template <>
class Stringifier<u64> : public detail_::ToStringStringifier<u64>
{};

template <>
class Stringifier<i16> : public detail_::ToStringStringifier<i16>
{};

template <>
class Stringifier<i32> : public detail_::ToStringStringifier<i32>
{};

template <>
class Stringifier<i64> : public detail_::ToStringStringifier<i64>
{};

template <>
class Stringifier<float> : public detail_::ToStringStringifier<float>
{};

template <>
class Stringifier<double> : public detail_::ToStringStringifier<double>
{};

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
class Stringifier<char> : public detail_::StreamingStringifier<char>
{};

template <>
class Stringifier<unsigned char> : public detail_::StreamingStringifier<unsigned char>
{};

template <typename T>
class Stringifier<T*> : public detail_::StreamingStringifier<T*>
{};

template <typename T>
class Stringifier<const T*> : public detail_::StreamingStringifier<const T*>
{};

template <typename T>
class Stringifier<memory::ManagedPointer<T> >
    : public detail_::StreamingStringifier<memory::ManagedPointer<T> >
{};

template <typename T>
class Stringifier<memory::ManagedConstPointer<T> >
    : public detail_::StreamingStringifier<memory::ManagedConstPointer<T> >
{};

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

template <typename... TupleTypes>
class Stringifier<std::tuple<TupleTypes...> >
{
private:
    typedef std::tuple<TupleTypes...> TheTupleType;

    template <u64 INDEX>
    inline typename std::enable_if<INDEX == sizeof...(TupleTypes), void>::type
    stringify_tuple(const TheTupleType& the_tuple, i64 verbosity, std::ostream& out) const
    {
        return;
    }

    template <u64 INDEX>
    inline typename std::enable_if<INDEX != sizeof...(TupleTypes), void>::type
    stringify_tuple(const TheTupleType& the_tuple, i64 verbosity, std::ostream& out) const
    {
        typedef typename std::tuple_element<INDEX, TheTupleType>::type ElemType;
        Stringifier<ElemType> sub_stringifier;

        if (INDEX > 0) {
            out << ", ";
        }

        out << sub_stringifier(std::get<INDEX>(the_tuple), verbosity);
        stringify_tuple<INDEX+1>(the_tuple, verbosity, out);
    }

public:
    inline std::string operator () (const std::tuple<TupleTypes...>& object, i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << "<";
        stringify_tuple<0>(object, verbosity, sstr);
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

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepStringifier : public Stringifier<T>
{};

template <typename T>
class DeepStringifier<T*, 0> : public Stringifier<T*>
{};

template <typename T>
class DeepStringifier<const T*, 0> : public Stringifier<const T*>
{};

template <typename T>
class DeepStringifier<memory::ManagedPointer<T>, 0>
    : public Stringifier<memory::ManagedPointer<T> >
{};

template <typename T>
class DeepStringifier<memory::ManagedConstPointer<T>, 0>
    : public Stringifier<memory::ManagedConstPointer<T> >
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepStringifier<T*, NUM_DEREFS_ALLOWED>
{
public:
    inline std::string operator () (const T* const& object, i64 verbosity) const
    {
        DeepStringifier<T, NUM_DEREFS_ALLOWED-1> sub_stringifier;
        return sub_stringifier(*object, verbosity);
    }

    inline std::string operator () (const T* const& object) const
    {
        return (*this)(object, 0);
    }
};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepStringifier<const T*, NUM_DEREFS_ALLOWED>
    : public DeepStringifier<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepStringifier<memory::ManagedPointer<T>, NUM_DEREFS_ALLOWED>
    : public DeepStringifier<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T, u64 NUM_DEREFS_ALLOWED>
class DeepStringifier<memory::ManagedConstPointer<T>, NUM_DEREFS_ALLOWED>
    : public DeepStringifier<T*, NUM_DEREFS_ALLOWED>
{};

template <typename T1, typename T2, u64 NUM_DEREFS_ALLOWED>
class DeepStringifier<std::pair<T1, T2>, NUM_DEREFS_ALLOWED>
{
private:
    typedef std::pair<T1, T2> PairType;

public:
    inline std::string operator () (const PairType& the_pair, i64 verbosity) const
    {
        DeepStringifier<T1, NUM_DEREFS_ALLOWED> sub_stringifier1;
        DeepStringifier<T2, NUM_DEREFS_ALLOWED> sub_stringifier2;

        std::ostringstream sstr;

        sstr << "<" << sub_stringifier1(the_pair.first) << ", "
             << sub_stringifier2(the_pair.second) << ">";
        return sstr.str();
    }

    inline std::string operator () (const PairType& the_pair) const
    {
        return (*this)(the_pair, 0);
    }
};

template <typename... TupleTypes, i64 NUM_DEREFS_ALLOWED>
class DeepStringifier<std::tuple<TupleTypes...>, NUM_DEREFS_ALLOWED>
{
private:
    typedef std::tuple<TupleTypes...> TheTupleType;

    template <u64 INDEX>
    inline typename std::enable_if<INDEX == sizeof...(TupleTypes), void>::type
    stringify_tuple(const TheTupleType& the_tuple, i64 verbosity, std::ostream& out) const
    {
        return;
    }

    template <u64 INDEX>
    inline typename std::enable_if<INDEX != sizeof...(TupleTypes), void>::type
    stringify_tuple(const TheTupleType& the_tuple, i64 verbosity, std::ostream& out) const
    {
        typedef typename std::tuple_element<INDEX, TheTupleType>::type ElemType;
        DeepStringifier<ElemType, NUM_DEREFS_ALLOWED> sub_stringifier;

        if (INDEX > 0) {
            out << ", ";
        }

        out << sub_stringifier(std::get<INDEX>(the_tuple), verbosity);
        stringify_tuple<INDEX+1>(the_tuple, verbosity, out);
    }

public:
    inline std::string operator () (const TheTupleType& the_tuple, i64 verbosity) const
    {
        std::ostringstream sstr;
        sstr << "<";
        stringify_tuple<0>(the_tuple, verbosity, sstr);
        sstr << ">";
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

// define deep to_string
template <u64 NUM_DEREFS_ALLOWED, typename T>
inline std::string deep_to_string(const T& object)
{
    DeepStringifier<T, NUM_DEREFS_ALLOWED> stringifier;
    return stringifier(object);
}

} /* end namespace stringification */
} /* end namespace aurum */

#endif /* AURUM_STRINGIFICATION_STRINGIFIERS_HPP_ */

//
// Stringifiers.hpp ends here
