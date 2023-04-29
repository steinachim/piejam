// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <piejam/audio/types.h>

namespace piejam::runtime::modules::tool
{

enum class parameter_key : fx::parameter_key
{
    gain
};

auto make_module(fx_parameter_factory const&) -> fx::module;

} // namespace piejam::runtime::modules::tool
