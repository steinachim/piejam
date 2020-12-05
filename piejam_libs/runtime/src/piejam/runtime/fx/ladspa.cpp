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

#include <piejam/runtime/fx/ladspa.h>

#include <piejam/audio/ladspa/port_descriptor.h>
#include <piejam/functional/overload.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/parameter/map.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::fx
{

static auto
make_module_parameters(
        std::span<audio::ladspa::port_descriptor const> control_inputs,
        parameters_t& fx_params,
        float_parameters& float_params,
        int_parameters& /*int_params*/,
        bool_parameters& /*bool_params*/) -> fx::module_parameters
{
    fx::module_parameters module_params;

    for (auto const& port_desc : control_inputs)
    {
        if (auto const* const p = std::get_if<audio::ladspa::float_port>(
                    &port_desc.type_desc))
        {
            auto id = float_params.add(float_parameter{
                    .default_value = p->default_value,
                    .min = p->min,
                    .max = p->max,
                    .to_normalized = &runtime::parameter::to_normalized_linear,
                    .from_normalized =
                            &runtime::parameter::from_normalized_linear});

            fx_params.emplace(
                    id,
                    parameter{
                            .name = port_desc.name,
                            .unit = parameter_unit::none});

            module_params.emplace(port_desc.index, id);
        }
        else if (std::get_if<audio::ladspa::int_port>(&port_desc.type_desc))
        {
            BOOST_ASSERT_MSG(false, "int ports not implemented");
        }
        else if (std::get_if<audio::ladspa::bool_port>(&port_desc.type_desc))
        {
            BOOST_ASSERT_MSG(false, "bool ports not implemented");
        }
    }

    return module_params;
}

auto
make_ladspa_module(
        ladspa_instance_id instance_id,
        std::string const& name,
        std::span<audio::ladspa::port_descriptor const> control_inputs,
        parameters_t& fx_params,
        float_parameters& float_params,
        int_parameters& int_params,
        bool_parameters& bool_params) -> module
{
    return module{
            .fx_type_id = instance_id,
            .name = name,
            .parameters = make_module_parameters(
                    control_inputs,
                    fx_params,
                    float_params,
                    int_params,
                    bool_params)};
}

} // namespace piejam::runtime::fx
