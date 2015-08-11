// FilteredStreams.hpp ---
//
// Filename: FilteredStreams.hpp
// Author: Abhishek Udupa
// Created: Mon Aug 10 16:27:57 2015 (-0400)
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

#if !defined AURUM_IO_FILTERED_STREAMS_HPP_
#define AURUM_IO_FILTERED_STREAMS_HPP_

#include <ostream>
#include <istream>
#include <fstream>
#include <sstream>

#include "../basetypes/AurumTypes.hpp"

#include "Categories.hpp"
#include "FilterBase.hpp"

namespace aurum {
namespace io {

namespace detail_ {

class FilteredInputStreamBase
    : public AurumObject<FilteredInputStreamBase>,
      public std::istream

{
protected:
    std::streambuf* m_chain;
    std::streambuf* m_chain_root;
    bool m_chain_root_owned;

    FilteredInputStreamBase();

public:
    FilteredInputStreamBase(std::streambuf* chain_root);
    FilteredInputStreamBase(const FilteredInputStreamBase& other) = delete;
    FilteredInputStreamBase(FilteredInputStreamBase&& other);

    virtual ~FilteredInputStreamBase();

    FilteredInputStreamBase& operator = (const FilteredInputStreamBase& other) = delete;
    FilteredInputStreamBase& operator = (FilteredInputStreamBase&& other);

    // Do NOT include chained filter in the args!
    template <typename FilterType, typename... ArgTypes>
    inline void push(ArgTypes&&... args)
    {
        static_assert(IsInputFilter<FilterType>::value,
                      "Only input filters can be pushed onto FilteredInputStreams!");
        auto new_filter = new FilterType(m_chain, std::forward<ArgTypes>(args)...);
        m_chain = new_filter;
        set_rdbuf(m_chain);
    }

    std::streambuf* peek() const;
    std::streambuf* pop();
    std::streambuf* get_root() const;
};

class FilteredOutputStreamBase
    : public AurumObject<FilteredOutputStreamBase>,
      public std::ostream

{
protected:
    std::streambuf* m_chain;
    std::streambuf* m_chain_root;
    bool m_chain_root_owned;

    FilteredOutputStreamBase();

public:
    FilteredOutputStreamBase(std::streambuf* chain_root);
    FilteredOutputStreamBase(const FilteredOutputStreamBase& other) = delete;
    FilteredOutputStreamBase(FilteredOutputStreamBase&& other);

    virtual ~FilteredOutputStreamBase();

    FilteredOutputStreamBase& operator = (const FilteredOutputStreamBase& other) = delete;
    FilteredOutputStreamBase& operator = (FilteredOutputStreamBase&& other);

    // Do NOT include chained filter in the args!
    template <typename FilterType, typename... ArgTypes>
    inline void push(ArgTypes&&... args)
    {
        static_assert(IsOutputFilter<FilterType>::value,
                      "Only output filters can be pushed onto FilteredOutputStreams!");
        auto new_filter = new FilterType(m_chain, std::forward<ArgTypes>(args)...);
        m_chain = new_filter;
        set_rdbuf(m_chain);
    }

    std::streambuf* peek() const;
    std::streambuf* pop();
    std::streambuf* get_root() const;
};

} /* end namespace detail_ */

class FilteredIStream
    : public detail_::FilteredInputStreamBase
{
protected:
    FilteredIStream();

public:
    FilteredIStream(const FilteredIStream& other) = delete;

    explicit FilteredIStream(std::streambuf* stream_buffer);
    // for files
    explicit FilteredIStream(const char* filename);
    explicit FilteredIStream(const std::string& filename);

    FilteredIStream(FilteredIStream&& other);
    virtual ~FilteredIStream();

    FilteredIStream& operator = (const FilteredIStream& other) = delete;
    FilteredIStream& operator = (FilteredIStream&& other);
};

class FilteredOStream
    : public detail_::FilteredOutputStreamBase
{
protected:
    FilteredOStream();

public:
    FilteredOStream(const FilteredOStream& other) = delete;

    explicit FilteredOStream(std::streambuf* stream_buffer);
    // for files
    explicit FilteredOStream(const char* filename);
    explicit FilteredOStream(const std::string& filename);
    FilteredOStream(FilteredOStream&& other);

    virtual ~FilteredOStream();
    FilteredOStream& operator = (const FilteredOStream& other) = delete;
    FilteredOStream& operator = (FilteredOStream&& other);
};


} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_FILTERED_STREAMS_HPP_ */

//
// FilteredStreams.hpp ends here
