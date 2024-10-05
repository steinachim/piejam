// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/thread/priority.h>

#include <spdlog/spdlog.h>

#include <pthread.h>
#include <sched.h>

#include <sstream>
#include <system_error>

namespace piejam::this_thread
{

void
set_realtime_priority(int prio)
{
    sched_param const parm{.sched_priority = prio};
    int const status = pthread_setschedparam(pthread_self(), SCHED_FIFO, &parm);
    if (status != 0)
    {
        std::ostringstream error_message;
        error_message << "set_realtime_priority: unable to set thread priority to " << prio << " (status = " << status << ")";
        spdlog::error(error_message.str());
    }
}

} // namespace piejam::this_thread
