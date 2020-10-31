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

#include <piejam/audio/engine/event.h>

#include <gtest/gtest.h>

namespace piejam::audio::engine::test
{

TEST(event, offset)
{
    event<float> ev(5, 23.f);
    EXPECT_EQ(5u, ev.offset());
}

TEST(event, value)
{
    event<float> ev(5, 23.f);
    EXPECT_EQ(23.f, ev.value());
}

} // namespace piejam::audio::engine::test