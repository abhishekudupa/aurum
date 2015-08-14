// CodeWriter.hpp ---
//
// Filename: CodeWriter.hpp
// Author: Abhishek Udupa
// Created: Thu Aug 13 15:51:13 2015 (-0400)
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

#include "FilteredStreams.hpp"
#include "CodeFilter.hpp"

namespace aurum {
namespace io {

class CodeWriter : public FilteredOStream
{
private:
    static constexpr u32 sc_default_spaces_per_indent = 4;
    static constexpr u32 sc_default_buffer_size = 65536;

    inline CodeFilter* get_code_filter() const;

    // overrides
    virtual std::streambuf* pop_filter() override;
    virtual void push_filter(detail_::IOFilterBase* filter) override;
    virtual std::streambuf* peek_filter() const override;

public:
    CodeWriter(const CodeWriter& other) = delete;
    explicit CodeWriter(std::streambuf* stream_buffer,
                        u32 spaces_per_indent = sc_default_spaces_per_indent);
    explicit CodeWriter(const char* filename,
                        u32 spaces_per_indent = sc_default_spaces_per_indent);
    explicit CodeWriter(const std::string& filename,
                        u32 spaces_per_indent = sc_default_spaces_per_indent);

    CodeWriter(CodeWriter&& other) = delete;
    virtual ~CodeWriter();

    CodeWriter& operator = (const CodeWriter& other) = delete;
    CodeWriter& operator = (CodeWriter&& other) = delete;

    // forwarded methods to codefilter
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

//
// CodeWriter.hpp ends here
