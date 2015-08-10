// FilterBase.hpp ---
//
// Filename: FilterBase.hpp
// Author: Abhishek Udupa
// Created: Fri Jul 31 16:32:46 2015 (-0400)
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

#if !defined AURUM_IO_FILTER_BASE_HPP_
#define AURUM_IO_FILTER_BASE_HPP_

#include <streambuf>

#include "../basetypes/AurumTypes.hpp"
#include "../basetypes/AurumTraits.hpp"

namespace aurum {
namespace io {

struct IOCategoryBase
{};

struct Input : IOCategoryBase
{};

struct Output : IOCategoryBase
{};

struct AccessCategoryBase
{};

struct Sequential
{};

struct RandomAccess
{};

namespace detail_ {

class IOFilterBase : public AurumObject<IOFilterBase>,
                     public std::streambuf,
                     public AurumNonCopyable
{
protected:
    // some constants for default buffer sizes
    static constexpr u64 sc_default_buffer_size = 16384;

    // variables
    const u64 m_buffer_size;
    u08* m_buffer;
    std::streambuf* m_chained_buffer;

    void throw_on_unsupported_operation(const std::string& operation_name) const;

public:
    IOFilterBase(std::streambuf* chained_buffer, u64 buffer_size = sc_default_buffer_size);
    virtual ~IOFilterBase();

    std::streambuf* get_chained_buffer() const;
    void set_chained_buffer(std::streambuf* buffer);

    // the protected functions are as in std::streambuf
};

} /* end namespace detail_ */

// selectively disable some overloads
// this class should never be instantiated,
// because we provide specializations for all the relevant
// instantiations
template <typename IOCategory, typename AccessCategory>
class FilterBase : public detail_::IOFilterBase
{
    static_assert(((std::is_same<IOCategory, Input>::value ||
                    std::is_same<IOCategory, Output>::value) ||
                   (!std::is_same<IOCategory, Input>::value &&
                    !std::is_same<IOCategory, Output>::value)),
                  "Attempted to instantiate FilterBase with unsupported IOCategory");
    static_assert(((std::is_same<AccessCategory, Sequential>::value ||
                    std::is_same<AccessCategory, RandomAccess>::value) ||
                   (!std::is_same<AccessCategory, Sequential>::value &&
                    !std::is_same<AccessCategory, RandomAccess>::value)),
                  "Attempted to instantiate FilterBase with unsupported AccessCategory");
};

template <>
class FilterBase<Input, Sequential> : public detail_::IOFilterBase
{
private:
    typedef detail_::IOFilterBase BaseType;

public:
    FilterBase(std::streambuf* chained_buffer, u64 buffer_size = sc_default_buffer_size);
    virtual ~FilterBase();

protected:
    virtual void imbue(const std::locale& loc) override;
    virtual std::streambuf* setbuf(char_type* s, std::streamsize n) override;
    virtual pos_type seekoff(off_type offset, std::ios_base::seekdir dir,
                             std::ios_base::openmode which =
                             std::ios_base::in | std::ios_base::out) override;
    virtual pos_type seekpos(pos_type pos,
                             std::ios_base::openmode which =
                             std::ios_base::in | std::ios_base::out) override;
    virtual std::streamsize xsputn(const char_type* s, std::streamsize count) override;
    virtual int_type overflow(int_type ch = traits_type::eof()) override;
};

template <>
class FilterBase<Input, RandomAccess> : public detail_::IOFilterBase
{
private:
    typedef detail_::IOFilterBase BaseType;

public:
    FilterBase(std::streambuf* chained_buffer, u64 buffer_size = sc_default_buffer_size);
    virtual ~FilterBase();

protected:
    virtual void imbue(const std::locale& loc) override;
    virtual std::streambuf* setbuf(char_type* s, std::streamsize n) override;
    virtual std::streamsize xsputn(const char_type* s, std::streamsize count) override;
    virtual int_type overflow(int_type ch = traits_type::eof()) override;
};

template <>
class FilterBase<Output, Sequential> : public detail_::IOFilterBase
{
private:
    typedef detail_::IOFilterBase BaseType;

public:
    FilterBase(std::streambuf* chained_buffer, u64 buffer_size = sc_default_buffer_size);
    virtual ~FilterBase();

protected:
    virtual void imbue(const std::locale& loc) override;
    virtual std::streambuf* setbuf(char_type* s, std::streamsize n) override;
    virtual pos_type seekoff(off_type offset, std::ios_base::seekdir dir,
                             std::ios_base::openmode which =
                             std::ios_base::in | std::ios_base::out) override;
    virtual pos_type seekpos(pos_type pos,
                             std::ios_base::openmode which =
                             std::ios_base::in | std::ios_base::out) override;

    virtual std::streamsize showmanyc() override;

    virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;
    virtual int_type underflow() override;
    virtual int_type uflow() override;
};

template <>
class FilterBase<Output, RandomAccess> : public detail_::IOFilterBase
{
private:
    typedef detail_::IOFilterBase BaseType;

public:
    FilterBase(std::streambuf* chained_buffer, u64 buffer_size = sc_default_buffer_size);
    virtual ~FilterBase();

protected:
    virtual void imbue(const std::locale& loc) override;
    virtual std::streambuf* setbuf(char_type* s, std::streamsize n) override;

    virtual std::streamsize showmanyc() override;

    virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;
    virtual int_type underflow() override;
    virtual int_type uflow() override;
};

typedef FilterBase<Input, Sequential> SequentialInputFilterBase;
typedef FilterBase<Input, RandomAccess> RandomAccessInputFilterBase;

typedef FilterBase<Output, Sequential> SequentialOutputFilterBase;
typedef FilterBase<Output, RandomAccess> RandomAccessOutputFilterBase;

// type traits
template <typename T>
struct IsInputFilter
    : std::conditional<(std::is_base_of<SequentialInputFilterBase, T>::value ||
                        std::is_base_of<RandomAccessInputFilterBase, T>::value),
                       aurum::detail_::TrueStruct, aurum::detail_::FalseStruct>::type
{};

template <typename T>
struct IsOutputFilter
    : std::conditional<(std::is_base_of<SequentialOutputFilterBase, T>::value ||
                        std::is_base_of<RandomAccessOutputFilterBase, T>::value),
                       typename aurum::detail_::TrueStruct, aurum::detail_::FalseStruct>::type
{};

template <typename T>
struct IsSequentialFilter
    : std::conditional<(std::is_base_of<SequentialInputFilterBase, T>::value ||
                        std::is_base_of<SequentialOutputFilterBase, T>::value),
                       aurum::detail_::TrueStruct, aurum::detail_::FalseStruct>::type
{};

template <typename T>
struct IsRandomAccessFilter
    : std::conditional<(std::is_base_of<RandomAccessInputFilterBase, T>::value ||
                        std::is_base_of<RandomAccessOutputFilterBase, T>::value),
                       typename aurum::detail_::TrueStruct, aurum::detail_::FalseStruct>::type
{};

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_FILTER_BASE_HPP_ */

//
// FilterBase.hpp ends here
