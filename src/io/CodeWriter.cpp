// CodeWriter.cpp ---
//
// Filename: CodeWriter.cpp
// Author: Abhishek Udupa
// Created: Thu Aug 13 16:36:39 2015 (-0400)
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

#include "CodeWriter.hpp"

namespace aurum {
namespace io {

constexpr u32 CodeWriter::sc_default_buffer_size;

CodeWriter::CodeWriter(std::streambuf* stream_buffer, u32 spaces_per_indent)
    : FilteredOStream(stream_buffer)
{
    // push a formatting filter on top of the filterchain
    auto code_filter = new CodeFilter(stream_buffer, sc_default_buffer_size, spaces_per_indent);
    FilteredOStream::push_filter(code_filter);
}

CodeWriter::CodeWriter(const char* filename, u32 spaces_per_indent)
    : FilteredOStream(filename)
{
    auto code_filter = new CodeFilter(m_chain, sc_default_buffer_size, spaces_per_indent);
    FilteredOStream::push_filter(code_filter);
}

CodeWriter::CodeWriter(const std::string& filename, u32 spaces_per_indent)
    : CodeWriter(filename.c_str(), spaces_per_indent)
{
    // Nothing here
}

CodeWriter::~CodeWriter()
{
    // Nothing here
}

// overrides
std::streambuf* CodeWriter::pop_filter()
{
    // do not allow the CodeFilter to be popped
    CodeFilter* code_filter = static_cast<CodeFilter*>(FilteredOStream::pop_filter());
    auto retval = FilteredOStream::pop_filter();
    detail_::IOFilterBase* retval_as_filter = static_cast<detail_::IOFilterBase*>(nullptr);

    if (retval_as_filter == nullptr) {
        FilteredOStream::push_filter(code_filter);
        return nullptr;
    } else {
        code_filter->set_chained_buffer(retval_as_filter->get_chained_buffer());
        FilteredOStream::push_filter(code_filter);
        return retval;
    }
}

std::streambuf* CodeWriter::peek_filter() const
{
    auto top_filter = static_cast<detail_::IOFilterBase*>(FilteredOStream::peek_filter());
    return top_filter->get_chained_buffer();
}

void CodeWriter::push_filter(detail_::IOFilterBase* filter)
{
    CodeFilter* code_filter = static_cast<CodeFilter*>(FilteredOStream::pop_filter());
    filter->set_chained_buffer(code_filter->get_chained_buffer());
    code_filter->set_chained_buffer(filter);
    FilteredOStream::push_filter(filter);
    FilteredOStream::push_filter(code_filter);
}

inline CodeFilter* CodeWriter::get_code_filter() const
{
    return static_cast<CodeFilter*>(FilteredOStream::peek_filter());
}

u32 CodeWriter::get_spaces_per_indent() const
{
    return get_code_filter()->get_spaces_per_indent();
}

void CodeWriter::increment_indent()
{
    get_code_filter()->increment_indent();
}

void CodeWriter::decrement_indent()
{
    get_code_filter()->decrement_indent();
}

void CodeWriter::set_indent_level(u32 indent_level)
{
    get_code_filter()->set_indent_level(indent_level);
}

u32 CodeWriter::get_indent_level() const
{
    return get_code_filter()->get_indent_level();
}

u32 CodeWriter::get_indent_level_in_spaces() const
{
    return get_code_filter()->get_indent_level_in_spaces();
}

void CodeWriter::push_indent()
{
    get_code_filter()->push_indent();
}

void CodeWriter::pop_indent()
{
    get_code_filter()->pop_indent();
}

} /* end namespace io */
} /* end namespace aurum */

//
// CodeWriter.cpp ends here
