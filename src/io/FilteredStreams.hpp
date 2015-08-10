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

template <typename StreamType>
class FilteredInputStreamBase
    : public AurumObject<FilteredInputStreamBase>,
      public StreamType

{
    static_assert(std::is_base_of<std::istream, StreamType>::value,
                  "FilteredInputStreamBase<> can only be instantiated "
                  "with types which are (subclasses of) std::istream!");

private:
    std::istream* m_chain;

public:
    FilteredInputStreamBase(std::istream* chain_root);
    virtual ~FilteredInputStreamBase();

    // Do NOT include chained filter in the args!
    template <typename FilterType, typename... ArgTypes>
    inline void push(ArgTypes&&... args)
    {
        static_assert(IsInputFilter<FilterType>::value,
                      "Only input filters can be pushed onto FilteredInputStreams!");
        auto new_filter = new T(m_chain, std::forward<ArgTypes>(args)...);
        m_chain = new_filter;
        set_rdbuf(m_chain);
    }

    std::istream* peek() const;
    std::istream* pop();
};

template <typename StreamType>
class FilteredOutputStreamBase
    : public AurumObject<FilteredInputStreamBase>,
      public StreamType

{
    static_assert(std::is_base_of<std::ostream, StreamType>::value,
                  "FilteredInputStreamBase<> can only be instantiated "
                  "with types which are (subclasses of) std::ostream!");

private:
    std::ostream* m_chain;

public:
    FilteredOutputStreamBase(std::ostream* chain_root);
    virtual ~FilteredOutputStreamBase();

    // Do NOT include chained filter in the args!
    template <typename FilterType, typename... ArgTypes>
    inline void push(ArgTypes&&... args)
    {
        static_assert(IsOutputFilter<FilterType>::value,
                      "Only output filters can be pushed onto FilteredOutputStreams!");
        auto new_filter = new T(m_chain, std::forward<ArgTypes>(args)...);
        m_chain = new_filter;
        set_rdbuf(m_chain);
    }

    std::ostream* peek() const;
    std::ostream* pop();
};

} /* end namespace detail_ */

class FilteredIStream
    : public detail_::FilteredInputStreamBase<std::istream>
{
public:
    FilteredIStream(const FilteredIStream& other) = delete;

    explicit FilteredIStream(std::streambuf* stream_buffer);
    FilteredIStream(FilteredIStream&& other);
    virtual ~FilteredIStream();

    FilteredIStream& operator = (const FilteredIStream& other) = delete;
    FilteredIStream& operator = (FilteredIStream&& other);
};

class FilteredOStream
    : public detail_::FilteredOutputStreamBase<std::ostream>
{
public:
    FilteredOStream(const FilteredOStream& other) = delete;

    explicit FilteredOStream(std::streambuf* stream_buffer);
    FilteredOStream(FilteredOStream&& other);

    virtual ~FilteredOStream();
    FilteredOStream& operator = (const FilteredOStream& other) = delete;
    FilteredOStream& operator = (FilteredOStream&& other);
};

class FilteredIFStream
    : public detail_::FilteredInputStreamBase<std::ifstream>
{
public:
    FilteredIFStream(const FilteredIFStream& other) = delete;

    explicit FilteredIFStream(const char* filename,
                              std::ios_base::openmode mode = std::ios_base::in);
    explicit FilteredIFStream(const std::string& filename,
                              std::ios_base::openmode mode = std::ios_base::in);
    FilteredIFStream(FilteredIFStream&& other);
    FilteredIFStream();

    virtual ~FilteredIFStream();

    FilteredIFStream& operator = (const FilteredIFStream& other) = delete;
    FilteredIFStream& operator = (FilteredIFStream&& other);
}

class FilteredOFStream
    : public detail_::FilteredOutputStreamBase<std::ofstream>
{
public:
    FilteredOFStream(const FilteredOFStream& other) = delete;

    explicit FilteredOFStream(const char* filename,
                              std::ios_base::openmode = std::ios_base::out);
    explicit FilteredOFStream(const std::string& filename,
                              std::ios_base::openmode = std::ios_base::out);
    FilteredOFStream(FilteredOFStream&& other);
    FilteredOFStream();

    virtual ~FilteredOFStream();

    FilteredOFStream& operator = (const FilteredOFStream& other) = delete;
    FilteredOFStream&& operator = (FilteredOFStream&& other);
}



} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_FILTERED_STREAMS_HPP_ */

//
// FilteredStreams.hpp ends here
