/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2012-2012 Yichao Yu
 *  Copyright (C) 2020-2021 CSSlayer <wengxt@gmail.com>
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
#include <fcitx-utils/i18n.h>
#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>
#include <limits>

namespace fcitx {

class OpenBanglaState;

FCITX_CONFIGURATION(OpenBanglaConfig,
                    ExternalOption config{this, "OpenBanglaKeyboard",
                                          _("OpenBangla Keyboard"),
                                          LIBEXECDIR "/openbangla-keyboard/openbangla-gui"};);

class OpenBanglaEngine final : public InputMethodEngine {
public:
  OpenBanglaEngine(Instance *instance);
  ~OpenBanglaEngine();

  void activate(const fcitx::InputMethodEntry &,
                fcitx::InputContextEvent &) override;
  void keyEvent(const InputMethodEntry &entry, KeyEvent &keyEvent) override;
  void reset(const InputMethodEntry &entry, InputContextEvent &event) override;

  auto *factory() { return &factory_; }
  void reloadConfig() override;

  const Configuration *getConfig() const override { return &config_; }

  auto getRitiConfig() const { return cfg_.get(); }

  auto candidateWinHorizontal() const { return candidateWinHorizontal_; }
  auto enterKeyClosesPrevWin() const { return enterKeyClosesPrevWin_; }

private:
  void populateConfig(const RawConfig &config);

  Instance *instance_;
  OpenBanglaConfig config_;
  UniqueCPtr<Config, riti_config_free> cfg_;
  FactoryFor<OpenBanglaState> factory_;
  int64_t lastConfigTimestamp_ = std::numeric_limits<int64_t>::min();
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
