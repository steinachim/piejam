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

#include <piejam/audio/ladspa/port_descriptor.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <span>
#include <string>

namespace piejam::runtime::actions
{

struct add_ladspa_fx_module final
    : ui::cloneable_action<add_ladspa_fx_module, action>
{
    fx::ladspa_instance_id instance_id;
    std::string name;
    std::span<audio::ladspa::port_descriptor const> control_inputs;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions