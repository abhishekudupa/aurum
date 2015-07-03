// ProgramOptionsTests.cpp ---
//
// Filename: ProgramOptionsTests.cpp
// Author: Abhishek Udupa
// Created: Thu Jul  2 18:36:30 2015 (-0400)
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

#include <sstream>

#include "../../src/progoptions/ProgramOptions.hpp"
#include "../../src/allocators/MemoryManager.hpp"

#include <gtest/gtest.h>

namespace ap = aurum::program_options;
namespace aa = aurum::allocators;

using aurum::program_options::ProgramOptions;
using aurum::program_options::OptionValueCRef;
using aurum::program_options::OptionValueRef;
using aurum::program_options::make_value;

using aurum::u32;
using aurum::i32;
using aurum::u64;
using aurum::i64;

#define MAX_NUM_PROGOPTIONS (1024)
#define MAX_OPTION_LEN (4096)

static inline char** allocate_argv()
{
    char** retval = aa::casted_allocate_raw_cleared<char*>(sizeof(char*) * MAX_NUM_PROGOPTIONS);
    for (u32 i = 0; i < MAX_NUM_PROGOPTIONS; ++i) {
        retval[i] = aa::casted_allocate_raw_cleared<char>(MAX_OPTION_LEN);
    }
    return retval;
}

static inline void free_argv(char** argv_ptr)
{
    for (u32 i = 0; i < MAX_NUM_PROGOPTIONS; ++i) {
        aa::deallocate_raw(argv_ptr[i], MAX_OPTION_LEN);
    }

    aa::deallocate_raw(argv_ptr, sizeof(char*) * MAX_NUM_PROGOPTIONS);
}

static inline void populate_argv(char** argv_ptr, ac::Vector<std::string> argv)
{
    const u32 num_options = argv.size();

    for (u32 i = 0; i < num_options; ++i) {
        memset(argv_ptr[i], 0, MAX_OPTION_LEN);
        memcpy(argv_ptr[i], argv[i].c_str(), argv[i].length());
    }
    return;
}


TEST(ProgramOptions, Basic1)
{
    ac::Vector<std::string> argv;
    argv.push_back("program_name");
    argv.push_back("-a");
    argv.push_back("12345");
    argv.push_back("-b");
    argv.push_back("Test String");
    argv.push_back("-c");
    argv.push_back("123.45");
    argv.push_back("-d");

    char** argv_ptr = allocate_argv();
    populate_argv(argv_ptr, argv);

    u32 option_a_value;
    std::string option_b_value;
    float option_c_value;

    ProgramOptions program_options;

    program_options.add_option("option-a", "a", "Option a",
                               make_value<u32>(&option_a_value));
    program_options.add_option("option-b", "b", "Option b",
                               make_value<std::string>(&option_b_value));
    program_options.add_option("option-c", "c", "Option c",
                               make_value<float>(&option_c_value));
    program_options.add_option("option-d", "d", "Option d");

    std::ostringstream sstr;
    sstr << program_options;

    EXPECT_EQ("--option-a, -a:\n    Option a\n--option-b, -b:\n    Option b\n--option-c, -c:\n    Option c\n--option-d, -d:\n    Option d\n",
              sstr.str());

    program_options.parse_command_line(argv.size(), argv_ptr);

    EXPECT_EQ(12345, option_a_value);
    EXPECT_EQ("Test String", option_b_value);
    EXPECT_LE(123.4f, option_c_value);
    EXPECT_GE(123.5f, option_c_value);

    free_argv(argv_ptr);
}

TEST(ProgramOptions, Basic2)
{
    ac::Vector<std::string> argv;
    argv.push_back("program_name");
    argv.push_back("-a");
    argv.push_back("4,5,6,7,8");

    ac::Vector<u32> option_a_value;

    ProgramOptions program_options;
    program_options.add_option("option-a", "a", "Option a", make_value<ac::Vector<u32> >(&option_a_value)->multitoken()->separator(','));

    auto argv_ptr = allocate_argv();
    populate_argv(argv_ptr, argv);

    program_options.parse_command_line(argv.size(), argv_ptr);

    EXPECT_EQ((u64)5, option_a_value.size());
    EXPECT_EQ(4, option_a_value[0]);
    EXPECT_EQ(5, option_a_value[1]);
    EXPECT_EQ(6, option_a_value[2]);
    EXPECT_EQ(7, option_a_value[3]);
    EXPECT_EQ(8, option_a_value[4]);

    free_argv(argv_ptr);
}

TEST(ProgramOptions, Basic3)
{
    ac::Vector<std::string> argv;
    argv.push_back("program_name");
    argv.push_back("-a");
    argv.push_back("4,");
    argv.push_back("5,");
    argv.push_back("6,");
    argv.push_back("--option-b");
    argv.push_back("Test String");

    ac::Vector<u32> option_a_value;
    std::string option_b_value;

    ProgramOptions program_options;
    program_options.add_option("option-a", "a", "Option a", make_value<ac::Vector<u32> >(&option_a_value)->multitoken()->separator(','));
    program_options.add_option("option-b", "b", "Option b", make_value<std::string>(&option_b_value));

    auto argv_ptr = allocate_argv();
    populate_argv(argv_ptr, argv);

    program_options.parse_command_line(argv.size(), argv_ptr);

    EXPECT_EQ((u64)3, option_a_value.size());
    EXPECT_EQ(4, option_a_value[0]);
    EXPECT_EQ(5, option_a_value[1]);
    EXPECT_EQ(6, option_a_value[2]);


    free_argv(argv_ptr);
}


//
// ProgramOptionsTests.cpp ends here
