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

#include <ostream>

#include "../basetypes/AurumTypes.hpp"
#include "../containers/UnorderedMap.hpp"
#include "../containers/OrderedMap.hpp"

#include "OptionMap.hpp"

namespace aurum {
namespace program_options {

namespace ac = aurum::containers;

// forward declaration
namespace parsers {

typedef ac::UnifiedUnorderedMap<std::string, ac::Vector<std::string> > ParseMap;

} /* end namespace parsers */

extern const std::string gc_positional_option_prefix_;

namespace detail_ {

class OptionDescription : public AurumObject<OptionDescription>,
                          public Stringifiable<OptionDescription>
{
public:
    std::string m_full_name;
    std::string m_short_name;
    std::string m_description;
    bool m_positional;
    u64 m_position;

    OptionDescription();
    OptionDescription(const OptionDescription& other);
    OptionDescription(OptionDescription&& other);
    OptionDescription(const std::string& full_name, const std::string& short_name,
                      const std::string& description);
    OptionDescription(const std::string& full_name, u64 position,
                      const std::string& description);

    OptionDescription& operator = (const OptionDescription& other);
    OptionDescription& operator = (OptionDescription&& other);

    virtual ~OptionDescription();

    std::string to_string() const;
    std::string to_string(u32 verbosity) const;
};

extern std::ostream& operator << (std::ostream& out,
                                  const OptionDescription& option_description);

} /* end namespace detail_ */

class ProgramOptions : private OptionMap
{
private:
    typedef ac::OrderedMap<std::string, detail_::OptionDescription> DescriptionMap;
    DescriptionMap m_description_map;
    ac::Vector<u32> m_anon_option_values;
    u64 m_next_positional_option;
    u64 m_num_specified_options_parsed;
    u64 m_num_unspecified_options_parsed;
    ac::UnifiedUnorderedMap<std::string, ac::Vector<std::string> > m_unspecified_options;
    const ac::Vector<std::string> m_empty_string_vector;

    // helper functions
    inline void store_options_from_parse_map(const parsers::ParseMap& parse_map,
                                             bool allow_unspecified_options);

public:
    typedef OptionMap::ConstIterator Iterator;

    ProgramOptions();
    ~ProgramOptions();

    void add_option(const std::string& full_name, const std::string& short_name,
                    const std::string& option_description,
                    const OptionValueRef& option_value = OptionValueRef::null_pointer);

    void add_positional_option(const std::string& full_name,
                               const std::string& option_description,
                               const OptionValueRef& option_value);

    using OptionMap::operator[];
    using OptionMap::find;

    Iterator begin() const;
    Iterator end() const;

    void parse_command_line(int argc, char* argv[],
                            bool allow_unspecified_options = true,
                            bool process_escapes = true);
    void parse_config_file(const std::string& config_file_name,
                           bool allow_unspecified_options = true,
                           bool do_shell_expansion = true,
                           bool process_escapes = true);
    void parse_option_string(const std::string& option_string,
                             bool allow_unspecified_options = true,
                             bool do_shell_expansion = true,
                             bool process_escapes = true);

    void get_descriptions(std::ostream& out) const;
    std::string get_descriptions() const;

    std::string get_description(const std::string& option_name) const;
    std::string get_description(u64 option_position) const;

    u64 get_num_specified_options_parsed() const;

    u64 get_num_unspecified_options_parsed() const;
    const ac::Vector<std::string>&
    get_unspecified_option_value(const std::string& option_name) const;
    const ac::Vector<std::string>&
    get_unspecified_option_value(u64 option_position) const;
};

extern std::ostream& operator << (std::ostream& out, const ProgramOptions& prog_options);

} /* end namespace program_options */
} /* end namespace aurum */

#endif /* AURUM_PROGOPTIONS_PROGRAM_OPTIONS_HPP_ */

//
// ProgramOptions.hpp ends here
