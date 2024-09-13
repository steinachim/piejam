// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/external_audio_device_actions.h>

#include <piejam/runtime/state.h>

#include <piejam/algorithm/contains.h>

#include <fmt/format.h>

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::runtime::actions
{

namespace
{

auto
default_bus_name(state const& st, io_direction io_dir, audio::bus_type bus_type)
{
    switch (io_dir)
    {
        case io_direction::input:
            return fmt::format(
                    "In {} {}",
                    st.device_io_state.inputs->size() + 1,
                    bus_type == audio::bus_type::mono ? "M" : "S");

        case io_direction::output:
            return fmt::format(
                    "Speaker {}",
                    st.device_io_state.outputs->size() + 1);
    }
}

auto
default_channels(state const& st, io_direction io_dir, audio::bus_type bus_type)
{
    auto num_channels =
            io_dir == io_direction::input
                    ? st.selected_io_sound_card.in.hw_params->num_channels
                    : st.selected_io_sound_card.out.hw_params->num_channels;

    std::vector<bool> assigned_channels(num_channels);

    auto const& device_ids = io_dir == io_direction::input
                                     ? *st.device_io_state.inputs
                                     : *st.device_io_state.outputs;

    for (external_audio::device_id const device_id : device_ids)
    {
        external_audio::device const* const device =
                st.device_io_state.devices.find(device_id);
        BOOST_ASSERT(device);
        if (auto ch = device->channels.left; ch != npos)
        {
            assigned_channels[ch] = true;
        }

        if (auto ch = device->channels.right; ch != npos)
        {
            assigned_channels[ch] = true;
        }
    }

    auto it = std::ranges::find(assigned_channels, false);
    std::size_t ch = it != assigned_channels.end()
                             ? std::distance(assigned_channels.begin(), it)
                             : npos;

    channel_index_pair channels{ch};

    if (bus_type == audio::bus_type::stereo)
    {
        if (it != assigned_channels.end())
        {
            it = std::find(std::next(it), assigned_channels.end(), false);
        }

        channels.right = it != assigned_channels.end()
                                 ? std::distance(assigned_channels.begin(), it)
                                 : npos;
    }

    return channels;
}

} // namespace

void
add_external_audio_device::reduce(state& st) const
{
    BOOST_ASSERT(
            direction != io_direction::output ||
            type == audio::bus_type::stereo);

    auto added_device_id = runtime::add_external_audio_device(
            st,
            default_bus_name(st, direction, type),
            direction,
            type,
            default_channels(st, direction, type));

    if (direction == io_direction::output)
    {
        st.mixer_state.channels.update(
                st.mixer_state.main,
                [added_device_id](mixer::channel& mixer_channel) {
                    if (std::holds_alternative<default_t>(mixer_channel.out))
                    {
                        mixer_channel.out = added_device_id;
                    }
                });
    }
}

void
remove_external_audio_device::reduce(state& st) const
{
    runtime::remove_external_audio_device(st, device_id);
}

void
set_external_audio_device_bus_channel::reduce(state& st) const
{
    st.device_io_state.devices.update(
            device_id,
            [this](external_audio::device& device) {
                switch (channel_selector)
                {
                    case audio::bus_channel::mono:
                        device.channels = channel_index_pair{channel_index};
                        break;

                    case audio::bus_channel::left:
                        device.channels.left = channel_index;
                        break;

                    case audio::bus_channel::right:
                        device.channels.right = channel_index;
                        break;
                }
            });
}

void
set_external_audio_device_name::reduce(state& st) const
{
    st.device_io_state.devices.update(
            device_id,
            [this](external_audio::device& device) { device.name = name; });

    auto const io_dir =
            algorithm::contains(*st.device_io_state.inputs, device_id)
                    ? io_direction::input
                    : io_direction::output;

    st.mixer_state.channels.update(
            [this,
             io_dir,
             route_name = mixer::io_address_t(mixer::missing_device_address(
                     name))](mixer::channel_id, mixer::channel& channel) {
                auto& io_addr = io_dir == io_direction::input ? channel.in
                                                              : channel.out;

                if (io_addr == route_name)
                {
                    io_addr = device_id;
                }
            });
}

} // namespace piejam::runtime::actions