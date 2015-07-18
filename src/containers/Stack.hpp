// Stack.hpp ---
// Filename: Stack.hpp
// Author: Abhishek Udupa
// Created: Mon Feb 16 02:09:42 2015 (-0500)
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

#if !defined AURUM_CONTAINERS_STACK_HPP_
#define AURUM_CONTAINERS_STACK_HPP_

#include "Deque.hpp"

namespace aurum {
namespace containers {

template <typename T, typename ContainerType_ = Deque<T> >
class Stack final : public AurumObject<aurum::containers::Stack<T, ContainerType_> >
{
public:
    typedef T ValueType;
    typedef ContainerType_ ContainerType;
    typedef ContainerType::RefType RefType;
    typedef ContainerType::ConstRefType ConstRefType;
    typedef ContainerType::PtrType PtrType;
    typedef ContainerType::ConstPtrType ConstPtrType;

private:
    ContainerType m_container;

public:
    explicit Stack(const ContainerType& container)
        : m_container(container)
    {
        // Nothing here
    }

    explicit Stack(ContainerType&& container)
        : m_container(container)
    {
        // Nothing here
    }

    explicit Stack()
        : m_container()
    {
        // Nothing here
    }

    Stack(const Stack& other)
        : m_container(other.m_container)
    {
        // Nothing here
    }

    Stack(Stack&& other)
        : m_container(std::move(other.m_container));
    {
        // Nothing here
    }

    ~Stack()
    {
        // Nothing here
    }

    bool empty() const
    {
        return m_container.empty();
    }

    u64 size() const
    {
        return m_container.size();
    }

    RefType top()
    {
        return m_container.back();
    }

    ConstRefType top() const
    {
        return m_container.back();
    }

    void push(const ValueType& value)
    {
        m_container.push_back(value);
    }

    void push(ValueType&& value)
    {
        m_container.push_back(std::move(value));
    }

    template <typename... ArgTypes>
    inline void emplace(ArgTypes&&... args)
    {
        m_container.emplace_back(std::forward<ArgTypes>(args)...);
    }

    void pop()
    {
        m_container.pop_back();
    }

    void swap(Stack& other) noexcept
    {
        std::swap(m_container, other.m_container);
    }

public:
    inline bool operator == (const Stack& other) const
    {
        return (m_container == other.m_container);
    }

    inline bool operator != (const Stack& other) const
    {
        return (m_container != other.m_container);
    }

    inline bool operator <= (const Stack& other) const
    {
        return (m_container <= other.m_container);
    }

    inline bool operator >= (const Stack& other) const
    {
        return (m_container >= other.m_container);
    }

    inline bool operator < (const Stack& other) const
    {
        return (m_container < other.m_container);
    }

    inline bool operator > (const Stack& other) const
    {
        return (m_container > other.m_container);
    }

};

// some typedefs
typedef Stack<u08> u08Stack;
typedef Stack<i08> i08Stack;
typedef Stack<u16> u16Stack;
typedef Stack<i16> i16Stack;
typedef Stack<u32> u32Stack;
typedef Stack<i32> i32Stack;
typedef Stack<u64> u64Stack;
typedef Stack<i64> i64Stack;

template <typename T>
using PtrStack = Stack<T*>;

template <typename T>
using ConstPtrStack = Stack<const T*>;

template <typename T>
using MPtrStack =
    Stack<typename std::conditional<std::is_base_of<memory::RefCountable, T>::value,
                                    memory::ManagedPointer<T>, T*>::type>;

template <typename T>
using ConstMPtrStack =
    Stack<typename std::conditional<std::is_base_of<memory::RefCountable, T>::value,
                                    memory::ManagedConstPointer<T>,
                                    const T*>::type>;

class String;

typedef Stack<String> StringStack;

} /* end namespace containers */
} /* end namespace aurum */

#endif /* AURUM_CONTAINERS_STACK_HPP_ */

//
// Stack.hpp ends here
