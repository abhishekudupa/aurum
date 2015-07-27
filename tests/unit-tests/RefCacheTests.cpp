// RefCacheTests.cpp ---
//
// Filename: RefCacheTests.cpp
// Author: Abhishek Udupa
// Created: Mon Jul 27 13:17:34 2015 (-0400)
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

#include "../../src/containers/RefCache.hpp"
#include "../../src/memory/ManagedPointer.hpp"
#include "../../src/containers/Vector.hpp"
#include "../../src/containers/UnorderedMap.hpp"
#include "../../src/basetypes/AurumErrors.hpp"
#include "../../src/basetypes/Hashable.hpp"
#include "../../src/basetypes/Stringifiable.hpp"
#include "../../src/basetypes/EComparable.hpp"

#include <gtest/gtest.h>

using aurum::u32;
using aurum::u64;
using aurum::i32;
using aurum::i64;

using aurum::RefCountable;
using aurum::AurumObject;
using aurum::Stringifiable;
using aurum::memory::ManagedPointer;
using aurum::memory::ManagedConstPointer;
using aurum::containers::Vector;
using aurum::containers::UnifiedUnorderedMap;
using aurum::containers::RefCache;
using aurum::AurumException;
using aurum::Hashable;
using aurum::EComparable;

enum class ExprType { VarExpr, ConstExpr, OpExpr };

class ExprBase : public AurumObject<ExprBase>,
                 public Stringifiable<ExprBase>,
                 public RefCountable<ExprBase>,
                 public Hashable<ExprBase>,
                 public EComparable<ExprBase>
{
private:
    // static counters for testing
    static u64 num_objects_alive;

    typedef ManagedConstPointer<ExprBase> ExprRef;
    typedef Vector<ExprRef> ChildVector;
    typedef std::string string;
    typedef AurumObject<ExprBase> ObjectBase;

    struct OpExprData
    {
        string m_op_name;
        ChildVector m_children;
    };

    union Data
    {
        OpExprData m_op_expr_data;
        string m_var_expr_name;
        i64 m_const_expr_value;

        Data() {}
        ~Data() {}
    };

    // fields
    ExprType m_expr_type;
    Data m_data;

public:
    typedef UnifiedUnorderedMap<std::string, i64> EvalMap;

    // overloads for object counting
    void* operator new (std::size_t sz)
    {
        ++num_objects_alive;
        return ObjectBase::operator new(sz);
    }

    void operator delete (void* ptr, std::size_t sz)
    {
        --num_objects_alive;
        ObjectBase::operator delete(ptr, sz);
    }

    void operator delete[](void* ptr) = delete;
    void operator delete[](void* ptr, std::size_t sz) = delete;

    static u64 get_num_objects_alive()
    {
        return num_objects_alive;
    }

    explicit ExprBase(i64 const_value)
        : m_expr_type(ExprType::ConstExpr), m_data()
    {
        m_data.m_const_expr_value = const_value;
    }

    explicit ExprBase(const std::string& var_name)
        : m_expr_type(ExprType::VarExpr), m_data()
    {
        new (&(m_data.m_var_expr_name)) std::string(var_name);
    }

    ExprBase(const std::string& op_name, const ChildVector& children)
        : m_expr_type(ExprType::OpExpr)
    {
        new (&(m_data.m_op_expr_data.m_op_name)) std::string(op_name);
        new (&(m_data.m_op_expr_data.m_children)) ChildVector(children);
    }

    ExprBase(const std::string& op_name, std::initializer_list<ExprRef> children)
        : m_expr_type(ExprType::OpExpr)
    {
        new (&(m_data.m_op_expr_data.m_op_name)) std::string(op_name);
        new (&(m_data.m_op_expr_data.m_children)) ChildVector(children);
    }

    virtual ~ExprBase()
    {
        switch (m_expr_type) {
        case ExprType::ConstExpr:
            break;
        case ExprType::VarExpr:
            m_data.m_var_expr_name.~string();
            break;
        case ExprType::OpExpr:
            m_data.m_op_expr_data.m_op_name.~string();
            m_data.m_op_expr_data.m_children.~ChildVector();
            break;
        default:
            break;
        }
    }

    i64 evaluate(const EvalMap& eval_map) const
    {
        switch (m_expr_type) {
        case ExprType::VarExpr:
        {
            auto it = eval_map.find(m_data.m_var_expr_name);
            if (it == eval_map.end()) {
                throw AurumException((std::string)"Unable to evaluate variable \"" + m_data.m_var_expr_name +
                                     "\" with the given eval_map");
            }
            return it->second;
        }
        case ExprType::ConstExpr:
            return m_data.m_const_expr_value;
        case ExprType::OpExpr:
        {
            auto const& op_name = m_data.m_op_expr_data.m_op_name;
            auto const& children = m_data.m_op_expr_data.m_children;
            auto const num_children = children.size();
            Vector<u64> child_evaluations(num_children);

            for (u64 i = 0; i < num_children; ++i) {
                child_evaluations[i] = children[i]->evaluate(eval_map);
            }

            i64 retval = 0;
            if (op_name == "+") {
                std::for_each(child_evaluations.begin(), child_evaluations.end(),
                              [&](const i64 value) -> void
                              {
                                  retval += value;
                              });
            } else if (op_name == "-") {
                retval = child_evaluations[0];
                std::for_each(child_evaluations.begin() + 1, child_evaluations.end(),
                              [&] (const i64 value) -> void
                              {
                                  retval -= value;
                              });
            } else if (op_name == "/") {
                retval = child_evaluations[0];
                std::for_each(child_evaluations.begin() + 1, child_evaluations.end(),
                              [&] (const i64 value) -> void
                              {
                                  retval /= value;
                              });
            } else if (op_name == "*") {
                retval = 1;
                std::for_each(child_evaluations.begin(), child_evaluations.end(),
                              [&] (const i64 value) -> void
                              {
                                  retval *= value;
                              });
            } else {
                throw AurumException("Strange operand: " + op_name);
            }
            return retval;
        }
        }
        return 0;
    }

    // hash function
    u64 compute_hash_value() const
    {
        switch (m_expr_type) {
        case ExprType::VarExpr:
        {
            auto type_hash = aurum::hashing::integer_hash(0x10000000);
            aurum::hashing::Hasher<std::string> var_hasher;
            auto var_hash = var_hasher(m_data.m_var_expr_name);
            return (type_hash ^ var_hash);
        }

        case ExprType::ConstExpr:
        {
            auto type_hash = aurum::hashing::integer_hash(0x20000000);
            aurum::hashing::Hasher<i64> val_hasher;
            auto val_hash = val_hasher(m_data.m_const_expr_value);
            return (type_hash ^ val_hash);
        }

        case ExprType::OpExpr:
        {
            auto type_hash = aurum::hashing::integer_hash(0x30000000);
            aurum::hashing::Hasher<std::string> op_hasher;
            auto op_hash = op_hasher(m_data.m_op_expr_data.m_op_name);
            auto retval = (type_hash ^ op_hash);
            auto const& children = m_data.m_op_expr_data.m_children;
            for (auto const& child : children) {
                auto hash_value = child->hash();
                retval = retval ^ hash_value;
            }
            return retval;
        }
        default:
            return 0;
        }
    }

    // compare function
    bool equal_to(const ExprBase& other) const
    {
        if (m_expr_type != other.m_expr_type) {
            return false;
        }
        switch (m_expr_type) {
        case ExprType::VarExpr:
            return (m_data.m_var_expr_name == other.m_data.m_var_expr_name);
        case ExprType::ConstExpr:
            return (m_data.m_const_expr_value == other.m_data.m_const_expr_value);
        case ExprType::OpExpr:
        {
            if (m_data.m_op_expr_data.m_op_name != other.m_data.m_op_expr_data.m_op_name) {
                return false;
            }
            auto const& children = m_data.m_op_expr_data.m_children;
            auto const& other_children = other.m_data.m_op_expr_data.m_children;
            auto const num_children = children.size();
            auto const other_num_children = other_children.size();

            if (num_children != other_num_children) {
                return false;
            }
            for (u64 i = 0; i < num_children; ++i) {
                if (*(children[i]) != *(other_children[i])) {
                    return false;
                }
            }
            return true;
        }
        default:
            return false;
        }
    }

    // stringification
    std::string as_string(i64 verbosity) const
    {
        switch(m_expr_type) {
        case ExprType::ConstExpr:
            return std::to_string(m_data.m_const_expr_value);
        case ExprType::VarExpr:
            return m_data.m_var_expr_name;
        case ExprType::OpExpr:
        {
            std::ostringstream sstr;
            sstr << "(" << m_data.m_op_expr_data.m_op_name << " ";
            auto const& children = m_data.m_op_expr_data.m_children;
            auto const num_children = children.size();

            for (u64 i = 0; i < num_children; ++i) {
                sstr << children[i]->to_string();
                if (i != num_children - 1) {
                    sstr << " ";
                }
            }
            sstr << ")";
            return sstr.str();
        }
        }
        return "";
    }
};

// initialization of static class variable
u64 ExprBase::num_objects_alive = 0;

typedef RefCache<ExprBase> ExprCache;
typedef ManagedConstPointer<ExprBase> ExprRef;
typedef aurum::containers::Vector<ExprRef> ExprVector;

TEST(RefCacheTest, BasicOne)
{
    ExprCache the_cache;

    EXPECT_EQ(0ul, ExprBase::get_num_objects_alive());

    ExprRef var_a_exp = the_cache.get<ExprBase>("varA");
    EXPECT_EQ(1ul, ExprBase::get_num_objects_alive());
    ExprRef var_b_exp = the_cache.get<ExprBase>("varB");
    EXPECT_EQ(2ul, ExprBase::get_num_objects_alive());

    ExprBase::EvalMap eval_map;
    eval_map["varA"] = 42;
    eval_map["varB"] = 4200;

    ExprRef a_plus_b_exp = the_cache.get<ExprBase>("+", ExprVector { var_a_exp, var_b_exp });
    EXPECT_EQ(3ul, ExprBase::get_num_objects_alive());
    EXPECT_EQ(4242l, a_plus_b_exp->evaluate(eval_map));

    ExprRef a_times_b_exp = the_cache.get<ExprBase>("*", ExprVector { var_a_exp, var_b_exp });
    EXPECT_EQ(4ul, ExprBase::get_num_objects_alive());
    EXPECT_EQ(176400l, a_times_b_exp->evaluate(eval_map));

    a_plus_b_exp = nullptr;
    the_cache.garbage_collect();

    EXPECT_EQ(3ul, ExprBase::get_num_objects_alive());

    EXPECT_EQ(176400l, a_times_b_exp->evaluate(eval_map));

    ExprRef var_a_exp_copy = the_cache.get<ExprBase>("varA");
    EXPECT_EQ(var_a_exp, var_a_exp_copy);
}

//
// RefCacheTests.cpp ends here
