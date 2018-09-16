/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
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

/* iBus IM Engine */

#include <ibus.h>
#include "ibus_keycode.h"
#include "Layout.h"

static IBusBus *bus = nullptr;
static IBusFactory *factory = nullptr;
static IBusEngine *engine = nullptr;
static IBusLookupTable *table = nullptr;
static gint id = 0;
static guint candidateSel = 0;

Suggestion suggestions;

void ibus_update_with_settings() {
  ibus_lookup_table_set_orientation(table, gLayout->isCandidateWinHorizontal() ? IBUS_ORIENTATION_HORIZONTAL
                                                                               : IBUS_ORIENTATION_VERTICAL);
}

void ibus_update_preedit() {
  if (suggestions.showCandidateWin) {
    ibus_lookup_table_set_cursor_pos(table, candidateSel);
    ibus_engine_update_lookup_table_fast(engine, table, TRUE);
  } else {
    candidateSel = 0; /* use the first candidate only */ }
  // Get current suggestion
  IBusText *txt = ibus_text_new_from_string((gchar *) suggestions.candidates[candidateSel].c_str());
  ibus_engine_update_preedit_text(engine, txt, ibus_text_get_length(txt), TRUE);
}

void ibus_table_sel_inc() {
  guint lastIndex = suggestions.candidates.size() - 1;
  if ((candidateSel + 1) > lastIndex) {
    candidateSel = -1;
  }
  ++candidateSel;
  ibus_update_preedit();
}

void ibus_table_sel_dec() {
  if (candidateSel == 0) {
    candidateSel = suggestions.candidates.size() - 1;
    ibus_update_preedit();
    return;
  } else {
    --candidateSel;
    ibus_update_preedit();
  }
}

void ibus_update_suggest(Suggestion suggest) {
  // Assign suggestions
  suggestions = suggest;
  if (suggestions.showCandidateWin) {
    // Update auxiliary text
    IBusText *caux = ibus_text_new_from_string((gchar *) suggestions.auxiliaryText.c_str());
    ibus_engine_update_auxiliary_text(engine, caux, TRUE);
    ibus_lookup_table_clear(table); // At first, remove all candidates
    for (auto &str : suggestions.candidates) {
      IBusText *ctext = ibus_text_new_from_string((gchar *) str.c_str());
      ibus_lookup_table_append_candidate(table, ctext);
      // Hide candidate labels // Hack learned from ibus-avro
      IBusText *clabel = ibus_text_new_from_string("");
      ibus_lookup_table_append_label(table, clabel);
    }
    // Previous selection
    candidateSel = (guint) suggestions.prevSelection;
    ibus_update_preedit();
  } else {
    candidateSel = 0;
    ibus_update_preedit();
  }
}

void ibus_reset() {
  // Reset all our mess
  suggestions = {};
  candidateSel = 0;
  ibus_lookup_table_clear(table);
  ibus_engine_hide_preedit_text(engine);
  ibus_engine_hide_auxiliary_text(engine);
  ibus_engine_hide_lookup_table(engine);
}

void ibus_commit() {
  if (!suggestions.isEmpty()) {
    std::string candidate = suggestions.candidates[candidateSel];
    IBusText *txt = ibus_text_new_from_string((gchar *) candidate.c_str());
    ibus_engine_commit_text(engine, txt);
    gLayout->candidateCommited(candidateSel);
  }
  ibus_reset();
  candidateSel = 0;
}

void ibus_disconnected_cb(IBusBus *bus, gpointer user_data) {
  ibus_quit();
  LOG_INFO("[IM:iBus]: Bus disconnected!\n");
}

gboolean ibus_process_key_event_cb(IBusEngine *engine,
                                   guint keyval,
                                   guint keycode,
                                   guint state) {
  // Set Defaults
  bool kshift, kctrl, kalt;
  kshift = false;
  kctrl = false;
  kalt = false;

  // Don't accept Key Release event
  if (state & IBUS_RELEASE_MASK)
    return FALSE;

  // Set modifiers
  if (state & IBUS_SHIFT_MASK)
    kshift = true;
  if (state & IBUS_CONTROL_MASK)
    kctrl = true;
  if (state & IBUS_MOD1_MASK)
    kalt = true;

  int key = ibus_keycode(keyval);

  // Update with settings
  ibus_update_with_settings();

  // Special Keys
  if ((key == VC_ENTER) || (key == VC_KP_ENTER) || (key == VC_BACKSPACE) || (key == VC_SPACE)) {
    IMCommand command = gLayout->handleSpecialKey(key);
    LOG_DEBUG("[IM:iBus]: Layout Management %s the event\n", (command.accepted) ? "accepted" : "rejected");
    if (command.commit) {
      // We have a request to Commit
      ibus_commit();
      return (gboolean) command.accepted;
    }
    // Usually it happens if the key is Backspace
    if (command.needUpdate && command.needReset) {
      ibus_update_suggest(gLayout->getCandidates());
      ibus_reset();
      return (gboolean) command.accepted;
    }
    if (command.needUpdate) {
      ibus_update_suggest(gLayout->getCandidates());
      return (gboolean) command.accepted;
    }
    // We have checked all the things and we have no other options to do rather than this
    return (gboolean) command.accepted;
  } else if ((key == VC_UP) || (key == VC_DOWN) || (key == VC_RIGHT) || (key == VC_LEFT) || (key == VC_TAB)) {
    // Cursor keys
    IMCommand command = gLayout->handleSpecialKey(key);
    LOG_DEBUG("[IM:iBus]: Layout Management %s the event\n", (command.accepted) ? "accepted" : "rejected");

    if (command.accepted) {
      if ((key == VC_DOWN) || (key == VC_RIGHT) || (key == VC_TAB)) {
        ibus_table_sel_inc();
        return TRUE;
      } else {
        ibus_table_sel_dec();
        return TRUE;
      }
    } else {
      ibus_commit();
      return FALSE;
    }
  }

  // Send key events to the Layout
  Suggestion sgg = gLayout->getSuggestion(key, kshift, kctrl, kalt);
  // If we have processed the key, update suggestions
  int ret = gLayout->handledKeyPress();
  if (ret) {
    ibus_update_suggest(sgg);
  } else {
    ibus_commit();
  }

  LOG_DEBUG("[IM:iBus]: Layout Management %s the event\n", ret ? "accepted" : "rejected");
  return (gboolean) ret;
}

void ibus_enable_cb(IBusEngine *engine) {
  LOG_INFO("[IM:iBus]: IM enabled\n");
  // Update Engine
  gLayout->updateEngine();
}

void ibus_disable_cb(IBusEngine *engine) {
  LOG_INFO("[IM:iBus]: IM disabled\n");
}

void ibus_focus_out_cb(IBusEngine *engine) {
  if (!suggestions.isEmpty()) {
    ibus_commit();
  }
}

void ibus_candidate_clicked_cb(IBusEngine *engine, guint index, guint button, guint state) {
  candidateSel = index;
  ibus_commit();
}

IBusEngine *ibus_create_engine_cb(IBusFactory *factory,
                                  gchar *engine_name,
                                  gpointer user_data) {
  id += 1;
  gchar *path = g_strdup_printf("/org/freedesktop/IBus/Engine/%i", id);
  engine = ibus_engine_new(engine_name,
                           path,
                           ibus_bus_get_connection(bus));

  // Setup Lookup table
  table = ibus_lookup_table_new(9, 0, TRUE, TRUE);
  g_object_ref_sink(table);
  ibus_update_with_settings();

  LOG_INFO("[IM:iBus]: Creating IM Engine\n");
  LOG_DEBUG("[IM:iBus]: Creating IM Engine with name:%s and id:%d\n", engine_name, id);

  g_signal_connect(engine, "process-key-event", G_CALLBACK(ibus_process_key_event_cb), NULL);
  g_signal_connect(engine, "enable", G_CALLBACK(ibus_enable_cb), NULL);
  g_signal_connect(engine, "disable", G_CALLBACK(ibus_disable_cb), NULL);
  g_signal_connect(engine, "focus-out", G_CALLBACK(ibus_focus_out_cb), NULL);
  g_signal_connect(engine, "candidate-clicked", G_CALLBACK(ibus_candidate_clicked_cb), NULL);

  return engine;
}

void ibus_start_setup(bool ibus) {
  IBusComponent *component;

  LOG_DEBUG("[IM:iBus]: Started IM facilities.\n");

  ibus_init();

  bus = ibus_bus_new();
  g_signal_connect(bus, "disconnected", G_CALLBACK(ibus_disconnected_cb), NULL);

  factory = ibus_factory_new(ibus_bus_get_connection(bus));
  g_signal_connect(factory, "create-engine", G_CALLBACK(ibus_create_engine_cb), NULL);

  if (ibus) {
    ibus_bus_request_name(bus, "org.freedesktop.IBus.OpenBangla", 0);
  } else {
    component = ibus_component_new("org.freedesktop.IBus.OpenBangla",
                                   "OpenBangla Keyboard",
                                   PROJECT_VERSION,
                                   "GPL 3",
                                   "See AboutBox",
                                   "http://openbangla.github.io/",
                                   PROJECT_DATADIR "/OpenBangla-Engine --ibus",
                                   "openbangla-keyboard"
    );

    ibus_component_add_engine(component,
                              ibus_engine_desc_new("OpenBangla",
                                                   "OpenBangla Keyboard",
                                                   "OpenBangla Keyboard IME for iBus",
                                                   "bn",
                                                   "GPL 3",
                                                   "See About Dialog",
                                                   PKGDATADIR "/icons/OpenBangla-Keyboard.png",
                                                   "default"
                              ));
    ibus_bus_register_component(bus, component);

    ibus_bus_set_global_engine_async(bus, "OpenBangla", -1, nullptr, nullptr, nullptr);
  }
  ibus_main();
}

int main(int argc, char *argv[]) {
  gLayout = new Layout();
  initKeycode();

  ibus_start_setup(argc > 1 && strcmp(argv[1], "--ibus") == 0);

  delete gLayout;

  return 0;
}
