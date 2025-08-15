/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2012-2012 Yichao Yu
 *  Copyright (C) 2020-2025 CSSlayer <wengxt@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _FCITX5_OPENBANGLA_OPENBANGLA_H_
#define _FCITX5_OPENBANGLA_OPENBANGLA_H_

#include "riti.h"
#include <fcitx-config/configuration.h>
#include <fcitx-config/iniparser.h>
#include <fcitx-config/option.h>
#include <fcitx-config/rawconfig.h>
#include <fcitx-utils/i18n.h>
#include <fcitx-utils/misc.h>
#include <fcitx-utils/trackableobject.h>
#include <fcitx/addonfactory.h>
#include <fcitx/addoninstance.h>
#include <fcitx/addonmanager.h>
#include <fcitx/event.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>
#include <filesystem>

namespace fcitx {

class OpenBanglaState;

FCITX_CONFIGURATION(OpenBanglaConfig,
                    ExternalOption config{this, "OpenBanglaKeyboard",
                                          _("OpenBangla Keyboard"),
                                          BIN_DIR "/openbangla-gui"};);

class OpenBanglaEngine final : public InputMethodEngine {
public:
  OpenBanglaEngine(Instance *instance);
  ~OpenBanglaEngine();

  void activate(const fcitx::InputMethodEntry & /*entry*/,
                fcitx::InputContextEvent & /*event*/) override;
  void keyEvent(const InputMethodEntry &entry, KeyEvent &keyEvent) override;
  void reset(const InputMethodEntry &entry, InputContextEvent &event) override;

  auto *factory() { return &factory_; }
  void reloadConfig() override;

  const Configuration *getConfig() const override { return &config_; }

  auto candidateWinHorizontal() const { return candidateWinHorizontal_; }
  auto enterKeyClosesPrevWin() const { return enterKeyClosesPrevWin_; }

  RitiContext *context(InputContext *ic);

  auto *suggestion() const { return suggestion_.get(); }
  void setSuggestion(Suggestion *suggestion) { suggestion_.reset(suggestion); }

private:
  void populateConfig(const RawConfig &config);

  Instance *instance_;
  OpenBanglaConfig config_;
  UniqueCPtr<Config, riti_config_free> cfg_;
  UniqueCPtr<RitiContext, riti_context_free> ctx_;
  UniqueCPtr<Suggestion, riti_suggestion_free> suggestion_;
  TrackableObjectReference<InputContext> currentIC_;
  FactoryFor<OpenBanglaState> factory_;
  std::filesystem::file_time_type lastConfigTimestamp_ =
      std::filesystem::file_time_type::min();
  bool candidateWinHorizontal_ = true;
  bool enterKeyClosesPrevWin_ = false;
};

class OpenBanglaFactory : public AddonFactory {
public:
  AddonInstance *create(AddonManager *manager) override {
    return new OpenBanglaEngine(manager->instance());
  }
};

} // namespace fcitx

#endif // _FCITX5_OPENBANGLA_OPENBANGLA_H_
