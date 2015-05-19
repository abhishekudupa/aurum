// ProgramOptions.hpp ---
//
// Filename: ProgramOptions.hpp
// Author: Abhishek Udupa
// Created: Fri May 15 16:21:08 2015 (-0400)
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

#if !defined AURUM_PROGOPTIONS_PROGRAM_OPTIONS_HPP_
#define AURUM_PROGOPTIONS_PROGRAM_OPTIONS_HPP_

#include "../basetypes/AurumTypes.hpp"
#include "../containers/Vector.hpp"
#include "../memory/RefCountable.hpp"
#include "../memory/ManagedPointer.hpp"
#include "../containers/UnorderedMap.hpp"

namespace aurum {
namespace program_options {

namespace ac = aurum::containers;
namespace am = aurum::memory;

class OptionBase;

typedef am::ManagedConstPointer<OptionBase> OptionRef;

class UnsupportedOptionConversion : public AurumException
{
public:
    inline UnsupportedOptionConversion() noexcept
        : AurumException()
    {
        // Nothing here
    }

    inline UnsupportedOptionConversion(const std::string& exception_info) noexcept
        : AurumException(exception_info)
    {
        // Nothing here
    }

    inline UnsupportedOptionConversion(const UnsupportedOptionConversion& other) noexcept
        : AurumException(other.m_exception_info)
    {
        // Nothing here
    }

    inline UnsupportedOptionConversion(UnsupportedOptionConversion&& other) noexcept
        : AurumException(std::move(other.m_exception_info))
    {
        // Nothing here
    }

    virtual ~UnsupportedOptionConversion() noexcept
    {
        // Nothing here
    }

    inline UnsupportedOptionConversion&
    operator = (const UnsupportedOptionConversion& other) noexcept
    {
        AurumException::operator=(other);
        return *this;
    }

    inline UnsupportedOptionConversion&
    operator = (UnsupportedOptionConversion&& other) noexcept
    {
        if (&other == this) {
            return *this;
        }
        std::swap(m_exception_info, other.m_exception_info);
        return *this;
    }
};

class OptionBase : public AurumObject, public Downcastable<OptionBase>,
                   public am::RefCountable
{
private:
    bool m_is_named;
    std::string m_option_name;
    u64 m_option_position;

protected:
    inline void throw_unsupported_conversion(const std::string& message) const
    {
        throw UnsupportedOptionConversion(message);
    }

public:
    OptionBase(const std::string& option_name);
    OptionBase(u64 option_position);
    virtual ~OptionBase();

    const std::string& get_option_name() const;
    u64 get_option_position() const;
    bool is_option_named() const;
    bool is_option_positional() const;

    inline operator int () const
    {
        return (int)get_value_as_signed_integer();
    }

    inline operator unsigned int() const
    {
        return (unsigned int)get_value_as_unsigned_integer();
    }

    inline operator long () const
    {
        return (long)get_value_as_signed_integer();
    }

    inline operator unsigned long () const
    {
        return (unsigned long)get_value_as_unsigned_integer();
    }

    inline operator double () const
    {
        return get_value_as_double();
    }

    inline operator std::string () const
    {
        return get_value_as_string();
    }

    // abstract methods
    virtual std::string get_value_as_string() const = 0;
    virtual i64 get_value_as_signed_integer() const = 0;
    virtual u64 get_value_as_unsigned_integer() const = 0;
    virtual double get_value_as_double() const = 0;
    virtual const ac::Vector<OptionRef>& get_value_as_list() const = 0;

    // parsing methods
    virtual void parse_option_value(const std::string& option_value) = 0;
};

class IntegerOption : public OptionBase
{
private:
    i64 m_option_value;

public:
    IntegerOption(const std::string& option_name);
    IntegerOption(u64 option_position);

    virtual ~IntegerOption();

    virtual std::string get_value_as_string() const override;
    virtual i64 get_value_as_signed_integer() const override;
    virtual u64 get_value_as_unsigned_integer() const override;
    virtual double get_value_as_double() const override;
    virtual const ac::Vector<OptionRef>& get_value_as_list() const override;

    virtual void parse_option_value(const std::string& option_value) override;
};

class UnsignedOption : public OptionBase
{
private:
    u64 m_option_value;

public:
    UnsignedOption(const std::string& option_name);
    UnsignedOption(u64 option_position);

    virtual ~UnsignedOption();

    virtual std::string get_value_as_string() const override;
    virtual i64 get_value_as_signed_integer() const override;
    virtual u64 get_value_as_unsigned_integer() const override;
    virtual double get_value_as_double() const override;
    virtual const ac::Vector<OptionRef>& get_value_as_list() const override;

    virtual void parse_option_value(const std::string& option_value) override;
};

class RealOption : public OptionBase
{
private:
    double m_option_value;

public:
    RealOption(const std::string& option_name);
    RealOption(u64 option_position);

    virtual ~RealOption();

    virtual std::string get_value_as_string() const override;
    virtual i64 get_value_as_signed_integer() const override;
    virtual u64 get_value_as_unsigned_integer() const override;
    virtual double get_value_as_double() const override;
    virtual const ac::Vector<OptionRef>& get_value_as_list() const override;

    virtual void parse_option_value(const std::string& option_value) override;
};

class StringOption : public OptionBase
{
private:
    std::string m_option_value;

public:
    StringOption(const std::string& option_name);
    StringOption(u64 option_position);

    virtual ~StringOption();

    virtual std::string get_value_as_string() const override;
    virtual i64 get_value_as_signed_integer() const override;
    virtual u64 get_value_as_unsigned_integer() const override;
    virtual double get_value_as_double() const override;
    virtual const ac::Vector<OptionRef>& get_value_as_list() const override;

    virtual void parse_option_value(const std::string& option_value) override;
};

class ListOption : public OptionBase
{
private:
    ac::Vector<OptionRef> m_option_value();

public:
    ListOption(const std::string& option_name);
    ListOption(u64 option_position);

    virtual ~ListOption();

    virtual std::string get_value_as_string() const override;
    virtual i64 get_value_as_signed_integer() const override;
    virtual u64 get_value_as_unsigned_integer() const override;
    virtual double get_value_as_double() const override;
    virtual const ac::Vector<OptionRef>& get_value_as_list() const override;

    virtual void parse_option_value(const std::string& option_value) override;
};

class ProgramOptionMap : public AurumObject
{
    friend class ProgramOptionParser;
private:
    ac::RestrictedUnorderedMap<std::string, OptionRef> m_prog_option_map;

public:
    typedef ac::RestrictedUnorderedMap<std::string, OptionRef>::ConstIterator ConstIterator;
    typedef ConstIterator const_iterator;

    ProgramOptionMap();
    ~ProgramOptionMap();

    void insert(const std::string& option_name, const OptionRef& option_value);
    void insert(u64 option_position, const OptionRef& option_value);
    void erase(const std::string& option_name);
    void erase(u64 option_position);
    OptionRef find(const std::string& option_name) const;
    OptionRef find(u64 option_position) const;
    OptionRef operator [] (const std::string& option_name) const;
    OptionRef operator [] (u64 option_position) const;
    ConstIterator begin() const;
    ConstIterator end() const;
};

class ProgramOptionParser : public AurumObject
{
private:
    ProgramOptionMap m_program_option_map;

public:
    typedef
    std::function<std::pair<bool, std::string>(const OptionRef&, const ProgramOptionMap&)>
    ProgOptCheckerFun;

    ProgramOptionParser(bool unknown_option_is_error = false);
    ProgramOptionParser(const ProgramOptionParser& other) = delete;
    ProgramOptionParser(ProgramOptionParser&& other) = delete;

    ~ProgramOptionParser();

    ProgramOptionParser& operator = (const ProgramOptionParser& other) = delete;
    ProgramOptionParser& operator = (ProgramOptionParser&& other) = delete;

    template <typename T>
    inline void register_program_option(const std::string& name,
                                        const std::string& description,
                                        const ProgOptCheckerFun& check_function);

    std::string get_option_descriptions() const;

    const ProgramOptionMap&
    parse_command_line(int argc, char* argv[],
                       const std::function<bool(const ProgramOptionMap&)>& global_option_checker);

    const ProgramOptionMap& parse_command_line(int argc, char* argv[]);
};

} /* end namespace program_options */
} /* end namespace aurum */

#endif /* AURUM_PROGOPTIONS_PROGRAM_OPTIONS_HPP_ */

//
// ProgramOptions.hpp ends here
