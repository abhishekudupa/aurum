// CodeFilter.cpp ---
//
// Filename: CodeFilter.cpp
// Author: Abhishek Udupa
// Created: Wed Aug 12 17:45:53 2015 (-0400)
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

#include "AurumIOException.hpp"
#include "CodeFilter.hpp"

namespace aurum {
namespace io {

CodeFilter::CodeFilter(std::streambuf* chained_buffer, u64 buffer_size,
                       u32 spaces_per_indent)
    : SequentialOutputFilterBase(chained_buffer, buffer_size),
      m_spaces_per_indent(spaces_per_indent),
      m_indent_level(0), m_current_buffer_position(0),
      m_indent_stack(), m_on_newline(true)
{
    // we'll manage our buffers ourselves, thank you very much!
    // just get sputc, etc to call overflow every single time.
    setp((char_type*)m_buffer, (char_type*)m_buffer);
}

CodeFilter::~CodeFilter()
{
    // Nothing here
}

inline void CodeFilter::flush_and_write_indented_char(int_type ch)
{
    u32 num_spaces = m_indent_level * m_spaces_per_indent;
    if (m_current_buffer_position + num_spaces + 1 <= m_buffer_size) {
        memset(m_buffer + m_current_buffer_position, ' ', num_spaces);
        m_current_buffer_position += num_spaces;
        m_buffer[m_current_buffer_position++] = ch;
    } else {
        m_chained_buffer->sputn((char_type*)m_buffer, m_current_buffer_position);
        memset(m_buffer, ' ', num_spaces);
        m_current_buffer_position = num_spaces;
        m_buffer[m_current_buffer_position++] = ch;
    }
}

inline void CodeFilter::flush_and_write_char(int_type ch)
{
    if (m_current_buffer_position + 1 <= m_buffer_size) {
        m_buffer[m_current_buffer_position++] = ch;
    } else {
        m_chained_buffer->sputn((char_type*)m_buffer, m_current_buffer_position);
        m_current_buffer_position = 0;
        m_buffer[m_current_buffer_position++] = ch;
    }
}

inline void CodeFilter::check_runaway_indent() const
{
    if ((u64)m_indent_level * (u64)m_spaces_per_indent > (u64)sc_max_indent_spaces) {
        throw AurumIOException((std::string)"Exceeded maximum allowed indentation " +
                               "in CodeFilter");
    }
}

std::streamsize CodeFilter::xsputn(const char_type* s, std::streamsize n)
{
    for (u64 i = 0; i < (u64)n; ++i) {
        sputc(s[i]);
    }
    return n;
}

CodeFilter::int_type CodeFilter::overflow(int_type ch)
{
    if (traits_type::eq_int_type(ch, traits_type::eof())) {
        if (m_current_buffer_position < m_buffer_size) {
            return 0;
        } else {
            m_chained_buffer->sputn((char_type*)m_buffer, m_current_buffer_position);
            m_current_buffer_position = 0;
            return 0;
        }
    }

    if (ch != '\n') {
        if (m_on_newline) {
            flush_and_write_indented_char(ch);
            m_on_newline = false;
        } else {
            flush_and_write_char(ch);
        }
    } else {
        // ch == '\n'
        if (m_on_newline) {
            flush_and_write_char(ch);
            // leave m_on_newline set
        } else {
            flush_and_write_char(ch);
        }
        m_on_newline = true;
    }
    return 0;
}

int CodeFilter::sync()
{
    m_chained_buffer->sputn((char_type*)m_buffer, m_current_buffer_position);
    m_current_buffer_position = 0;
    return m_chained_buffer->pubsync();
}

u32 CodeFilter::get_spaces_per_indent() const
{
    return m_spaces_per_indent;
}

void CodeFilter::increment_indent()
{
    ++m_indent_level;
    check_runaway_indent();
}

void CodeFilter::decrement_indent()
{
    --m_indent_level;
    check_runaway_indent();
}

void CodeFilter::set_indent_level(u32 indent_level)
{
    m_indent_level = indent_level;
    check_runaway_indent();
}

u32 CodeFilter::get_indent_level() const
{
    return m_indent_level;
}

u32 CodeFilter::get_indent_level_in_spaces() const
{
    return (m_indent_level * m_spaces_per_indent);
}

void CodeFilter::push_indent()
{
    m_indent_stack.push(m_indent_level);
}

void CodeFilter::pop_indent()
{
    m_indent_level = m_indent_stack.top();
    m_indent_stack.pop();
}

} /* end namespace io */
} /* end namespace aurum */

//
// CodeFilter.cpp ends here
