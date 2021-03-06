// CodeFilter.hpp ---
//
// Filename: CodeFilter.hpp
// Author: Abhishek Udupa
// Created: Wed Aug 12 17:39:06 2015 (-0400)
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

#if !defined AURUM_IO_CODE_FILTER_HPP_
#define AURUM_IO_CODE_FILTER_HPP_

#include "../containers/Stack.hpp"

#include "FilterBase.hpp"

namespace aurum {
namespace io {

namespace ac = aurum::containers;

class CodeFilter
    : public SequentialOutputFilterBase
{
private:
    // constants
    static constexpr u32 sc_default_num_spaces_per_indent = 4;
    static constexpr u32 sc_max_indent_spaces = 4096;

    const u32 m_spaces_per_indent;
    u32 m_indent_level;
    u32 m_current_buffer_position;
    ac::Stack<u32> m_indent_stack;
    bool m_on_newline;

    inline void flush_and_write_indented_char(int_type ch);
    inline void flush_and_write_char(int_type ch);
    inline void check_runaway_indent() const;

public:
    CodeFilter(std::streambuf* chained_buffer, u64 buffer_size = sc_default_buffer_size,
               u32 spaces_per_indent = sc_default_num_spaces_per_indent);
    CodeFilter() = delete;
    virtual ~CodeFilter();

protected:
    virtual std::streamsize xsputn(const char_type* s, std::streamsize n) override;
    virtual int_type overflow(int_type ch = traits_type::eof()) override;
    virtual int sync() override;

    // methods specific to code filter
public:
    u32 get_spaces_per_indent() const;

    void increment_indent();
    void decrement_indent();
    void set_indent_level(u32 indent_level);
    u32 get_indent_level() const;

    u32 get_indent_level_in_spaces() const;
    void push_indent();
    void pop_indent();
};

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_CODE_FILTER_HPP_ */

//
// CodeFilter.hpp ends here
