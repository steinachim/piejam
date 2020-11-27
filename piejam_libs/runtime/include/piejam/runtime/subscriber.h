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

#include <piejam/entity_id_hash.h>
#include <piejam/reselect/fwd.h>
#include <piejam/runtime/fwd.h>

#include <functional>

namespace piejam::runtime
{

using subscriber = reselect::subscriber<audio_state>;

template <class Value>
using selector = reselect::selector<Value, audio_state>;

using subscription_id = entity_id<struct subs>;
using subscriptions_manager = reselect::subscriptions_manager<subscription_id>;

} // namespace piejam::runtime