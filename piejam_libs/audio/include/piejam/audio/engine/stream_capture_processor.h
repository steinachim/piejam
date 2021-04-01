// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/named_processor.h>

#include <boost/lockfree/spsc_queue.hpp>

#include <string_view>

namespace piejam::audio::engine
{

class stream_capture_processor final : public named_processor
{
public:
    stream_capture_processor(
            std::size_t capacity,
            std::string_view const& name = {});

    auto type_name() const -> std::string_view override
    {
        return "stream_capture";
    }

    auto num_inputs() const -> std::size_t override { return 1; }
    auto num_outputs() const -> std::size_t override { return 1; }

    auto event_inputs() const -> event_ports override { return {}; }
    auto event_outputs() const -> event_ports override { return {}; }

    void process(process_context const& ctx) override;

    template <class F>
    void consume_all(F&& f)
    {
        m_buffer.consume_all(std::forward<F>(f));
    }

private:
    boost::lockfree::spsc_queue<float> m_buffer;
};

} // namespace piejam::audio::engine
