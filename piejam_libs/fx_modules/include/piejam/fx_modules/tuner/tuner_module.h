// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/internal_fx_module_factory.h>

#include <piejam/audio/types.h>

namespace piejam::fx_modules::tuner
{

enum class stereo_channel : int
{
    left,
    right,
    middle,

    _min = left,
    _max = middle,
};

enum class parameter_key : runtime::fx::parameter_key
{
    channel,
};

enum class stream_key : runtime::fx::stream_key
{
    input
};

auto make_module(runtime::internal_fx_module_factory_args const&)
        -> runtime::fx::module;

} // namespace piejam::fx_modules::tuner