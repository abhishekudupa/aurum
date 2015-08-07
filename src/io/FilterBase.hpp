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

#include <type_traits>

#include "../basetypes/AurumTypes.hpp"

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

class IOFilterBase : public AurumObject<FilterBase>,
                     public std::streambuf
{
protected:
    std::streambuf* m_piped_buffer;

    inline void throw_on_unsupported_operation(const std::string& operation_name) const;

public:
    IOFilterBase(std::streambuf* piped_buffer);
    IOFilterBase(const FilterBase& other);
    IOFilterBase(FilterBase&& other);
    virtual ~IOFilterBase();

    IOFilterBase& operator = (const IOFilterBase& other);
    IOFilterBase& operator = (IOFilterBase&& other);

    std::streambuf* get_piped_buffer() const;
    void set_piped_buffer(std::streambuf* buffer);

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
    using BaseType::BaseType;
    virtual ~FilterBase();

    FilterBase& operator = (const FilterBase& other);
    FilterBase& operator = (FilterBase&& other);

protected:
    virtual void imbue(const std::locale& loc) override;
    virtual std::streambuf* setbuf(char_type* s, std::streamsize n) override;
    virtual pos_type seekoff(off_type offset, std::ios_base::seekdir dir,
                             std::ios_base::openmode which =
                             std::ios_base::in | std::ios_base::out) override;
};

} /* end namespace io */
} /* end namespace aurum */

#endif /* AURUM_IO_FILTER_BASE_HPP_ */

//
// FilterBase.hpp ends here
