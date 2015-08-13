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

template <typename IOCategory>
class FilteredStreamBase
    : public AurumObject<FilteredStreamBase<IOCategory> >,
      public std::conditional<std::is_same<IOCategory, Input>::value,
                              std::istream, std::ostream>::type
{
    static_assert(std::is_same<IOCategory, Input>::value ||
                  std::is_same<IOCategory, Output>::value,
                  "FilteredStreamBase can only be instantiated with Input or Output "
                  "as the IOCategory!");

protected:
    typedef FilteredStreamBase<IOCategory> FilteredStreamBaseType;

    template <typename T>
    using FilterTypeChecker =
        typename std::conditional<std::is_same<IOCategory, Input>::value,
                                  IsInputFilter<T>, IsOutputFilter<T> >::type;

    std::streambuf* m_chain;
    std::streambuf* m_chain_root;
    bool m_chain_root_owned;

    FilteredStreamBase()
        : m_chain(nullptr)
    {
        // Nothing here
    }

    virtual void push_filter(detail_::IOFilterBase* filter)
    {
        m_chain->pubsync();
        m_chain = filter;
        this->set_rdbuf(m_chain);
    }

    virtual std::streambuf* pop_filter()
    {
        if (m_chain == m_chain_root) {
            return nullptr;
        }
        m_chain->pubsync();
        auto to_pop = m_chain;
        m_chain = static_cast<IOFilterBase*>(m_chain)->get_chained_buffer();
        return to_pop;
    }

    virtual std::streambuf* peek_filter() const
    {
        return m_chain;
    }

public:
    FilteredStreamBase(std::streambuf* chain_root)
        : m_chain(chain_root), m_chain_root(chain_root), m_chain_root_owned(false)
    {
        // Nothing here
    }

    FilteredStreamBase(const FilteredStreamBase& other) = delete;

    FilteredStreamBase(FilteredStreamBase&& other)
        : FilteredStreamBase()
    {
        std::swap(m_chain, other.m_chain);
        std::swap(m_chain_root, other.m_chain_root);
        std::swap(m_chain_root_owned, other.m_chain_root_owned);
    }

    virtual ~FilteredStreamBase()
    {
        m_chain->pubsync();

        auto cur_filter = m_chain;
        while (cur_filter != m_chain_root) {
            auto to_delete = cur_filter;
            cur_filter =
                static_cast<detail_::IOFilterBase*>(cur_filter)->get_chained_buffer();
            delete to_delete;
        }

        if (m_chain_root_owned) {
            delete cur_filter;
        }
    }

    FilteredStreamBase& operator = (const FilteredStreamBase& other) = delete;

    FilteredStreamBase& operator = (FilteredStreamBase&& other)
    {
        if (&other == this) {
            return *this;
        }
        std::swap(m_chain, other.m_chain);
        std::swap(m_chain_root, other.m_chain_root);
        std::swap(m_chain_root_owned, other.m_chain_root_owned);
        return *this;
    }

    std::streambuf* peek() const
    {
        return this->peek_filter();
    }

    // a push or pop causes a sync of the buffers!
    virtual std::streambuf* pop()
    {
        return this->pop_filter();
    }

    template <typename FilterType, typename... ArgTypes>
    inline void push(ArgTypes&&... args)
    {
        static_assert(FilterTypeChecker<FilterType>::value,
                      "Wrong type of filter pushed onto a FilteredStream");

        auto new_filter = new FilterType(m_chain, std::forward<ArgTypes>(args)...);
        this->push_filter(new_filter);
    }

    // for dual-use filters
    template <template <typename> class FilterTemplate, typename... ArgTypes>
    inline void push(ArgTypes&&... args)
    {
        static_assert(FilterTypeChecker<FilterTemplate<IOCategory> >::value,
                      "Wrong type of filter pushed onto a FilteredStream");

        auto new_filter = new FilterTemplate<IOCategory>(m_chain, std::forward<ArgTypes>(args)...);
        this->push_filter(new_filter);
    }

    std::streambuf* get_root() const
    {
        return m_chain_root;
    }
};

typedef FilteredStreamBase<Input> FilteredInputStreamBase;
typedef FilteredStreamBase<Output> FilteredOutputStreamBase;

} /* end namespace detail_ */

class FilteredIStream : public detail_::FilteredInputStreamBase
{
private:
    typedef detail_::FilteredInputStreamBase FilteredInputStreamBase;

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

class FilteredOStream : public detail_::FilteredOutputStreamBase
{
private:
    typedef detail_::FilteredOutputStreamBase FilteredOutputStreamBase;
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
