// OptionValue.hpp ---
//
// Filename: OptionValue.hpp
// Author: Abhishek Udupa
// Created: Wed May 20 12:44:46 2015 (-0400)
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

#if !defined AURUM_PROGOPTIONS_OPTION_VALUE_HPP_
#define AURUM_PROGOPTIONS_OPTION_VALUE_HPP_

#include <string>
#include <typeinfo>
#include <functional>

#include "../basetypes/AurumTypes.hpp"
#include "../basetypes/RefCountable.hpp"
#include "../basetypes/Downcastable.hpp"
#include "../memory/ManagedPointer.hpp"
#include "../stringification/StrUtils.hpp"
#include "../containers/Vector.hpp"

#include "ProgramOptionException.hpp"

namespace aurum {
namespace program_options {

namespace am = aurum::memory;

class OptionMap;

class OptionValueBase : public AurumObject<OptionValueBase>,
                        public RefCountable<OptionValueBase>,
                        public Downcastable<OptionValueBase>
{
protected:
    std::string m_textual_value;

    bool m_is_required;
    bool m_has_default_value;
    std::string m_textual_default_value;
    bool m_has_implicit_value;
    std::string m_textual_implicit_value;
    bool m_has_been_stored_to;
    bool m_is_multitoken;
    bool m_is_positional;
    // separator for multitoken strings
    // default is to use a comma
    char m_separator;

public:
    OptionValueBase();

    virtual ~OptionValueBase();

    bool is_required() const;
    bool has_default_value() const;
    bool has_implicit_value() const;
    char get_separator() const;
    bool has_been_stored_to() const;
    bool is_multitoken() const;
    bool is_positional() const;

    OptionValueBase* required();
    OptionValueBase* default_value(const std::string& textual_default_value);
    OptionValueBase* implicit_value(const std::string& textual_implicit_value);
    OptionValueBase* separator(char separator);
    OptionValueBase* multitoken();
    OptionValueBase* positional();

    const std::string& get_textual_default_value() const;
    const std::string& get_textual_implicit_value() const;
    const std::string& get_textual_value() const;

    virtual void store(const std::string& textual_value) = 0;
    virtual void store(const ac::Vector<std::string>& textual_values) = 0;

    template <typename TargetType>
    inline TargetType get_value() const
    {
        strutils::StringCaster<TargetType> caster;
        try {
            return caster(m_textual_value);
        } catch (strutils::StringConversionException& e) {
            throw ProgramOptionException((std::string)"Bad conversion in " +
                                         "OptionValueBase::get_value(). " +
                                         "Details:\n" + e.what());
        }
    }
};

typedef am::ManagedPointer<OptionValueBase> OptionValueRef;
typedef am::ManagedConstPointer<OptionValueBase> OptionValueCRef;

template <typename ValueType>
class OptionValueImpl : public OptionValueBase
{
protected:
    ValueType* m_storage_ptr;
    // local checks performed during store
    std::function<bool(const ValueType&)> m_local_option_checker;

    inline void rethrow_storage_exception(const AurumException& exception)
    {
        throw ProgramOptionException((std::string)"Error storing program options. Details:\n" +
                                     exception.what());
    }

    inline void run_local_check()
    {
        if (!m_local_option_checker(*m_storage_ptr)) {
            throw ProgramOptionException("Local check failed while storing program option.");
        }
    }

    template <typename CastType>
    inline void do_store(const std::string& textual_value)
    {
        strutils::StringCaster<CastType> caster;
        try{
            *m_storage_ptr = caster(textual_value);
        } catch (const AurumException& e) {
            rethrow_storage_exception(e);
        }
        run_local_check();
        m_has_been_stored_to = true;
    }

    template <typename CastType>
    inline void do_store(const ac::Vector<std::string>& textual_values)
    {
        if (textual_values.size() != 1) {
            throw ProgramOptionException((std::string)"Called do_store on scalar type " +
                                         "with more than one value string");
        }
        strutils::StringCaster<CastType> caster;
        try {
            *m_storage_ptr = caster(textual_values[0]);
        } catch (const AurumException& e) {
            rethrow_storage_exception(e);
        }
        run_local_check();
        m_has_been_stored_to = true;
    }


public:
    inline OptionValueImpl(ValueType* storage_ptr)
        : OptionValueBase(),
          m_storage_ptr(storage_ptr),
          m_local_option_checker([] (const ValueType& unused) -> bool { return true; })
    {
        // Nothing here
    }

    inline OptionValueImpl(ValueType* storage_ptr,
                           const std::function<bool(const ValueType&)>& local_option_checker)
        : OptionValueBase(),
          m_storage_ptr(storage_ptr),
          m_local_option_checker(local_option_checker)
    {
        // Nothing here
    }

    virtual ~OptionValueImpl()
    {
        // Nothing here
    }
};

template <typename ValueType>
class OptionValue : public OptionValueImpl<ValueType>
{
    static_assert(true, "Unsupported instantiation of class aurum::program_options::OptionValue");
};

template <>
class OptionValue<bool> : public OptionValueImpl<bool>
{
public:
    using OptionValueImpl<bool>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<i64> : public OptionValueImpl<i64>
{
public:
    using OptionValueImpl<i64>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<i32> : public OptionValueImpl<i32>
{
public:
    using OptionValueImpl<i32>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<i16> : public OptionValueImpl<i16>
{
public:
    using OptionValueImpl<i16>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<i08> : public OptionValueImpl<i08>
{
public:
    using OptionValueImpl<i08>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<u64> : public OptionValueImpl<u64>
{
public:
    using OptionValueImpl<u64>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<u32> : public OptionValueImpl<u32>
{
public:
    using OptionValueImpl<u32>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<u16> : public OptionValueImpl<u16>
{
public:
    using OptionValueImpl<u16>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<u08> : public OptionValueImpl<u08>
{
public:
    using OptionValueImpl<u08>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<float> : public OptionValueImpl<float>
{
public:
    using OptionValueImpl<float>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<double> : public OptionValueImpl<double>
{
public:
    using OptionValueImpl<double>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <>
class OptionValue<std::string> : public OptionValueImpl<std::string>
{
public:
    using OptionValueImpl<std::string>::OptionValueImpl;
    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <typename T1, typename T2>
class OptionValue<std::pair<T1, T2>> : public OptionValueImpl<std::pair<T1, T2> >
{
private:
    typedef std::pair<T1, T2> ValueType;
    typedef OptionValueImpl<ValueType> BaseType;

public:
    OptionValue(ValueType* storage_ptr);
    OptionValue(ValueType* storage_ptr,
                const std::function<bool(const ValueType&)>& local_option_checker);

    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <typename... TupleTypes>
class OptionValue<std::tuple<TupleTypes...> >
    : public OptionValueImpl<std::tuple<TupleTypes...> >
{
private:
    typedef std::tuple<TupleTypes...> ValueType;
    typedef OptionValueImpl<ValueType> BaseType;

public:
    OptionValue(ValueType* storage_ptr);
    OptionValue(ValueType* storage_ptr,
                const std::function<bool(const ValueType&)>& local_option_checker);

    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <typename ElemType>
class OptionValue<ac::Vector<ElemType> > : public OptionValueImpl<ac::Vector<ElemType> >
{
private:
    typedef ac::Vector<ElemType> ValueType;
    typedef OptionValueImpl<ValueType> BaseType;

public:
    OptionValue(ValueType* storage_ptr);
    OptionValue(ValueType* storage_ptr,
                const std::function<bool(const ValueType&)>& local_option_checker);

    virtual ~OptionValue();

    virtual void store(const std::string& textual_value) override;
    virtual void store(const ac::Vector<std::string>& textual_values) override;
};

template <typename ValueType>
inline OptionValueRef make_value(ValueType* storage_ptr)
{
    return new OptionValue<ValueType>(storage_ptr);
}

static inline void trim_components(ac::Vector<std::string>& component_vector)
{
    for (auto& component : component_vector) {
        strutils::unquote_string(component);
        strutils::trim(component);
    }
}

template <typename T1, typename T2>
inline void OptionValue<std::pair<T1, T2> >::store(const std::string& textual_value)
{
    try {
        strutils::StringCaster<std::pair<T1, T2> > caster;
        *(this->m_storage_ptr) = caster(textual_value, this->m_separator);
    } catch (const strutils::StringConversionException& e) {
        this->rethrow_storage_exception(e);
    }

    this->run_local_check();
    this->m_has_been_stored_to = true;
}

template <typename T1, typename T2>
inline void OptionValue<std::pair<T1, T2> >::store(const ac::Vector<std::string> &textual_values)
{
    if (textual_values.size() != 2) {
        throw ProgramOptionException((std::string)"Incorrect number of values in store() to " +
                                     "a pair OptionValue.");
    }
    try {
        strutils::StringCaster<T1> caster1;
        strutils::StringCaster<T2> caster2;
        *(this->m_storage_ptr) = std::make_pair(caster1(textual_values[0]),
                                                caster2(textual_values[1]));
    } catch (const strutils::StringConversionException& e) {
        this->rethrow_storage_exception(e);
    }
}

template <typename... TupleTypes>
inline void OptionValue<std::tuple<TupleTypes...> >::store(const std::string& textual_value)
{
    try {
        strutils::StringCaster<std::tuple<TupleTypes...> > caster;
        *(this->m_storage_ptr) = caster(textual_value, this->m_separator);
    } catch (const strutils::StringConversionException& e) {
        this->rethrow_storage_exception(e);
    }
    this->run_local_check();
    this->m_has_been_stored_to = true;
}

namespace detail_ {

template <u64 INDEX, typename... TupleTypes>
inline typename std::enable_if<INDEX == sizeof...(TupleTypes), void>::type
populate_tuple(std::tuple<TupleTypes...>& the_tuple,
               const ac::Vector<std::string>& textual_values)
{
    return;
}

template <u64 INDEX, typename... TupleTypes>
inline typename std::enable_if<INDEX != sizeof...(TupleTypes), void>::type
populate_tuple(std::tuple<TupleTypes...>& the_tuple,
               const ac::Vector<std::string>& textual_values)
{
    typedef std::tuple<TupleTypes...> TheTupleType;
    typedef typename std::tuple_element<INDEX, TheTupleType>::type ElemType;
    strutils::StringCaster<ElemType> caster;

    std::get<INDEX>(the_tuple) = caster(textual_values[INDEX]);

    populate_tuple<INDEX+1, TupleTypes...>(the_tuple, textual_values);
}

} /* end namespace detail_ */

template <typename... TupleTypes>
inline void
OptionValue<std::tuple<TupleTypes...> >::store(const ac::Vector<std::string>& textual_values)
{
    if (textual_values.size() != sizeof...(TupleTypes)) {
        throw ProgramOptionException((std::string)"Incorrect number of values in store() to " +
                                     "a tuple OptionValue.");
    }
    detail_::populate_tuple<0, TupleTypes...>(*(this->m_storage_ptr), textual_values);
    this->run_local_check();
    this->m_has_been_stored_to = true;
}

template <typename ElemType>
inline void OptionValue<ac::Vector<ElemType> >::store(const std::string& textual_value)
{
    this->m_storage_ptr->clear();
    try {
        strutils::StringCaster<ac::Vector<ElemType> > caster;
        *(this->m_storage_ptr) = caster(textual_value, this->m_separator);
    } catch (const strutils::StringConversionException& e) {
        this->rethrow_storage_exception(e);
    }

    this->run_local_check();
    this->m_has_been_stored_to = true;
}

template <typename ElemType>
inline void OptionValue<ac::Vector<ElemType> >::store(const ac::Vector<std::string>& textual_values)
{
    this->m_storage_ptr->clear();
    strutils::StringCaster<ElemType> caster;
    try {
        for (auto const& textual_value : textual_values) {
            this->m_storage_ptr->push_back(caster(textual_value));
        }
    } catch (const AurumException& e) {
        this->rethrow_storage_exception(e);
    }
    this->run_local_check();
    this->m_has_been_stored_to = true;
}

template <typename T1, typename T2>
inline OptionValue<std::pair<T1, T2> >::OptionValue(ValueType* storage_ptr)
    : BaseType(storage_ptr)
{
    this->multitoken();
}

template <typename T1, typename T2>
inline
OptionValue<std::pair<T1, T2> >::OptionValue(ValueType* storage_ptr,
                                             const std::function<bool(const ValueType&)>&
                                             local_option_checker)
    : BaseType(storage_ptr, local_option_checker)
{
    this->multitoken();
}

template <typename... TupleTypes>
inline OptionValue<std::tuple<TupleTypes...> >::OptionValue(ValueType* storage_ptr)
    : BaseType(storage_ptr)
{
    this->multitoken();
}

template <typename... TupleTypes>
inline
OptionValue<std::tuple<TupleTypes...> >::OptionValue(ValueType* storage_ptr,
                                                     const std::function<bool(const ValueType&)>&
                                                     local_option_checker)
    : BaseType(storage_ptr, local_option_checker)
{
    this->multitoken();
}

template <typename ElemType>
inline OptionValue<ac::Vector<ElemType> >::OptionValue(ValueType* storage_ptr)
    : BaseType(storage_ptr)
{
    this->multitoken();
}

template <typename ElemType>
inline
OptionValue<ac::Vector<ElemType> >::OptionValue(ValueType* storage_ptr,
                                                const std::function<bool(const ValueType&)>&
                                                local_option_checker)
    : BaseType(storage_ptr, local_option_checker)
{
    this->multitoken();
}

template <typename ElemType>
inline OptionValue<ac::Vector<ElemType> >::~OptionValue()
{
    // Nothing here
}

template <typename T1, typename T2>
inline OptionValue<std::pair<T1, T2> >::~OptionValue()
{
    // Nothing here
}

template <typename... TupleTypes>
inline OptionValue<std::tuple<TupleTypes...> >::~OptionValue()
{
    // Nothing here
}

} /* end namespace program_options */
} /* end namespace aurum */

#endif /* AURUM_PROGOPTIONS_OPTION_VALUE_HPP_ */

//
// OptionValue.hpp ends here
