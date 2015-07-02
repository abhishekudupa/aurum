// OptionMap.cpp ---
//
// Filename: OptionMap.cpp
// Author: Abhishek Udupa
// Created: Wed May 20 18:46:36 2015 (-0400)
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

#include "OptionMap.hpp"
#include "ProgramOptions.hpp"

namespace aurum {
namespace program_options {

OptionMap::OptionMap()
    : m_option_map()
{
    // Nothing here
}

OptionMap::~OptionMap()
{
    // Nothing here
}

u64 OptionMap::size() const
{
    return m_option_map.size();
}

void OptionMap::insert(const std::string& option_name, const OptionValueRef& option_value)
{
    m_option_map[option_name] = option_value;
}

void OptionMap::erase(const std::string& option_name)
{
    m_option_map.erase(option_name);
}

OptionMap::Iterator OptionMap::begin()
{
    return m_option_map.begin();
}

OptionMap::Iterator OptionMap::end()
{
    return m_option_map.end();
}

OptionMap::ConstIterator OptionMap::begin() const
{
    return m_option_map.begin();
}

OptionMap::ConstIterator OptionMap::end() const
{
    return m_option_map.end();
}

OptionMap::ConstIterator OptionMap::cbegin() const
{
    return begin();
}

OptionMap::ConstIterator OptionMap::cend() const
{
    return end();
}

const OptionValueRef& OptionMap::operator [] (const std::string& option_name) const
{
    return find(option_name);
}

const OptionValueRef& OptionMap::operator [] (u64 option_position) const
{
    return find(option_position);
}

const OptionValueRef& OptionMap::find(const std::string& option_name) const
{
    auto it = m_option_map.find(option_name);
    if (it == m_option_map.end()) {
        return OptionValueRef::null_pointer;
    }
    return it->second;
}

const OptionValueRef& OptionMap::find(u64 option_position) const
{
    std::string index_string = gc_positional_option_prefix_ + std::to_string(option_position);
    return find(index_string);
}

} /* end namespace program_options */
} /* end namespace aurum */

//
// OptionMap.cpp ends here
