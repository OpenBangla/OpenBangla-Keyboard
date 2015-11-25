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
#include <glib/gprintf.h>
#include "im/im.h"
#include "im/ibus/ibus_keycode.h"
#include "layout/layout.h"
#include "log.h"

static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;
static IBusEngine *engine = NULL;
static int id = 0;

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

  // Force Shift
  if(keyval >= IBUS_KEY_A && keyval <= IBUS_KEY_Z) kshift = true;

  // At last, send the key
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
  engine = ibus_engine_new( engine_name,
                            g_strdup_printf("/org/freedesktop/IBus/Engine/%i",id),
                            ibus_bus_get_connection(bus) );

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
                                    "",
                                    ""
                                  );

    engine_dec = ibus_engine_desc_new( "OpenBangla",
                                       "OpenBangla Keyboard",
                                       "OpenBangla Keyboard IME for iBus",
                                       "bn",
                                       "GPL 3",
                                       "See AboutBox",
                                       "",
                                       "bn"
                                     );
    ibus_component_add_engine(component, engine_dec);
    ibus_bus_register_component(bus, component);
  }
  ibus_main();
}

void im_commit(std::string text) {
  IBusText * txt = ibus_text_new_from_string((gchar *)text.c_str());
  ibus_engine_commit_text(engine,txt);
  LOG_DEBUG("[IM:iBus]: String commited: %s",text.c_str());
}

void im_start(bool executed) {
  start_setup(executed);
}
