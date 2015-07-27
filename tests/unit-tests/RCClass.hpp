// RCClass.hpp ---
// Filename: RCClass.hpp
// Author: Abhishek Udupa
// Created: Mon Mar  2 22:43:00 2015 (-0500)
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

#if !defined AURUM_TESTS_UNIT_TESTS_RC_CLASS_HPP_
#define AURUM_TESTS_UNIT_TESTS_RC_CLASS_HPP_

#include "../../src/basetypes/RefCountable.hpp"

using aurum::RefCountable;

class RCClass : public RefCountable<RCClass>
{
private:
    int m_data;

public:
    RCClass()
        : m_data(0)
    {
        // nothing here
    }

    RCClass(int data)
        : m_data(data)
    {
        // nothing here
    }

    RCClass(const RCClass& other)
        : m_data(other.m_data)
    {
        // nothing here
    }

    inline operator int () const
    {
        return m_data;
    }

    virtual ~RCClass()
    {
        // nothing here
    }
};

#endif /* AURUM_TESTS_UNIT_TESTS_RC_CLASS_HPP_ */

//
// RCClass.hpp ends here
