/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015 Muhammad Mominul Huque <nahidbinbaten1995@gmail.com>
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
#include <glib.h>
#include "im.h"
#include "ibus_keycode.h"
#include "layout.h"
#include "log.h"

static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;
static IBusEngine *engine = NULL;
static IBusLookupTable *table = NULL;
static gint id = 0;
static guint candidateSel = 0;

void ibus_disconnected_cb(IBusBus *bus, gpointer user_data) {
  ibus_quit();
  LOG_INFO("[IM:iBus]: Bus disconnected!");
}

gboolean ibus_process_key_event_cb(IBusEngine *engine,
                                   guint       keyval,
                                   guint       keycode,
                                   guint       state) {
  // Set Defaults
  bool kshift, kctrl, kalt;
  kshift = false;
  kctrl = false;
  kalt = false;

  // Don't accept Key Release event
  if (state & IBUS_RELEASE_MASK) return FALSE;

  // Set modifiers
  if(state & IBUS_SHIFT_MASK) kshift = true;
  if(state & IBUS_CONTROL_MASK) kctrl = true;
  if(state & IBUS_MOD1_MASK) kalt = true;

  // Send the key to layout management
  return (gboolean)gLayout->sendKey(ibus_keycode(keyval), kshift, kctrl, kalt);
}

void ibus_enable_cb(IBusEngine *engine) {
  LOG_INFO("[IM:iBus]: IM enabled");
}

void ibus_disable_cb(IBusEngine *engine) {
  LOG_INFO("[IM:iBus]: IM disabled");
}

IBusEngine* ibus_create_engine_cb(IBusFactory *factory,
                                  gchar* engine_name,
                                  gpointer     user_data) {
  id += 1;
  gchar *path = g_strdup_printf("/org/freedesktop/IBus/Engine/%i",id);
  engine = ibus_engine_new( engine_name,
                            path,
                            ibus_bus_get_connection(bus) );

  table = ibus_lookup_table_new (9, 0, TRUE, TRUE);

  LOG_INFO("[IM:iBus]: Creating IM Engine");
  LOG_DEBUG("[IM:iBus]: Creating IM Engine with name:%s and id:%i",(char*)engine_name, id);

  g_signal_connect(engine, "process-key-event", G_CALLBACK(ibus_process_key_event_cb), NULL);
  g_signal_connect(engine, "enable", G_CALLBACK(ibus_enable_cb), NULL);
  g_signal_connect(engine, "disable", G_CALLBACK(ibus_disable_cb), NULL);

  return engine;
}

void start_setup(bool ibus) {
  IBusComponent *component;
  IBusEngineDesc *engine_dec;

  ibus_init();

  bus = ibus_bus_new();
  g_signal_connect(bus, "disconnected", G_CALLBACK (ibus_disconnected_cb), NULL);

  factory = ibus_factory_new(ibus_bus_get_connection(bus));
  g_signal_connect(factory, "create-engine", G_CALLBACK(ibus_create_engine_cb), NULL);

  if(ibus) {
    ibus_bus_request_name(bus, "org.freedesktop.IBus.OpenBangla", 0);
  } else {
    component = ibus_component_new( "org.freedesktop.IBus.OpenBangla",
                                    "OpenBangla Keyboard",
                                    "1.0",
                                    "GPL 3",
                                    "See AboutBox",
                                    "https://github.com/OpenBangla/OpenBangla-Keyboard",
                                    "/home/mominul/experiment/OpenBnagla-Keyboard",
                                    "openbangla-keyboard"
                                  );

    engine_dec = ibus_engine_desc_new( "OpenBangla",
                                       "OpenBangla Keyboard",
                                       "OpenBangla Keyboard IME for iBus",
                                       "bn",
                                       "GPL 3",
                                       "See AboutBox",
                                       "/home/mominul/experiment/avro-bangla.png",
                                       "us"
                                     );
    ibus_component_add_engine(component, engine_dec);
    ibus_bus_register_component(bus, component);
  }
  ibus_main();
}

void ibus_update_preedit() {
  ibus_lookup_table_set_cursor_pos(table, candidateSel);
  ibus_engine_update_lookup_table_fast(engine, table, TRUE);
  // Get current suggestion
  IBusText *txt = ibus_lookup_table_get_candidate(table, candidateSel);
  ibus_engine_update_preedit_text(engine, txt, ibus_text_get_length(txt), TRUE);
}

void im_table_sel_inc() {
  guint lastIndex = ibus_lookup_table_get_number_of_candidates(table) - 1;
  if((candidateSel + 1) > lastIndex) { candidateSel -= 1; }
  ++candidateSel;
  ibus_update_preedit();
}

void im_table_sel_dec() {
  if((candidateSel - 1) < 0) {
    candidateSel = ibus_lookup_table_get_number_of_candidates(table);
  }
  --candidateSel;
  ibus_update_preedit();
}

void im_update_suggest(std::vector<std::string> lst, std::string typed) {
  // Update auxiliary text
  IBusText *caux = ibus_text_new_from_string((gchar*)typed.c_str());
  ibus_engine_update_auxiliary_text(engine, caux, TRUE);
  ibus_lookup_table_clear(table); // At first, remove all candidates
  for(auto& str : lst) {
    IBusText *ctext = ibus_text_new_from_string((gchar*)str.c_str());
    ibus_lookup_table_append_candidate(table, ctext);
    // Hide candidate labels // Hack learned from ibus-avro
    IBusText *clabel = ibus_text_new_from_string("");
    ibus_lookup_table_append_label(table, clabel);
  }
  ibus_update_preedit();
}

void im_reset() {
  // Reset all our mess
  candidateSel = 0;
  ibus_lookup_table_clear(table);
  ibus_engine_hide_preedit_text(engine);
  ibus_engine_hide_auxiliary_text(engine);
  ibus_engine_hide_lookup_table(engine);
}

void im_commit() {
  IBusText *txt = ibus_lookup_table_get_candidate(table, candidateSel);
  ibus_engine_commit_text(engine,txt);
  LOG_DEBUG("[IM:iBus]: String commited: %s",(char*)ibus_text_get_text(txt));
  im_reset();
}

void im_start(bool executed) {
  start_setup(executed);
}
