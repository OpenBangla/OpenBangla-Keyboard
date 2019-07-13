#include <ibus.h>
#include <QDebug>
#include "riti.h"
#include "keycode.h"
#include "FileSystem.h"
#include "Settings.h"
#include "Log.h"

static RitiContext *ctx = nullptr;
static IBusBus *bus = nullptr;
static IBusEngine *engine = nullptr;
static IBusLookupTable *table = nullptr;
static gint id = 0;
static bool input_session_ongoing = false;
static IBusText *preeditText = nullptr;
static uintptr_t table_pos = 0; 

void update_with_settings() {
    qputenv("RITI_LAYOUT_FILE", gSettings->getLayoutPath().toLatin1());
    qputenv("RITI_PHONETIC_DATABASE_DIR", DatabasePath().toLatin1());
    qputenv("RITI_LAYOUT_FIXED_VOWEL", gSettings->getAutoVowelFormFixed() ? "true" : "false");
    qputenv("RITI_LAYOUT_FIXED_CHANDRA", gSettings->getAutoChandraPosFixed() ? "true" : "false");
    qputenv("RITI_LAYOUT_FIXED_KAR", gSettings->getTraditionalKarFixed() ? "true" : "false");

    if(table != nullptr) {
      ibus_lookup_table_set_orientation(table, gSettings->getCandidateWinHorizontal() ? IBUS_ORIENTATION_HORIZONTAL : IBUS_ORIENTATION_VERTICAL);
    }
}

void engine_update_preedit(Suggestion *suggestion) {
  if (!riti_suggestion_is_lonely(suggestion)) {
    ibus_engine_update_lookup_table_fast(engine, table, TRUE);
    table_pos = ibus_lookup_table_get_cursor_pos(table);
    preeditText = ibus_lookup_table_get_candidate(table, table_pos);
  } else {
    preeditText = ibus_text_new_from_string(riti_suggestion_get_lonely_suggestion(suggestion));
    table_pos = 0;
  }

  g_object_ref_sink(preeditText);

  ibus_engine_update_preedit_text(engine, preeditText, ibus_text_get_length(preeditText), TRUE);
}

void engine_update_lookup_table(Suggestion *suggestion) {
  if (!riti_suggestion_is_lonely(suggestion)) {
    char *aux = riti_suggestion_get_auxiliary_text(suggestion);
    IBusText *auxiliary = ibus_text_new_from_string(aux);

    ibus_lookup_table_clear(table);
    ibus_engine_update_auxiliary_text(engine, auxiliary, TRUE);

    char *const *suggestions = riti_suggestion_get_suggestions(suggestion);
    uintptr_t len = riti_suggestion_get_length(suggestion);

    for(uintptr_t i = 0; i < len; i++) {
      IBusText *text = ibus_text_new_from_string(suggestions[i]);
      ibus_lookup_table_append_candidate(table, text);
    }

    ibus_lookup_table_set_cursor_pos(table, 0);
  }

  input_session_ongoing = true;
  engine_update_preedit(suggestion);
}

void engine_reset() {
  input_session_ongoing = false;
  ibus_lookup_table_clear(table);
  ibus_engine_hide_preedit_text(engine);
  ibus_engine_hide_auxiliary_text(engine);
  ibus_engine_hide_lookup_table(engine);
}

void engine_commit() {
  ibus_engine_commit_text(engine, preeditText);
  riti_context_candidate_committed(ctx, table_pos);
  g_object_unref(preeditText);
  engine_reset();
}

/************************************* Callbacks *************************************/

gboolean engine_process_key_event_cb(IBusEngine *engine,
                                   guint keyval,
                                   guint keycode,
                                   guint state) {
  u_int8_t modifier = 0;

  // Don't accept Key Release event
  if (state & IBUS_RELEASE_MASK)
    return FALSE;

  // Set modifiers
  if (state & IBUS_SHIFT_MASK)
    modifier |= MODIFIER_SHIFT;
  if (state & IBUS_CONTROL_MASK)
    modifier |= MODIFIER_CTRL;
  if (state & IBUS_MOD1_MASK)
    modifier |= MODIFIER_ALT;

  int key = ibus_keycode(keyval);

  Suggestion *suggestion = riti_get_suggestion_for_key(ctx, key, modifier);

  bool ret = riti_context_key_handled(ctx);
  LOG_DEBUG("[IM:iBus]: Layout Management %s the event\n", ret ? "accepted" : "rejected");

  if(ret) {
    if(key == VC_BACKSPACE || key == VC_RIGHT || key == VC_LEFT) {
      switch (key) {
        // We have to care specially when the key is Backspace! :)
        case VC_BACKSPACE:
          if(!riti_suggestion_is_empty(suggestion)) {
            engine_update_lookup_table(suggestion);
          } else {
            engine_reset();
          } 
          break;
        case VC_RIGHT:
          ibus_lookup_table_cursor_down(table);
          engine_update_preedit(suggestion);
          break;
        case VC_LEFT:
          ibus_lookup_table_cursor_up(table);
          engine_update_preedit(suggestion);
          break;
      }
    } else {
      engine_update_lookup_table(suggestion);
    }
  } else {
    if(input_session_ongoing) {
      engine_commit();
    } else {
      engine_reset();
    }
  }

  riti_suggestion_free(suggestion);
  return (gboolean) ret;
}

void engine_enable_cb(IBusEngine *engine) {
  LOG_INFO("[IM:iBus]: IM enabled\n");
  update_with_settings();
}

void engine_disable_cb(IBusEngine *engine) {
  LOG_INFO("[IM:iBus]: IM disabled\n");
}

void engine_focus_out_cb(IBusEngine *engine) {
  LOG_INFO("[IM:iBus]: IM Focus out\n");

  if(input_session_ongoing) {
    engine_commit();
  }

  update_with_settings();
  riti_context_update_engine(ctx);
}

IBusEngine *create_engine_cb(IBusFactory *factory,
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

  LOG_INFO("[IM:iBus]: Creating IM Engine\n");
  LOG_DEBUG("[IM:iBus]: Creating IM Engine with name:%s and id:%d\n", engine_name, id);

  g_signal_connect(engine, "process-key-event", G_CALLBACK(engine_process_key_event_cb), NULL);
  g_signal_connect(engine, "enable", G_CALLBACK(engine_enable_cb), NULL);
  g_signal_connect(engine, "disable", G_CALLBACK(engine_disable_cb), NULL);
  g_signal_connect(engine, "focus-out", G_CALLBACK(engine_focus_out_cb), NULL);
  //g_signal_connect(engine, "candidate-clicked", G_CALLBACK(ibus_candidate_clicked_cb), NULL);

  return engine;
}

void engine_disconnected_cb(IBusBus *bus, gpointer user_data) {
  ibus_quit();
  LOG_INFO("[IM:iBus]: Bus disconnected!\n");
}

void start_setup(bool ibus) {
  IBusComponent *component;

  LOG_DEBUG("[IM:iBus]: Started IM facilities.\n");

  ibus_init();

  bus = ibus_bus_new();
  g_signal_connect(bus, "disconnected", G_CALLBACK(engine_disconnected_cb), NULL);

  IBusFactory *factory = ibus_factory_new(ibus_bus_get_connection(bus));
  g_signal_connect(factory, "create-engine", G_CALLBACK(create_engine_cb), NULL);

  if (ibus) {
    ibus_bus_request_name(bus, "org.freedesktop.IBus.OpenBangla", 0);
  } else {
    component = ibus_component_new("org.freedesktop.IBus.OpenBangla",
                                   "OpenBangla Keyboard",
                                   PROJECT_VERSION,
                                   "GPL 3",
                                   "See AboutBox",
                                   "http://openbangla.github.io/",
                                   PROJECT_DATADIR "/ibus-openbangla --ibus",
                                   "openbangla-keyboard"
    );

    ibus_component_add_engine(component,
                              ibus_engine_desc_new("OpenBangla",
                                                   "OpenBangla Keyboard",
                                                   "OpenBangla Keyboard IME for iBus",
                                                   "bn",
                                                   "GPL 3",
                                                   "See About Dialog",
                                                   PROJECT_DATADIR "/icons/OpenBangla-Keyboard.png",
                                                   "default"
                              ));
    ibus_bus_register_component(bus, component);

    ibus_bus_set_global_engine_async(bus, "OpenBangla", -1, nullptr, nullptr, nullptr);
  }
  ibus_main();
}

int main(int argc, char* argv[]) {
    gSettings = new Settings();
    update_with_settings();
    initKeycode();
    ctx = riti_context_new();

    start_setup(argc > 1 && strcmp(argv[1], "--ibus") == 0);

    riti_context_free(ctx);
    return 0;
}
