// Downcastable.hpp ---
//
// Filename: Downcastable.hpp
// Author: Abhishek Udupa
// Created: Tue Jul 14 17:00:28 2015 (-0400)
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

#if !defined AURUM_BASETYPES_DOWNCASTABLE_HPP_
#define AURUM_BASETYPES_DOWNCASTABLE_HPP_

namespace aurum {

template <typename DerivedClass>
class Downcastable
{
public:
    template <typename T>
    T* as()
    {
        return dynamic_cast<T*>(static_cast<DerivedClass*>(this));
    }

    template <typename T>
    const T* as() const
    {
        return dynamic_cast<const T*>(static_cast<const DerivedClass*>(this));
    }

    template <typename T>
    T* sas()
    {
        return static_cast<T*>(static_cast<DerivedClass*>(this));
    }

    template <typename T>
    const T* sas() const
    {
        return static_cast<const T*>(static_cast<const DerivedClass*>(this));
    }

    template <typename T>
    T& as_ref()
    {
        return dynamic_cast<T&>(static_cast<DerivedClass&>(*this));
    }

    template <typename T>
    const T& as_ref() const
    {
        return dynamic_cast<const T&>(static_cast<const DerivedClass&>(*this));
    }

    template <typename T>
    T& sas_ref()
    {
        return static_cast<T&>(static_cast<DerivedClass&>(*this));
    }

    template <typename T>
    const T& sas_ref() const
    {
        return static_cast<const T&>(static_cast<const DerivedClass&>(*this));
    }

    template <typename T>
    bool is() const
    {
        return (dynamic_cast<const T*>(static_cast<const DerivedClass*>(this)) != nullptr);
    }
};

} /* end namespace aurum */

#endif /* AURUM_BASETYPES_DOWNCASTABLE_HPP_ */

//
// Downcastable.hpp ends here
