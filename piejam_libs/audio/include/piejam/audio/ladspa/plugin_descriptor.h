// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/audio/ladspa/fwd.h>

#include <filesystem>
#include <string>
#include <vector>

namespace piejam::audio::ladspa
{

struct plugin_descriptor
{
    std::filesystem::path file;
    unsigned long index{};
    plugin_id_t id{};
    std::string label;
    std::string name;
    std::string author;
    std::string copyright;
    std::size_t num_inputs{};
    std::size_t num_outputs{};
};

inline bool
operator==(plugin_descriptor const& l, plugin_descriptor const& r)
{
    return l.id == r.id;
}

inline bool
operator!=(plugin_descriptor const& l, plugin_descriptor const& r)
{
    return l.id != r.id;
}

} // namespace piejam::audio::ladspa
