// FilteredStreamTests.cpp ---
//
// Filename: FilteredStreamTests.cpp
// Author: Abhishek Udupa
// Created: Thu Aug 13 17:07:02 2015 (-0400)
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

#include <AurumConfig.h>
#include <random>

#include "../../src/io/FilteredStreams.hpp"
#include "../../src/io/ZLibFilter.hpp"
#include "../../src/io/BZip2Filter.hpp"
#include "../../src/io/LZMAFilter.hpp"
#include "../../src/io/CodeWriter.hpp"

#include <gtest/gtest.h>

using aurum::u32;
using aurum::u64;
using aurum::i32;
using aurum::i64;
using aurum::u08;
using aurum::i08;

namespace aio = aurum::io;

namespace filtered_stream_tests {

static constexpr u32 max_test_string_len = 128;
static constexpr u32 max_num_strings = 65536;

std::default_random_engine random_generator;
std::uniform_int_distribution<u08> alpha_distribution(0, 25);
std::uniform_int_distribution<u32> strlen_distribution(1, max_test_string_len - 1);

template <template <typename> class CompressionFilterType>
struct FileNameStruct
{
    static constexpr const char* file_name = "unknown_instantiation";
};

#ifdef AURUM_CFG_HAVE_ZLIB_
template <>
struct FileNameStruct<aio::ZLibFilter>
{
    static constexpr const char* file_name = "filtered_stream_zlib_test.gz";
};

struct ZLibFilterWrapper
{
    template <typename T>
    using FilterType = aio::ZLibFilter<T>;

    typedef aio::ZLibFilter<aio::Input> InputFilterType;
    typedef aio::ZLibFilter<aio::Output> OutputFilterType;
};
#endif

#ifdef AURUM_CFG_HAVE_BZIP2_
template <>
struct FileNameStruct<aio::BZip2Filter>
{
    static constexpr const char* file_name = "filtered_stream_bzip_test.bz2";
};

struct BZip2FilterWrapper
{
    template <typename T>
    using FilterType = aio::BZip2Filter<T>;

    typedef aio::BZip2Filter<aio::Input> InputFilterType;
    typedef aio::BZip2Filter<aio::Output> OutputFilterType;
};
#endif

#ifdef AURUM_CFG_HAVE_LZMA_
template <>
struct FileNameStruct<aio::LZMAFilter>
{
    static constexpr const char* file_name = "filtered_stream_lzma_test.xz";
};

struct LZMAFilterWrapper
{
    template <typename T>
    using FilterType = aio::LZMAFilter<T>;

    typedef aio::LZMAFilter<aio::Input> InputFilterType;
    typedef aio::LZMAFilter<aio::Output> OutputFilterType;
};
#endif

static inline std::string generate_random_string()
{
    char temp_buffer[max_test_string_len];
    auto str_len = strlen_distribution(random_generator);
    for (u32 i = 0; i < str_len; ++i) {
        temp_buffer[i]  = 'a' + alpha_distribution(random_generator);
    }
    temp_buffer[str_len] = '\0';
    return std::string(temp_buffer);
}

template <template <typename> class FilterType>
static inline void make_test_file(std::ostringstream& sstr)
{
    auto filename = FileNameStruct<FilterType>::file_name;
    aio::FilteredOStream fstr(filename);
    fstr.push<FilterType>();

    for (u32 i = 0; i < max_num_strings; ++i) {
        auto&& cur_string = generate_random_string();
        sstr << cur_string << "\n";
        fstr << cur_string << "\n";
    }
}

template <template <typename> class FilterType>
static inline void read_test_file(std::ostringstream& sstr)
{
    auto filename = FileNameStruct<FilterType>::file_name;
    aio::FilteredIStream fstr(filename);
    fstr.push<FilterType>();
    char temp_buffer[1025];

    while(!fstr.eof()) {
        fstr.read(temp_buffer, 1024);
        temp_buffer[fstr.gcount()] = '\0';
        sstr << temp_buffer;
    }
}

} /* end namespace filtered_stream_tests */


template <typename CompressionFilterType>
class CompressionFilterTest : public testing::Test
{
protected:
    CompressionFilterTest() {}
    virtual ~CompressionFilterTest() {}
};

TYPED_TEST_CASE_P(CompressionFilterTest);

TYPED_TEST_P(CompressionFilterTest, Functional)
{
    std::ostringstream sstr;
    std::ostringstream read_str;

    filtered_stream_tests::make_test_file<TypeParam::template FilterType>(sstr);
    filtered_stream_tests::read_test_file<TypeParam::template FilterType>(read_str);

    EXPECT_EQ(sstr.str(), read_str.str());
}

REGISTER_TYPED_TEST_CASE_P(CompressionFilterTest,
                           Functional);

typedef testing::Types<
#ifdef AURUM_CFG_HAVE_ZLIB_
    filtered_stream_tests::ZLibFilterWrapper,
#endif
#ifdef AURUM_CFG_HAVE_LZMA_
    filtered_stream_tests::LZMAFilterWrapper,
#endif
    filtered_stream_tests::BZip2FilterWrapper
    > CompressionFilterTestInstantiations;

INSTANTIATE_TYPED_TEST_CASE_P(CompressionFilterTests,
                              CompressionFilterTest, CompressionFilterTestInstantiations);

//
// FilteredStreamTests.cpp ends here
