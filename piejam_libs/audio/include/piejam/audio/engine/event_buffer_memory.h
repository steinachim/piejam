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

#include <memory_resource>
#include <vector>

namespace piejam::audio::engine
{

class event_buffer_memory final
{
public:
    event_buffer_memory(std::size_t const initial_size)
        : m_memory(initial_size)
    {
    }

    auto memory_resource() noexcept -> std::pmr::memory_resource&
    {
        return m_memory_resource;
    }

    void release() { m_memory_resource.release(); }

private:
    std::vector<std::byte> m_memory;
    std::pmr::monotonic_buffer_resource m_memory_resource{
            m_memory.data(),
            m_memory.size()};
};

} // namespace piejam::audio::engine