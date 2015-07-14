// AurumTypes.hpp ---
// Filename: AurumTypes.hpp
// Author: Abhishek Udupa
// Created: Fri Feb 13 19:00:04 2015 (-0500)
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

#if !defined AURUM_BASETYPES_AURUM_TYPES_HPP_
#define AURUM_BASETYPES_AURUM_TYPES_HPP_

#include <ostream>
#include <type_traits>
#include <string>
#include <cstddef>

#ifdef __GLIBCXX__
#include <cxxabi.h>
#endif /* __GLIBCXX__ */

#include "../memory/RefCountable.hpp"
#include "../memory/ManagedPointer.hpp"
#include "../allocators/MemoryManager.hpp"

#include "AurumBase.hpp"

namespace aurum {

namespace aa = aurum::allocators;

namespace detail_ {

class AurumObjectBaseEBC
{
    // Nothing here
};

} /* end namespace detail_ */

// A base class for objects
// can be allocated on heap or auto
template <typename DerivedClass>
class AurumObject : public detail_::AurumObjectBaseEBC
{
public:
    inline void* operator new(std::size_t sz)
    {
        return aa::allocate_raw(sz);
    }

    inline void* operator new[](std::size_t sz)
    {
        return aa::allocate_raw(sz);
    }

    inline void* operator new(std::size_t sz, void* ptr)
    {
        return ptr;
    }

    inline void* operator new[](std::size_t count, void* ptr)
    {
        return ptr;
    }

    inline void operator delete(void* ptr, std::size_t sz)
    {
        aa::deallocate_raw(ptr, sz);
    }

    inline void operator delete[](void* ptr, std::size_t sz)
    {
        aa::deallocate_raw(ptr, sz);
    }

    inline AurumObject()
    {
        // Nothing here
    }

    inline ~AurumObject()
    {
        // Nothing here
    }
};

namespace detail_ {

template <typename Derived>
class AurumHeapOnlyHelper : public Derived
{
public:
    AurumHeapOnlyHelper() = delete;

    template <typename... ArgTypes>
    inline AurumHeapOnlyHelper(ArgTypes&&... args)
        : Derived(std::forward<ArgTypes>(args)...)
    {
        // Nothing here
    }

    inline ~AurumHeapOnlyHelper()
    {
        // Nothing here
    }
};

class AurumHeapOnlyEBC
{
    // Nothing here
};

} /* end namespace detail_ */

// Base class for objects that can only be allocated
// on the heap
// Usage: derive publicly from this class and
// then make all constructors protected in the derived
// class
template <typename Derived>
class AurumHeapOnly : public detail_::AurumHeapOnlyEBC
{
protected:
    inline AurumHeapOnly()
    {
        // Nothing here
    }

    inline ~AurumHeapOnly()
    {
        // Nothing here
    }

public:
    template <typename... ArgTypes>
    static Derived* create(ArgTypes&&... args)
    {
        return new detail_::AurumHeapOnlyHelper<Derived>(std::forward<ArgTypes>(args)...);
    }
};

// Base class for objects that can only be auto allocated
class AurumAutoOnly
{
public:
    void* operator new(std::size_t sz) = delete;
    void* operator new[](std::size_t sz) = delete;
    void operator delete(void* ptr) = delete;
    void operator delete[](void* ptr) = delete;
    void operator delete(void* ptr, std::size_t sz) = delete;
    void operator delete[](void* ptr, std::size_t sz) = delete;
};


// Base class for noncopyable objects
class AurumNonCopyable
{
public:
    AurumNonCopyable(const AurumNonCopyable& other) = delete;
    AurumNonCopyable(AurumNonCopyable&& other) = delete;
    AurumNonCopyable& operator = (const AurumNonCopyable& other) = delete;
    AurumNonCopyable& operator = (AurumNonCopyable&& other) = delete;
};

// forward declarations of other classes
namespace detail_ {

class StringifiableEBC;
class HashableEBC;
class EComparableEBC;
class ComparableEBC;
class CloneableEBC;

} /* end namespace detail_ */

template <typename Derived> class Downcastable;
template <typename Derived> class Stringifiable;
template <typename Derived> class Hashable;
template <typename Derived> class EComparable;
template <typename Derived> class Comparable;
template <typename Derived> class Clonable;

class Interruptible
{
protected:
    mutable volatile bool m_interrupted;

public:
    inline Interruptible()
        : m_interrupted(false)
    {
        // Nothing here
    }

    inline void interrupt() const
    {
        m_interrupted = true;
    }

    inline void reset_interrupted() const
    {
        m_interrupted = false;
    }
};


class AurumException : public std::exception
{
protected:
    std::string m_exception_info;

public:
    AurumException()
        : m_exception_info("No information about exception")
    {
        // Nothing here
    }

    AurumException(const std::string& exception_info) noexcept
        : m_exception_info(exception_info)
    {
        // Nothing here
    }

    AurumException(std::string&& exception_info) noexcept
        : m_exception_info(std::move(exception_info))
    {
        // Nothing here
    }

    AurumException(const AurumException& other) noexcept
        : m_exception_info(other.m_exception_info)
    {
        // Nothing here
    }

    AurumException(AurumException&& other) noexcept
        : m_exception_info(std::move(other.m_exception_info))
    {
        // Nothing here
    }

    virtual ~AurumException() noexcept
    {
        // Nothing here
    }

    inline AurumException& operator = (const AurumException& other) noexcept
    {
        if (&other == this) {
            return *this;
        }
        m_exception_info = other.m_exception_info;
        return *this;
    }

    inline AurumException& operator = (AurumException&& other) noexcept
    {
        if (&other == this) {
            return *this;
        }

        std::swap(m_exception_info, other.m_exception_info);
        return *this;
    }

    virtual const char* what() const noexcept override
    {
        return m_exception_info.c_str();
    }

    const std::string& get_exception_info() const
    {
        return m_exception_info;
    }
};

template <typename T>
inline std::string type_name()
{
    std::string tname = typeid(T).name();

#ifdef __GLIBCXX__
    int status;
    char* demangled_name = abi::__cxa_demangle(tname.c_str(), nullptr, nullptr, &status);
    if (status == 0) {
        tname = demangled_name;
        std::free(demangled_name);
    }
#endif /* __GLIBCXX__ */

    return tname;
}

} /* end namespace aurum */

#endif /* AURUM_BASETYPES_AURUM_TYPES_HPP_ */

//
// AurumTypes.hpp ends here
