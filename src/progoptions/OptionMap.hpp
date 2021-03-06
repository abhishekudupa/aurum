// OptionMap.hpp ---
//
// Filename: OptionMap.hpp
// Author: Abhishek Udupa
// Created: Wed May 20 14:28:53 2015 (-0400)
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

#if !defined AURUM_PROGOPTIONS_OPTION_MAP_HPP_
#define AURUM_PROGOPTIONS_OPTION_MAP_HPP_

#include "../basetypes/AurumTypes.hpp"
#include "../containers/UnorderedMap.hpp"

#include "OptionValue.hpp"

namespace aurum {
namespace program_options {

namespace ac = aurum::containers;

class OptionMap : public AurumObject<OptionMap>
{
public:
    typedef ac::UnifiedUnorderedMap<std::string, OptionValueRef> OptionMapType;
    typedef OptionMapType::ConstIterator ConstIterator;
    typedef OptionMapType::Iterator Iterator;
    typedef ConstIterator const_iterator;
    typedef Iterator iterator;

private:
    OptionMapType m_option_map;

public:
    OptionMap();
    ~OptionMap();

    u64 size() const;

    const OptionValueRef& operator [] (const std::string& option_name) const;
    const OptionValueRef& operator [] (const u64 option_position) const;
    const OptionValueRef& find(const std::string& option_name) const;
    const OptionValueRef& find(u64 option_position) const;

    void insert(const std::string& option_name, const OptionValueRef& option_value);
    void erase(const std::string& option_name);

    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;

    ConstIterator cbegin() const;
    ConstIterator cend() const;
};

} /* end namespace program_options */
} /* end namespace aurum */

#endif /* AURUM_PROGOPTIONS_OPTION_MAP_HPP_ */

//
// OptionMap.hpp ends here
