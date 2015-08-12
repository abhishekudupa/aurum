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

#include "CodeFilter.hpp"

namespace aurum {
namespace io {

CodeFilter::CodeFilter(std::streambuf* chained_buffer, u64 buffer_size)
    : SequentialOutputFilterBase(chained_buffer, buffer_size),
      m_spaces_per_indent(sc_default_num_spaces_per_indent),
      m_indent_level(0), m_current_buffer_position(0),
      m_indent_stack(), m_indentation_string(""), m_on_newline(true)
{
    // we'll manage our buffers ourselves, thank you very much!
    // just get sputc, etc to call overflow every single time.
    setp(m_buffer, m_buffer);
}

CodeFilter::~CodeFilter()
{
    // Nothing here
}

std::streamsize CodeFilter::xsputn(char_type* s, std::streamsize n)
{
    for (u64 i = 0; i < n; ++i) {
        sputc(s[i]);
    }
}

// requires that there be enough space in the buffer
inline void CodeFilter::make_indented_newline()
{
    m_buffer[m_current_buffer_position++] = ch;
    memcpy(m_buffer + m_current_buffer_position, m_indentation_string.c_str(),
           m_indentation_string.length());
    m_current_buffer_position += m_indentation_string.length();
    return;
}

CodeFilter::int_type CodeFilter::overflow(int_type ch)
{
    if (traits_type::eq_int_type(ch, traits_type::eof())) {
        if (m_current_buffer_position < m_buffer_size) {
            return 0;
        } else {
            m_chained_buffer->sputn((char_type*)m_buffer, m_current_buffer_position);
            return 0;
        }
    }
    if (ch == '\n') {
        if (m_current_buffer_position + m_indentation_string.length() + 1 < m_buffer_size) {
            make_indented_newline();
        } else {
            m_chained_buffer->sputn((char_type*)m_buffer, m_current_buffer_position);
            m_current_buffer_position = 0;
            make_indented_newline();
        }
    } else {
        if (m_current_buffer_position < m_buffer_size) {
            m_buffer[m_current_buffer_position++] = ch;
        } else {
            m_chained_buffer->sputn((char_type*)m_buffer, m_current_buffer_position);
            m_current_buffer_position = 0;
            m_buffer[m_current_buffer_position++] = ch;
        }
    }
    return 0;
}

int CodeFilter::sync()
{
    m_chained_buffer->sputn((char_type*)m_buffer, m_current_buffer_position);
    m_current_buffer_position = 0;
}

} /* end namespace io */
} /* end namespace aurum */

//
// CodeFilter.cpp ends here
