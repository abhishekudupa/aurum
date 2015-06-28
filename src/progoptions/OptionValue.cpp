// OptionValue.cpp ---
//
// Filename: OptionValue.cpp
// Author: Abhishek Udupa
// Created: Wed May 20 15:50:52 2015 (-0400)
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

#include "OptionValue.hpp"

namespace aurum {
namespace program_options {

OptionValueBase::OptionValueBase()
    : m_textual_value(), m_is_required(false), m_has_default_value(false),
      m_textual_default_value(), m_has_implicit_value(false), m_textual_implicit_value(),
      m_has_been_stored_to(false), m_is_multitoken(false), m_separator(",")
{
    // Nothing here
}

OptionValueBase::~OptionValueBase()
{
    // Nothing here
}

bool OptionValueBase::is_required() const
{
    return m_is_required;
}

bool OptionValueBase::has_default_value() const
{
    return m_has_default_value;
}

bool OptionValueBase::has_implicit_value() const
{
    return m_has_implicit_value;
}

bool OptionValueBase::has_been_stored_to() const
{
    return m_has_been_stored_to;
}

bool OptionValueBase::is_multitoken() const
{
    return m_is_multitoken;
}

bool OptionValueBase::is_positional() const
{
    return m_is_positional;
}

OptionValueBase* OptionValueBase::required()
{
    if (m_has_default_value) {
        throw ProgramOptionException((std::string)"Cannot mark an option as required and also " +
                                     "set a default value for it.");
    }
    m_is_required = true;
    return this;
}

OptionValueBase* OptionValueBase::default_value(const std::string& textual_default_value)
{
    if (m_is_required) {
        throw ProgramOptionException((std::string)"Cannot mark an option as required and also " +
                                     "set a default value for it.");
    }
    if (m_is_positional) {
        throw ProgramOptionException((std::string)"A positional option cannot have a default" +
                                     " value associated with it.");
    }
    m_has_default_value = true;
    m_textual_default_value = textual_default_value;
    return this;
}

OptionValueBase* OptionValueBase::implicit_value(const std::string& textual_implicit_value)
{
    if (m_is_positional) {
        throw ProgramOptionException((std::string)"A positional option cannot have an implicit" +
                                     " value associated with it.");
    }
    m_has_implicit_value = true;
    m_textual_implicit_value = textual_implicit_value;
    return this;
}

OptionValueBase* OptionValueBase::separator(const std::string& separator)
{
    m_separator = separator;
    return this;
}

OptionValueBase* OptionValueBase::multitoken()
{
    if (m_is_positional) {
        throw ProgramOptionException((std::string)"An option cannot be positional and " +
                                     "also be multitoken.");
    }
    m_is_multitoken = true;
    return this;
}

OptionValueBase* OptionValueBase::positional()
{
    if (m_is_multitoken) {
        throw ProgramOptionException((std::string)"An option cannot be positional and " +
                                     "also be multitoken.");
    }
    m_is_positional = true;
}

const std::string& OptionValueBase::get_textual_default_value() const
{
    return m_textual_default_value;
}

const std::string& OptionValueBase::get_textual_implicit_value() const
{
    return m_textual_implicit_value;
}

const std::string& OptionValueBase::get_textual_value() const
{
    return m_textual_value;
}

// implementation of store methods for specializations
void OptionValue<bool>::store(const std::string& textual_value)
{
    do_store<bool>(textual_value);
}

OptionValue<bool>::~OptionValue()
{
    // Nothing here
}

void OptionValue<i64>::store(const std::string& textual_value)
{
    do_store<i64>(textual_value);
}

OptionValue<i64>::~OptionValue()
{
    // Nothing here
}

void OptionValue<u64>::store(const std::string& textual_value)
{
    do_store<u64>(textual_value);
}

OptionValue<u64>::~OptionValue()
{
    // Nothing here
}

void OptionValue<i32>::store(const std::string& textual_value)
{
    do_store<i32>(textual_value);
}

OptionValue<i32>::~OptionValue()
{
    // Nothing here
}

void OptionValue<u32>::store(const std::string& textual_value)
{
    do_store<u32>(textual_value);
}

OptionValue<u32>::~OptionValue()
{
    // Nothing here
}

void OptionValue<i16>::store(const std::string& textual_value)
{
    do_store<i16>(textual_value);
}

OptionValue<i16>::~OptionValue()
{
    // Nothing here
}

void OptionValue<u16>::store(const std::string& textual_value)
{
    do_store<u16>(textual_value);
}

OptionValue<u16>::~OptionValue()
{
    // Nothing here
}

void OptionValue<i08>::store(const std::string& textual_value)
{
    do_store<i08>(textual_value);
}

OptionValue<i08>::~OptionValue()
{
    // Nothing here
}

void OptionValue<u08>::store(const std::string& textual_value)
{
    do_store<u08>(textual_value);
}

OptionValue<u08>::~OptionValue()
{
    // Nothing here
}

void OptionValue<float>::store(const std::string& textual_value)
{
    do_store<float>(textual_value);
}

OptionValue<float>::~OptionValue()
{
    // Nothing here
}

void OptionValue<double>::store(const std::string& textual_value)
{
    do_store<double>(textual_value);
}

OptionValue<double>::~OptionValue()
{
    // Nothing here
}

void OptionValue<std::string>::store(const std::string& textual_value)
{
    do_store<std::string>(textual_value);
}

OptionValue<std::string>::~OptionValue()
{
    // Nothing here
}

} /* end namespace program_options */
} /* end namespace aurum */

//
// OptionValue.cpp ends here
