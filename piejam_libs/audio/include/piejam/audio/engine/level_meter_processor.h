// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/fwd.h>

#include <memory>
#include <string_view>

namespace piejam::audio::engine
{

auto make_level_meter_processor(sample_rate, std::string_view name = {})
        -> std::unique_ptr<processor>;

} // namespace piejam::audio::engine
