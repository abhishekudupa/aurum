// ProgramOptions.cpp ---
//
// Filename: ProgramOptions.cpp
// Author: Abhishek Udupa
// Created: Tue May 19 15:31:35 2015 (-0400)
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

#include "ProgramOptions.hpp"
#include "Parsers.hpp"

namespace aurum {
namespace program_options {

const std::string gc_positional_option_prefix_("__@position_");

namespace detail {

OptionDescription::OptionDescription()
    : m_full_name(), m_short_name(), m_description(), m_positional(false),
      m_position(UINT64_MAX)
{
    // Nothing here
}

OptionDescription::OptionDescription(const OptionDescription& other)
    : m_full_name(other.m_full_name), m_short_name(other.m_short_name),
      m_description(other.m_description), m_positional(other.m_positional),
      m_position(other.m_position)
{
    // Nothing here
}

OptionDescription::OptionDescription(OptionDescription&& other)
    : m_full_name(std::move(other.m_full_name)), m_short_name(std::move(other.m_short_name)),
      m_description(std::move(other.m_description)), m_positional(other.m_positional),
      m_position(other.m_position)
{
    // Nothing here
}

OptionDescription::OptionDescription(const std::string& full_name,
                                     const std::string& short_name,
                                     const std::string& description)
    : m_full_name(full_name), m_short_name(short_name), m_description(description),
      m_positional(false), m_position(UINT64_MAX)
{
    // Nothing here
}

OptionDescription::OptionDescription(const std::string& full_name,
                                     u64 position,
                                     const std::string& description)
    : m_full_name(full_name), m_short_name(), m_description(description),
      m_positional(true), m_position(position)
{
    // Nothing here
}

OptionDescription& OptionDescription::operator = (const OptionDescription& other)
{
    if (&other == this) {
        return *this;
    }

    m_full_name = other.m_full_name;
    m_short_name = other.m_short_name;
    m_description = other.m_description;
    m_positional = other.m_positional;
    m_position = other.m_position;
    return *this;
}

OptionDescription& OptionDescription::operator = (OptionDescription&& other)
{
    if (&other == this) {
        return *this;
    }
    std::swap(m_full_name, other.m_full_name);
    std::swap(m_short_name, other.m_short_name);
    std::swap(m_description, other.m_description);
    std::swap(m_positional, other.m_positional);
    std::swap(m_position, other.m_position);
    return *this;
}

OptionDescription::~OptionDescription()
{
    // Nothing here
}

std::string OptionDescription::to_string(u32 verbosity) const
{
    return to_string();
}

std::string OptionDescription::to_string() const
{
    std::ostringstream sstr;

    sstr << "--" << m_full_name << ", ";
    if (m_positional) {
        sstr << "positional at " << m_position;
    } else {
        sstr << "-" << m_short_name;
    }
    sstr << ":" << std::endl;
    sstr << "    " << m_description << std::endl;
    return sstr.str();
}

std::ostream& operator << (std::ostream& out, const OptionDescription& option_description)
{
    out << option_description.to_string();
    return out;
}

} /* end namespace detail */

ProgramOptions::ProgramOptions()
    : OptionMap(), m_anon_option_values(), m_next_positional_option((u64)0),
      m_num_specified_options_parsed(0), m_num_unspecified_options_parsed(0)
{
    // Nothing here
}

ProgramOptions::~ProgramOptions()
{
    // Nothing here
}

void ProgramOptions::add_option(const std::string& full_name, const std::string& short_name,
                                const std::string& option_description,
                                const OptionValueRef& option_value)
{
    const std::string qualified_full_name = (std::string)"--" + full_name;
    const std::string qualified_short_name = (std::string)"-" + short_name;

    if (find(qualified_full_name) != OptionValueRef::null_pointer) {
        throw ProgramOptionException((std::string)"ProgramOptions::add_option() : " +
                                     "Duplicate option \"" + qualified_full_name + "\" added.");
    }
    if (short_name != "-" && find(qualified_short_name) != OptionValueRef::null_pointer) {
        throw ProgramOptionException((std::string)"ProgramOptions::add_option() : " +
                                     "Duplicate option \"" + qualified_short_name + "\" added.");
    }

    auto actual_value = option_value;

    if (actual_value == OptionValueRef::null_pointer) {
        m_anon_option_values.push_back(0);
        auto val_ptr = static_cast<bool*>(static_cast<void*>(&(m_anon_option_values.back())));
        actual_value = make_value<bool>(val_ptr);
        actual_value->implicit_value("true")->default_value("false");
    }

    m_description_map[full_name] = detail::OptionDescription(full_name, short_name,
                                                             option_description);
    OptionMap::insert(qualified_full_name, actual_value);
    if (short_name != "") {
        OptionMap::insert(qualified_short_name, actual_value);
    }
}

// Option value cannot be empty for positional values
void ProgramOptions::add_positional_option(const std::string& full_name,
                                           const std::string& option_description,
                                           const OptionValueRef& option_value)
{
    u64 option_position = ++m_next_positional_option;

    if (find(full_name) != OptionValueRef::null_pointer) {
        throw ProgramOptionException((std::string)"ProgramOptions::add_positional_option() : " +
                                     "Duplicate option \"" + full_name + "\" added.");
    }
    std::string positional_name = gc_positional_option_prefix_ + std::to_string(option_position);

    m_description_map[full_name] = detail::OptionDescription(full_name, option_position,
                                                             option_description);

    OptionMap::insert((std::string)"--" + full_name, option_value);
    OptionMap::insert(positional_name, option_value);
    option_value->positional();
}

ProgramOptions::Iterator ProgramOptions::begin() const
{
    return OptionMap::cbegin();
}

ProgramOptions::Iterator ProgramOptions::end() const
{
    return OptionMap::cend();
}

void ProgramOptions::get_descriptions(std::ostream& out) const
{
    for (auto const& name_description : m_description_map) {
        out << name_description.second;
    }
}

std::string ProgramOptions::get_descriptions() const
{
    std::ostringstream out;
    get_descriptions(out);
    return out.str();
}

std::string ProgramOptions::get_description(const std::string& option_name) const
{
    auto it = m_description_map.find(option_name);
    if (it == m_description_map.end()) {
        return (std::string)"No description available for option \"" + option_name + "\"";
    } else {
        return it->second.to_string();
    }
}

std::string ProgramOptions::get_description(u64 option_position) const
{
    std::string search_string = gc_positional_option_prefix_ + std::to_string(option_position);
    auto it = m_description_map.find(search_string);
    if (it == m_description_map.end()) {
        return (std::string)"No description available for option at position " +
            std::to_string(option_position);
    } else {
        return it->second.to_string();
    }
}

inline void ProgramOptions::store_options_from_parse_map(const parsers::ParseMap& parse_map,
                                                         bool allow_unspecified_options)
{
    for (auto const& parse_entry : parse_map) {
        auto const& option_name = parse_entry.first;
        auto const& option_value = OptionMap::find(option_name);
        auto const& parsed_values = parse_entry.second;

        if (option_value == OptionValueRef::null_pointer) {
            if (!allow_unspecified_options) {
                throw ProgramOptionException((std::string)"Unknown option: \"" + option_name +
                                             "\" encountered while parsing options");
            }
            m_unspecified_options[option_name] = parsed_values;
            m_num_unspecified_options_parsed++;
            continue;
        }

        if (option_value->is_multitoken()) {
            try {
                option_value->store(parsed_values);
            } catch (const AurumException& e) {
                throw ProgramOptionException(e.get_exception_info() + "\nWhen parsing value " +
                                             "for option \"" + option_name + "\"");
            }
            m_num_specified_options_parsed++;
        } else {
            if (parsed_values.size() > 1) {
                throw ProgramOptionException((std::string)"Multiple values given for option \"" +
                                             option_name + "\" which cannot accept multiple " +
                                             "values.");
            }
            option_value->store(parsed_values);
            m_num_specified_options_parsed++;
        }
    }
}

void ProgramOptions::parse_command_line(int argc, char *argv[],
                                        bool allow_unspecified_options,
                                        bool process_escapes)
{
    parsers::ParseMap parse_map;
    parsers::parse_command_line(argc, argv, *this, parse_map, process_escapes);
    store_options_from_parse_map(parse_map, allow_unspecified_options);
}

void ProgramOptions::parse_config_file(const std::string& config_file_name,
                                       bool allow_unspecified_options,
                                       bool do_shell_expansion,
                                       bool process_escapes)
{
    parsers::ParseMap parse_map;
    parsers::parse_config_file(config_file_name, *this, parse_map,
                               do_shell_expansion, process_escapes);
    store_options_from_parse_map(parse_map, allow_unspecified_options);
}

void ProgramOptions::parse_option_string(const std::string& option_string,
                                         bool allow_unspecified_options,
                                         bool do_shell_expansion,
                                         bool process_escapes)
{
    parsers::ParseMap parse_map;
    parsers::parse_option_string(option_string, *this, parse_map,
                                 process_escapes, do_shell_expansion);
    store_options_from_parse_map(parse_map, allow_unspecified_options);
}

u64 ProgramOptions::get_num_specified_options_parsed() const
{
    return m_num_specified_options_parsed;
}

u64 ProgramOptions::get_num_unspecified_options_parsed() const
{
    return m_num_unspecified_options_parsed;
}

const ac::Vector<std::string>&
ProgramOptions::get_unspecified_option_value(const std::string& option_name) const
{
    auto it = m_unspecified_options.find(option_name);
    if (it == m_unspecified_options.end()) {
        return m_empty_string_vector;
    } else {
        return it->second;
    }
}

const ac::Vector<std::string>&
ProgramOptions::get_unspecified_option_value(u64 option_position) const
{
    return get_unspecified_option_value(gc_positional_option_prefix_ +
                                        std::to_string(option_position));
}

std::ostream& operator << (std::ostream& out, const ProgramOptions& prog_options)
{
    prog_options.get_descriptions(out);
    return out;
}

} /* end namespace program_options */
} /* end namespace aurum */

//
// ProgramOptions.cpp ends here
