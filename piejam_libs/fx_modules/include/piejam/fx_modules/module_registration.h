// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/component.h>
#include <piejam/gui/model/FxModuleContentFactory.h>
#include <piejam/runtime/components/make_fx.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/modules/factory.h>

#include <QString>

#include <functional>
#include <memory>
#include <string>

namespace piejam::fx_modules
{

struct module_registration
{
    std::string persistence_name;

    runtime::modules::internal_fx_module_factory fx_module_factory;
    runtime::components::internal_fx_component_factory fx_component_factory;

    QString fx_browser_entry_name;
    QString fx_browser_entry_description;

    gui::model::FxModuleContentFactory fx_module_content_factory;

    QString viewSource;
};

auto register_module(module_registration) -> runtime::fx::internal_id;

} // namespace piejam::fx_modules
