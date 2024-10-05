// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/SessionList.h>

#include <piejam/log/generic_log_sink.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/selectors.h>

#include <fmt/chrono.h>

#include <spdlog/spdlog.h>

namespace piejam::gui::model
{

SessionList::SessionList(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
    addSession("ACST Test");
}

void
SessionList::onSubscribe()
{
}

} // namespace piejam::gui::model
