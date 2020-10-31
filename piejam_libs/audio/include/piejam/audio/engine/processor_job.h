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

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/period_sizes.h>

#include <array>
#include <functional>
#include <span>
#include <vector>

namespace piejam::audio::engine
{

class processor;
class thread_context;

class processor_job final
{
public:
    using output_buffer_t = std::array<float, max_period_size>;

    processor_job(processor& proc, std::size_t const& buffer_size_ref);

    auto result_ref(std::size_t index) const -> audio_slice const&;
    void connect_result(std::size_t index, audio_slice const& res);

    auto event_result_ref(std::size_t index) -> abstract_event_buffer const&;
    void connect_event_result(
            std::size_t index,
            abstract_event_buffer const& ev_buf);

    void clear_event_output_buffers();

    void operator()(thread_context const&);

private:
    processor& m_proc;
    std::size_t const& m_buffer_size;
    std::vector<output_buffer_t> m_output_buffers;

    std::vector<std::reference_wrapper<audio_slice const>> m_inputs;
    std::vector<std::span<float>> m_outputs;
    std::vector<audio_slice> m_results;

    event_input_buffers m_event_inputs;
    event_output_buffers m_event_outputs;

    process_context m_process_context;
};

} // namespace piejam::audio::engine