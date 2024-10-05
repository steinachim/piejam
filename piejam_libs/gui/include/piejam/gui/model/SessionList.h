// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>

#include <QList>
#include <QStringList>

#include <memory>
#include <string>

namespace piejam::gui::model
{

class SessionList final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QStringList sessionList READ sessionList NOTIFY
                       sessionListChanged FINAL)

public:
    SessionList(runtime::store_dispatch, runtime::subscriber&);

    auto sessionList() const -> QStringList
    {
        return m_sessionList;
    }

    void addSession(QString const& session)
    {
        m_sessionList.push_back(session);
        emit sessionListChanged();
    }

signals:

    void sessionListChanged();

private:
    void onSubscribe() override;

    QStringList m_sessionList;
};

} // namespace piejam::gui::model
