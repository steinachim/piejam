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

#include <piejam/container/boxed_string.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/internal.h>

#include <boost/container/container_fwd.hpp>

#include <vector>

namespace piejam::runtime::fx
{

using parameter_key = std::size_t;
using module_parameters =
        boost::container::flat_map<parameter_key, parameter_id>;

struct module
{
    instance_id fx_instance_id;
    container::boxed_string name;
    container::box<module_parameters> parameters;
};

using module_id = entity_id<module>;
using modules_t = entity_map<module, module>;
using chain_t = std::vector<module_id>;

} // namespace piejam::runtime::fx
