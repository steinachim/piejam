// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxGenericModule.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/FxBoolParameter.h>
#include <piejam/gui/model/FxFloatParameter.h>
#include <piejam/gui/model/FxIntParameter.h>
#include <piejam/gui/model/FxParameterKeyId.h>
#include <piejam/runtime/selectors.h>

#include <boost/container/flat_map.hpp>
#include <boost/hof/construct.hpp>
#include <boost/hof/match.hpp>
#include <boost/hof/unpack.hpp>

namespace piejam::gui::model
{

namespace
{

auto
fxParameterKeyIds(runtime::fx::module_parameters const& params)
{
    return algorithm::transform_to_vector(
            params,
            boost::hof::unpack(boost::hof::construct<FxParameterKeyId>()));
}

} // namespace

struct FxGenericModule::Impl
{
    runtime::fx::module_id fx_mod_id;

    box<runtime::fx::module_parameters> param_ids;
    FxParametersList parameters;
};

FxGenericModule::FxGenericModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_mod_id))
{
}

FxGenericModule::~FxGenericModule() = default;

auto
FxGenericModule::parameters() noexcept -> FxParametersList*
{
    return &m_impl->parameters;
}

auto
FxGenericModule::parameters() const noexcept -> FxParametersList const*
{
    return &m_impl->parameters;
}

void
FxGenericModule::onSubscribe()
{
    observe(runtime::selectors::make_fx_module_parameters_selector(
                    m_impl->fx_mod_id),
            [this](box<runtime::fx::module_parameters> const& paramIds) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                fxParameterKeyIds(*m_impl->param_ids),
                                fxParameterKeyIds(*paramIds)),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *parameters(),
                                [this](FxParameterKeyId const& paramKeyId) {
                                    return makeFxParameter(
                                            dispatch(),
                                            state_change_subscriber(),
                                            paramKeyId);
                                }});

                m_impl->param_ids = paramIds;
            });
}

} // namespace piejam::gui::model
