// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/graph_endpoint.h>

#include <utility>

namespace piejam::audio::engine::endpoint_ports
{

template <std::size_t... Is>
using from_t = std::index_sequence<Is...>;

template <std::size_t... Is>
constexpr auto const from = from_t<Is...>{};

template <std::size_t... Is>
using to_t = std::index_sequence<Is...>;

template <std::size_t... Is>
constexpr auto const to = to_t<Is...>{};

constexpr std::index_sequence<0> const mono;
constexpr std::index_sequence<0, 1> const stereo;

} // namespace piejam::audio::engine::endpoint_ports
