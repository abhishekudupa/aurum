// TupleUtilsTests.cpp ---
// Filename: TupleUtilsTests.cpp
// Author: Abhishek Udupa
// Created: Mon Jul  6 20:53:31 2015 (-0400)
//
// Copyright (c) 2013, Abhishek Udupa, University of Pennsylvania
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

#include <string>
#include <sstream>

#include "../../src/containers/TupleUtils.hpp"

#include <gtest/gtest.h>

namespace ac = aurum::containers;

using aurum::u32;
using aurum::u64;

TEST(TupleUtils, Expansion)
{
    std::ostringstream sstr;
    auto tuple1 = std::tuple<u32, u64, std::string>(42, 420, "\"4200\"");
    ac::expand_tuple_and_apply(tuple1,
                               [&] (auto obj1, auto obj2, auto obj3) -> void
                               {
                                   sstr << obj1 << std::endl
                                        << obj2 << std::endl
                                        << obj3 << std::endl;
                               });
    EXPECT_EQ("42\n420\n\"4200\"\n", sstr.str());

    sstr.str("");

    float float_lvalue = 42.001f;
    auto tuple2 = std::tuple<u32, float, std::string, const char*>(42, float_lvalue,
                                                                   "42 as a string",
                                                                   "42 as a char*");
    ac::expand_tuple_and_apply(tuple2,
                               [&] (u32& obj1, float obj2,
                                    const std::string& obj3,
                                    std::string obj4) -> void
                               {
                                   sstr << obj1 << std::endl
                                        << obj2 << std::endl
                                        << obj3 << std::endl
                                        << obj4 << std::endl;
                               });

    EXPECT_EQ("42\n42.001\n42 as a string\n42 as a char*\n", sstr.str());
}

TEST(TupleUtils, ForEach)
{
    std::ostringstream sstr;
    auto print_lambda1 =
        [&] (auto obj) -> void
        {
            sstr << obj << std::endl;
        };

    auto tuple1 = std::tuple<u32, u64, std::string>(42, 420, "\"4200\"");
    ac::tuple_foreach(tuple1, print_lambda1);

    EXPECT_EQ("42\n420\n\"4200\"\n", sstr.str());

    sstr.str("");

    auto print_lambda2 =
        [&] (auto const& obj) -> void
        {
            sstr << obj << std::endl;
        };

    float float_lvalue = 42.001f;
    auto tuple2 = std::tuple<u32, float, std::string, const char*>(42, float_lvalue,
                                                                   "42 as a string",
                                                                   "42 as a char*");
    ac::tuple_foreach(tuple2, print_lambda2);

    EXPECT_EQ("42\n42.001\n42 as a string\n42 as a char*\n", sstr.str());

}

//
// TupleUtilsTests.cpp ends here
