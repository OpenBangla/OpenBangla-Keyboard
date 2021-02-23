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

#include <fcitx-config/configuration.h>
#include <fcitx-config/iniparser.h>
#include <fcitx-utils/i18n.h>
#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>

namespace fcitx {

class OpenBanglaState;

FCITX_CONFIGURATION(
    OpenBanglaConfig,
    Option<bool> enterKeyClosesPrevWin{this, "enterKeyClosesPrevWin",
                                       "enterKeyClosesPrevWin", false};
    Option<bool> candidateWinHorizontal{this, "CandidateWinHorizontal",
                                        "CandidateWinHorizontal", true};
    Option<std::string> layoutPath{
        this, "LayoutPath", "LayoutPath",
        "/usr/share/openbangla-keyboard/layouts/avrophonetic.json"};
    Option<bool> showCWPhonetic{this, "ShowCWPhonetic", "ShowCWPhonetic", true};
    Option<bool> includeEnglishPrevWin{this, "IncludeEnglishPrevWin",
                                       "IncludeEnglishPrevWin", true};
    Option<std::string> databasePath{this, "databasePath", "DatabasePath",
                                     "/usr/share/openbangla-keyboard/data"};
    Option<bool> showPrevWinFixed{this, "ShowPrevWinFixed", "ShowPrevWinFixed",
                                  true};
    Option<bool> autoVowelFormFixed{this, "AutoVowelFormFixed",
                                    "AutoVowelFormFixed", true};
    Option<bool> autoChandraPosFixed{this, "AutoChandraPosFixed",
                                     "AutoChandraPosFixed", true};
    Option<bool> traditionalKarFixed{this, "TraditionalKarFixed",
                                     "TraditionalKarFixed", false};
    Option<bool> oldReph{this, "OldReph", "OldReph", true};
    Option<bool> numberPadFixed{this, "NumberPadFixed", "NumberPadFixed",
                                true};);

class OpenBanglaEngine final : public InputMethodEngine {
public:
  OpenBanglaEngine(Instance *instance);
  ~OpenBanglaEngine();

  void keyEvent(const InputMethodEntry &entry, KeyEvent &keyEvent) override;
  void reset(const InputMethodEntry &entry, InputContextEvent &event) override;

  const auto &config() const { return config_; }
  auto *factory() { return &factory_; }
  void reloadConfig() override;

  const Configuration *getConfig() const override { return &config_; }
  void setConfig(const RawConfig &config) override {
    config_.load(config, true);
    safeSaveAsIni(config_, "conf/openbangla.conf");
    populateConfig();
  }

private:
  void populateConfig();

  Instance *instance_;
  OpenBanglaConfig config_;
  FactoryFor<OpenBanglaState> factory_;
};

class OpenBanglaFactory : public AddonFactory {
public:
  AddonInstance *create(AddonManager *manager) override {
    return new OpenBanglaEngine(manager->instance());
  }
};

} // namespace fcitx

#endif // _FCITX5_OPENBANGLA_OPENBANGLA_H_
