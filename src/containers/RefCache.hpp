// RefCache.hpp ---
//
// Filename: RefCache.hpp
// Author: Abhishek Udupa
// Created: Mon Jul 20 15:57:26 2015 (-0400)
//
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

#if !defined AURUM_CONTAINERS_REF_CACHE_HPP_
#define AURUM_CONTAINERS_REF_CACHE_HPP_

#include "../basetypes/AurumTypes.hpp"
#include "../basetypes/EComparable.hpp"
#include "../basetypes/Hashable.hpp"
#include "../basetypes/RefCountable.hpp"
#include "../memory/ManagedPointer.hpp"
#include "../allocators/MemoryManager.hpp"
#include "../hashing/Hashers.hpp"
#include "../comparisons/Comparators.hpp"

#include "HashTable.hpp"

namespace aurum {
namespace containers {
namespace ref_cache_detail_ {

namespace ac = aurum::containers;
namespace am = aurum::memory;
namespace aa = aurum::allocators;

template <typename T, typename HashFunction, typename EqualsFunction>
class RefCache
    : public AurumObject<ac::ref_cache_detail_::RefCache<T, HashFunction, EqualsFunction> >
{
    // ensure that the type is ref countable
    static_assert(IsRefCountable<T>::value,
                  "RefCaches can only be created with RefCountable types.");

private:
    RestrictedHashTable<T*, HashFunction, EqualsFunction> m_hash_table;
    u64 m_next_gc_limit;
    float m_growth_factor;
    static constexpr float sc_default_growth_factor = 1.5f;
    static constexpr u64 sc_initial_gc_limit = 128;

    inline void do_gc()
    {
        m_hash_table.begin_multi_erase_sequence();

        bool deleted_this_iteration;

        do {
            deleted_this_iteration = false;
            for (auto it = m_hash_table.begin(), last = m_hash_table.end(); it != last; ++it) {
                if ((*it)->get_ref_count_() == 1) {
                    deleted_this_iteration = true;
                    delete (*it);
                    m_hash_table.erase(it);
                }
            }
        } while (deleted_this_iteration);

        m_hash_table.end_multi_erase_sequence();
    }

    inline void try_gc()
    {
        if (m_hash_table.size() >= m_next_gc_limit) {
            do_gc();
            m_next_gc_limit = (u64)ceilf((float)m_hash_table.size() * m_growth_factor);
        } else {
            return;
        }
    }

public:
    typedef am::ManagedPointer<T> RefType;
    typedef am::ManagedConstPointer<T> CRefType;

    inline RefCache()
        : m_hash_table(),
          m_next_gc_limit(sc_initial_gc_limit),
          m_growth_factor(sc_default_growth_factor)
    {
        // Nothing here
    }

    explicit inline RefCache(float growth_factor)
        : RefCache()
    {
        m_growth_factor = growth_factor;
    }

    inline ~RefCache()
    {
        // just decrement the ref counts of
        // everything. If anyone is still holding
        // refs, then they ought to be via managed ptrs
        // and should be deleted eventually
        for (auto const& ptr : m_hash_table) {
            ptr->dec_ref_();
        }

        m_hash_table.clear();
    }

    template <typename U, typename... ArgTypes>
    inline U* get(ArgTypes&&... args)
    {
        static_assert(std::is_convertible<U*, T*>::value,
                      "RefCache: Cannot call get on unrelated type.");

        try_gc();

        auto new_object = new U(std::forward<ArgTypes>(args)...);

        auto it = m_hash_table.find(new_object);
        if (it == m_hash_table.end()) {
            new_object->inc_ref_();
            bool unused;
            m_hash_table.insert(new_object, unused);
            return new_object;
        } else {
            delete new_object;
            return (*it);
        }
    }

    inline u64 size() const
    {
        return m_hash_table.size();
    }

    inline void garbage_collect()
    {
        do_gc();
    }

    inline void set_growth_factor(float new_growth_factor)
    {
        // sanity: ensure that the growth factor is at least 10%
        if (new_growth_factor <= 1.1f) {
            return;
        }
        m_growth_factor = new_growth_factor;
    }

    inline float get_growth_factor() const
    {
        return m_growth_factor;
    }
};

} /* end namespace detail_ */

// exported typedefs for convenience
template <typename T,
          typename HashFunction = aurum::hashing::DeepHasher<const T*, 1>,
          typename EqualsFunction = aurum::comparisons::DeepEqualTo<const T*, 1> >
using RefCache = ref_cache_detail_::RefCache<T, HashFunction, EqualsFunction>;

} /* end namespace containers */
} /* end namespace aurum */

#endif /* AURUM_CONTAINERS_REF_CACHE_HPP_ */

//
// RefCache.hpp ends here
