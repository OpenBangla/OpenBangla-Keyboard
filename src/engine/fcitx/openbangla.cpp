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
#include "openbangla.h"
#include "keycode.h"
#include <fcitx-utils/log.h>
#include <fcitx-utils/misc.h>
#include <fcitx-utils/utf8.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputcontextmanager.h>
#include <fcitx/inputpanel.h>
#include <fcntl.h>
#include <filesystem>
#include <memory>

FCITX_DEFINE_LOG_CATEGORY(openbangla, "openbangla");
#define FCITX_OPENBANGLA_DEBUG() FCITX_LOGC(openbangla, Debug)

namespace fcitx {

class OpenBanglaState : public InputContextProperty {
public:
  OpenBanglaState(OpenBanglaEngine *engine, InputContext &ic)
      : engine_(engine), ic_(&ic),
        ctx_(riti_context_new_with_config(engine->ritiConfig())) {}

  ~OpenBanglaState() {}

  class OpenBanglaCandidate : public CandidateWord {
  public:
    OpenBanglaCandidate(OpenBanglaEngine *engine, std::string text, int index)
        : CandidateWord(Text(std::move(text))), engine_(engine), index_(index) {
    }

    void select(InputContext *inputContext) const override {
      auto state = inputContext->propertyFor(engine_->factory());
      state->selectCandidate(text().toString(), index_);
    }

  private:
    OpenBanglaEngine *engine_;
    int index_;
  };

  class OpenBanglaCandidateList : public CommonCandidateList {
  public:
    void setGlobalCursorIndex(int index) {
      int items = this->pageSize();
      int page = index / items;
      setPage(page);
      CommonCandidateList::setGlobalCursorIndex(index);
    }
  };

  void selectCandidate(const std::string &text, int index) {
    if (!suggestion_) {
      return;
    }

    char *txt = riti_suggestion_get_pre_edit_text(suggestion_.get(), index);
    ic_->commitString(std::string(txt));
    riti_context_candidate_committed(ctx_.get(), index);
    riti_string_free(txt);
    reset();
  }

  void updatePreedit() {
    if (!suggestion_) {
      return;
    }

    std::string text;
    auto index = 0;

    if (!riti_suggestion_is_lonely(suggestion_.get())) {
      auto candidateList = std::dynamic_pointer_cast<CommonCandidateList>(ic_->inputPanel().candidateList());
      auto idx = candidateList->globalCursorIndex();
      if (idx >= 0 && idx < candidateList->totalSize()) {
        index = idx;
      }
    }

    auto txt = riti_suggestion_get_pre_edit_text(suggestion_.get(), index);
    text = txt;
    riti_string_free(txt);

    Text preedit(std::move(text));
    preedit.setCursor(preedit.textLength());
    if (ic_->capabilityFlags().test(CapabilityFlag::Preedit)) {
      ic_->inputPanel().setClientPreedit(preedit);
      ic_->updatePreedit();
    }
    // we don't use input panel preedit here, because the aux text is actually
    // enough.
  }

  void commit() {
    if (!suggestion_) {
      return;
    }

    std::string text;
    if (!riti_suggestion_is_lonely(suggestion_.get())) {
      auto candidateList = ic_->inputPanel().candidateList();
      auto index = candidateList->cursorIndex();
      if (index >= 0 && index < candidateList->size()) {
        candidateList->candidate(index).select(ic_);
      }
    } else {
      char *txt = riti_suggestion_get_pre_edit_text(suggestion_.get(), 0);
      text = txt;
      ic_->commitString(text);
      riti_string_free(txt);
      riti_context_candidate_committed(ctx_.get(), 0);
      reset();
    }
  }
  void updateUI() {
    ic_->inputPanel().reset();
    do {
      if (!suggestion_) {
        break;
      }
      if (!riti_suggestion_is_lonely(suggestion_.get())) {
        char *aux = riti_suggestion_get_auxiliary_text(suggestion_.get());
        if (aux && aux[0]) {
          ic_->inputPanel().setAuxUp(Text(std::string(aux)));
        }
        riti_string_free(aux);

        auto candidateList = std::make_unique<OpenBanglaCandidateList>();
        auto len = riti_suggestion_get_length(suggestion_.get());
        for (decltype(len) i = 0; i < len; i++) {
          char *text = riti_suggestion_get_suggestion(suggestion_.get(), i);
          candidateList->append<OpenBanglaCandidate>(engine_, text, i);
          riti_string_free(text);
        }

        candidateList->setLayoutHint(engine_->candidateWinHorizontal()
                                         ? CandidateLayoutHint::Horizontal
                                         : CandidateLayoutHint::Vertical);
        auto index =
            riti_suggestion_previously_selected_index(suggestion_.get());
        if (index >= len) {
          index = 0;
        }
        candidateList->setGlobalCursorIndex(index);
        ic_->inputPanel().setCandidateList(std::move(candidateList));
      }
      updatePreedit();
    } while (0);
    ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
  }

  void reset() {
    altGrPressed_ = false;
    suggestion_.reset();
    if (riti_context_ongoing_input_session(ctx_.get())) {
      riti_context_finish_input_session(ctx_.get());
    }
    ic_->inputPanel().reset();
    ic_->updatePreedit();
    ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
  }

  void keyEvent(KeyEvent &keyEvent) {
    auto key = keyEvent.rawKey();
    if (keyEvent.isRelease()) {
      if (key.sym() == FcitxKey_Alt_R ||
          key.sym() == FcitxKey_ISO_Level3_Shift) {
        altGrPressed_ = false;
      }
      return;
    }

    auto ctx = ctx_.get();
    if (!riti_context_ongoing_input_session(ctx)) {
      engine_->reloadConfig();
      riti_context_update_engine(ctx, engine_->ritiConfig());
    }
    auto candidateList = ic_->inputPanel().candidateList();
    // At first, handle the special keys.
    switch (key.sym()) {
    case FcitxKey_BackSpace:
      if (riti_context_ongoing_input_session(ctx)) {
        // Is Ctrl key is pressed?
        auto ctrlKey = key.states().test(KeyState::Ctrl);
        suggestion_.reset(riti_context_backspace_event(ctx, ctrlKey));

        if (!riti_suggestion_is_empty(suggestion_.get())) {
          updateUI();
        } else {
          reset();
        }

        return keyEvent.filterAndAccept();
      }
      reset();
      return;
    case FcitxKey_Return:
      if (riti_context_ongoing_input_session(ctx)) {
        commit();
        if (engine_->enterKeyClosesPrevWin()) {
          keyEvent.filterAndAccept();
        }
      }
      return;
    case FcitxKey_space:
    case FcitxKey_KP_Enter:
      if (riti_context_ongoing_input_session(ctx)) {
        commit();
      }
      return;
    /** Arrow and Tab keys.
     * We use the arrow keys and the tab key to move the selection
     * in the preview window. So we have to ensure the preview
     * window is shown by checking if the current suggestion is
     * not a lonely one. Otherwise we don't handle it.
     **/
    case FcitxKey_Right:
    case FcitxKey_Left:
      if (riti_context_ongoing_input_session(ctx)) {
        if (engine_->candidateWinHorizontal() &&
            !riti_suggestion_is_lonely(suggestion_.get()) && candidateList &&
            candidateList->toCursorMovable()) {
          if (key.sym() == FcitxKey_Right) {
            candidateList->toCursorMovable()->nextCandidate();
          } else {
            candidateList->toCursorMovable()->prevCandidate();
          }
          updatePreedit();

          ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
          return keyEvent.filterAndAccept();
        } else {
          commit();
        }
      }
      return;
    case FcitxKey_Up:
    case FcitxKey_Down:
      if (riti_context_ongoing_input_session(ctx)) {
        if (!engine_->candidateWinHorizontal() &&
            !riti_suggestion_is_lonely(suggestion_.get()) && candidateList &&
            candidateList->toCursorMovable()) {
          if (key.sym() == FcitxKey_Up) {
            candidateList->toCursorMovable()->prevCandidate();
          } else {
            candidateList->toCursorMovable()->nextCandidate();
          }
          updatePreedit();
          ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
          keyEvent.filterAndAccept();
        } else {
          commit();
        }
      }
      return;
    case FcitxKey_Tab:
      if (riti_context_ongoing_input_session(ctx)) {
        if (!riti_suggestion_is_lonely(suggestion_.get()) && candidateList &&
            candidateList->toCursorMovable()) {
          candidateList->toCursorMovable()->nextCandidate();
          updatePreedit();
          ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
          keyEvent.filterAndAccept();
        } else {
          commit();
        }
      }
      return;
    /** Modifier keys **/
    case FcitxKey_Alt_R:
    case FcitxKey_ISO_Level3_Shift:
      // Keep track of the right Alt key (also known as the AltGr key)
      altGrPressed_ = true;
      [[fallthrough]];
    case FcitxKey_Shift_L:
    case FcitxKey_Shift_R:
    case FcitxKey_Control_L:
    case FcitxKey_Control_R:
    case FcitxKey_Alt_L:
    case FcitxKey_Meta_L:
    case FcitxKey_Meta_R:
      if (riti_context_ongoing_input_session(ctx)) {
        keyEvent.filterAndAccept();
      }
      return;
    default:
      break;
    }

    uint8_t modifier = 0;
    // Set modifiers
    if (key.states().test(KeyState::Shift)) {
      modifier |= MODIFIER_SHIFT;
    }

    auto ctrlKey = key.states().test(KeyState::Ctrl);
    auto altKey = key.states().test(KeyState::Alt);

    // Convert the key value into riti's key value.
    auto ritiKey = keySymToRitiKey(key.sym());

    // Reject the key which has only Ctrl or Alt (not the right one) combination
    // and riti doesn't handle.
    if ((ctrlKey && !altKey) || (!ctrlKey && altKey && !altGrPressed_) ||
        ritiKey == VC_UNKNOWN) {
      if (riti_context_ongoing_input_session(ctx)) {
        commit();
      }
      return;
    }

    // If we have Ctrl and Alt combination or the right Alt, set it as the AltGr
    // modifier.
    if ((ctrlKey && altKey) || altGrPressed_) {
      modifier |= MODIFIER_ALT_GR;
    }

    suggestion_.reset(riti_get_suggestion_for_key(ctx, ritiKey, modifier));

    if (!riti_suggestion_is_empty(suggestion_.get())) {
      updateUI();
      keyEvent.filterAndAccept();
      return;
    }

    // Corner case: When old style kar typing is enabled, a lonely suggestion and an empty
    // suggestion is not distinguishable. So we check if a input session is ongoing to
    // accept the key event.
    if(riti_context_ongoing_input_session(ctx)) {
      keyEvent.filterAndAccept();
    }
  }

private:
  OpenBanglaEngine *engine_;
  InputContext *ic_;
  /* Unfortunately, we have to keep track of the right Alt Key. */
  bool altGrPressed_ = false;
  UniqueCPtr<RitiContext, riti_context_free> ctx_;
  UniqueCPtr<Suggestion, riti_suggestion_free> suggestion_;
};

OpenBanglaEngine::OpenBanglaEngine(Instance *instance)
    : instance_(instance), cfg_(riti_config_new()),
      factory_(
          [this](InputContext &ic) { return new OpenBanglaState(this, ic); }) {
  if (!fs::makePath(stringutils::joinPath(
          StandardPath::global().userDirectory(StandardPath::Type::Data),
          "openbangla-keyboard"))) {
    throw std::runtime_error("Failed to create user directory");
  }
  reloadConfig();
  instance->inputContextManager().registerProperty("openbanglaState",
                                                   &factory_);
}

OpenBanglaEngine::~OpenBanglaEngine() {}

void OpenBanglaEngine::activate(const InputMethodEntry &, InputContextEvent &) {
  reloadConfig();
}

void OpenBanglaEngine::keyEvent(const InputMethodEntry &, KeyEvent &keyEvent) {
  auto ic = keyEvent.inputContext();
  auto state = ic->propertyFor(&factory_);
  state->keyEvent(keyEvent);
}

void OpenBanglaEngine::reset(const InputMethodEntry &,
                             InputContextEvent &event) {
  auto state = event.inputContext()->propertyFor(&factory_);
  state->reset();
}

bool booleanValue(const RawConfig &config, const std::string &path,
                  bool defaultValue) {
  bool value = defaultValue;
  if (auto *option = config.valueByPath(path)) {
    value = *option == "true";
  }
  return value;
}

void OpenBanglaEngine::populateConfig(const RawConfig &config) {
  // Keep sync with Settings.cpp
  std::string layoutPath = "avro_phonetic";
  if (auto *path = config.valueByPath("layout/path")) {
    layoutPath = *path;
  }
  const bool showCWPhonetic =
      booleanValue(config, "settings/CandidateWin\\Phonetic", true);
  const bool includeEnglish =
      booleanValue(config, "settings/PreviewWin\\IncludeEnglish", true);
  const bool showPrevWinFixed =
      booleanValue(config, "settings/FixedLayout\\ShowPrevWin", true);
  const bool autoVowelFormFixed =
      booleanValue(config, "settings/FixedLayout\\AutoVowelForm", true);
  const bool autoChandraPosFixed =
      booleanValue(config, "settings/FixedLayout\\AutoChandraPos", true);
  const bool traditionalKarFixed =
      booleanValue(config, "settings/FixedLayout\\TraditionalKar", false);
  const bool oldKarOrder =
      booleanValue(config, "settings/FixedLayout\\OldKarOrder", false);
  const bool oldReph =
      booleanValue(config, "settings/FixedLayout\\OldReph", true);
  const bool numberPadFixed =
      booleanValue(config, "settings/FixedLayout\\NumberPad", true);
  const bool ansiOutput =
      booleanValue(config, "settings/ANSI", false);
  const bool smartQuoting =
      booleanValue(config, "settings/SmartQuoting", true);

  riti_config_set_layout_file(cfg_.get(), layoutPath.data());
  riti_config_set_suggestion_include_english(cfg_.get(), includeEnglish);
  riti_config_set_phonetic_suggestion(cfg_.get(), showCWPhonetic);
  riti_config_set_database_dir(cfg_.get(),
                               "/usr/share/openbangla-keyboard/data");
  riti_config_set_fixed_suggestion(cfg_.get(), showPrevWinFixed);
  riti_config_set_fixed_auto_vowel(cfg_.get(), autoVowelFormFixed);
  riti_config_set_fixed_auto_chandra(cfg_.get(), autoChandraPosFixed);
  riti_config_set_fixed_traditional_kar(cfg_.get(), traditionalKarFixed);
  riti_config_set_fixed_old_kar_order(cfg_.get(), oldKarOrder);
  riti_config_set_fixed_old_reph(cfg_.get(), oldReph);
  riti_config_set_fixed_numpad(cfg_.get(), numberPadFixed);
  riti_config_set_ansi_encoding(cfg_.get(), ansiOutput);
  riti_config_set_smart_quote(cfg_.get(), smartQuoting);

  candidateWinHorizontal_ =
      booleanValue(config, "settings/CandidateWin\\Horizontal", true);
  enterKeyClosesPrevWin_ =
      booleanValue(config, "settings/EnterKeyClosesPrevWin", false);
}

void OpenBanglaEngine::reloadConfig() {
  auto path = StandardPath::global().locate(StandardPath::Type::Config,
                                            "OpenBangla/Keyboard.conf");
  auto time = std::filesystem::file_time_type::min();
  if (!path.empty()) {
    time = std::filesystem::last_write_time(path);
  }
  if (time < lastConfigTimestamp_) {
    return;
  }

  RawConfig config;
  auto configFile = StandardPath::global().open(
      StandardPath::Type::Config, "OpenBangla/Keyboard.conf", O_RDONLY);
  if (configFile.isValid()) {
    FCITX_OPENBANGLA_DEBUG() << "Reload openbangla configuration";
    lastConfigTimestamp_ = time;
    readFromIni(config, configFile.fd());
  }
  populateConfig(config);
}

FCITX_ADDON_FACTORY(OpenBanglaFactory);
} // namespace fcitx
