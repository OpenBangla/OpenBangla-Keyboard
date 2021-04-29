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
static Suggestion *suggestion = nullptr;
/* Unfortunately, we have to keep track of the right Alt Key. */
static bool altGr = false;

void update_with_settings() {
    qputenv("RITI_LAYOUT_FILE", gSettings->getLayoutPath().toLatin1());
    qputenv("RITI_PHONETIC_DATABASE_ON", gSettings->getShowCWPhonetic() ? "true" : "false");
    qputenv("RITI_PHONETIC_INCLUDE_ENGLISH", gSettings->getIncludeEnglishPrevWin() ? "true" : "false");
    qputenv("RITI_DATABASE_DIR", DatabasePath().toLatin1());
    qputenv("RITI_LAYOUT_FIXED_DATABASE_ON", gSettings->getShowPrevWinFixed() ? "true" : "false");
    qputenv("RITI_LAYOUT_FIXED_VOWEL", gSettings->getAutoVowelFormFixed() ? "true" : "false");
    qputenv("RITI_LAYOUT_FIXED_CHANDRA", gSettings->getAutoChandraPosFixed() ? "true" : "false");
    qputenv("RITI_LAYOUT_FIXED_KAR", gSettings->getTraditionalKarFixed() ? "true" : "false");
    qputenv("RITI_LAYOUT_FIXED_OLD_REPH", gSettings->getOldReph() ? "true" : "false");
    qputenv("RITI_LAYOUT_FIXED_NUMBERPAD", gSettings->getNumberPadFixed() ? "true" : "false");

    if(table != nullptr) {
      ibus_lookup_table_set_orientation(table, gSettings->getCandidateWinHorizontal() ? IBUS_ORIENTATION_HORIZONTAL : IBUS_ORIENTATION_VERTICAL);
    }
}

void engine_update_preedit() {
  IBusText *text = nullptr;

  if (!riti_suggestion_is_lonely(suggestion)) {
    ibus_engine_update_lookup_table_fast(engine, table, TRUE);
    guint index = ibus_lookup_table_get_cursor_pos(table);
    text = ibus_lookup_table_get_candidate(table, index);
  } else {
    text = ibus_text_new_from_string(riti_suggestion_get_lonely_suggestion(suggestion));
  }

  ibus_engine_update_preedit_text(engine, text, ibus_text_get_length(text), TRUE);
}

void engine_update_lookup_table() {
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

    uintptr_t prev_selection = riti_suggestion_previously_selected_index(suggestion);

    ibus_lookup_table_set_cursor_pos(table, prev_selection);
  }

  engine_update_preedit();
}

void engine_reset() {
  ibus_lookup_table_clear(table);
  ibus_engine_hide_preedit_text(engine);
  ibus_engine_hide_auxiliary_text(engine);
  ibus_engine_hide_lookup_table(engine);
}

void engine_commit_text(IBusText * text) {
  ibus_engine_commit_text(engine, text);
  engine_reset();
}

void engine_commit() {
  uintptr_t index = 0;
  IBusText *text = nullptr;

  if (!riti_suggestion_is_lonely(suggestion)) {
    index = ibus_lookup_table_get_cursor_pos(table);
    text = ibus_lookup_table_get_candidate(table, index);
  } else {
    index = 0;
    char *txt = riti_suggestion_get_lonely_suggestion(suggestion);
    text = ibus_text_new_from_string(txt);
  }

  engine_commit_text(text);
  riti_context_candidate_committed(ctx, index);
}

/************************************* Callbacks *************************************/

gboolean engine_process_key_event_cb(IBusEngine *engine,
                                   guint keyval,
                                   guint keycode,
                                   guint state) {
  u_int8_t modifier = 0;
  bool ctrl_key = false;
  bool alt_key = false;

  // Don't accept Key Release event
  if (state & IBUS_RELEASE_MASK) {
    if(keyval == IBUS_KEY_Alt_R || keyval == IBUS_KEY_ISO_Level3_Shift) {
      altGr = false;
    }
    return FALSE;
  }

  if(!riti_context_ongoing_input_session(ctx)) {
    update_with_settings();
    riti_context_update_engine(ctx);
  }

  // At first, handle the special keys.
  switch (keyval) {
    case IBUS_KEY_BackSpace:
      if(riti_context_ongoing_input_session(ctx)) {
        suggestion = riti_context_backspace_event(ctx);

        if(!riti_suggestion_is_empty(suggestion)) {
          engine_update_lookup_table();
        } else {
          engine_reset();
        }

        return TRUE;
      } else {
        return FALSE;
      }
    case IBUS_KEY_Return:
      if(riti_context_ongoing_input_session(ctx)) {
        engine_commit();
        return (gboolean) gSettings->getEnterKeyClosesPrevWin();
      } else {
        return FALSE;
      }
    case IBUS_KEY_space:
    case IBUS_KEY_KP_Enter:
      if(riti_context_ongoing_input_session(ctx)) {
        engine_commit();
      }
      return FALSE;
    /** Arrow and Tab keys.
     * We use the arrow keys and the tab key to move the selection
     * in the preview window. So we have to ensure the preview 
     * window is shown by checking if the current suggestion is
     * not a lonely one. Otherwise we don't handle it.
     **/
    case IBUS_KEY_Right:
    case IBUS_KEY_Left:
      if(riti_context_ongoing_input_session(ctx)) {
        if(gSettings->getCandidateWinHorizontal() && !riti_suggestion_is_lonely(suggestion)) {
          if(keyval == IBUS_KEY_Right) {
            ibus_lookup_table_cursor_down(table);
          } else {
            ibus_lookup_table_cursor_up(table);
          }
          engine_update_preedit();
          return TRUE;
        } else {
          engine_commit();
          return FALSE;
        }
      } else {
        return FALSE;
      }
    case IBUS_KEY_Up:
    case IBUS_KEY_Down:
      if(riti_context_ongoing_input_session(ctx)) {
        if(!gSettings->getCandidateWinHorizontal() && !riti_suggestion_is_lonely(suggestion)) {
          if(keyval == IBUS_KEY_Up) {
            ibus_lookup_table_cursor_up(table);
          } else {
            ibus_lookup_table_cursor_down(table);
          }
          engine_update_preedit();
          return TRUE;
        } else {
          engine_commit();
          return FALSE;
        }
      } else {
        return FALSE;
      }
    case IBUS_KEY_Tab:
      if(riti_context_ongoing_input_session(ctx)) {
        if(!riti_suggestion_is_lonely(suggestion)) {
          ibus_lookup_table_cursor_down(table);
          engine_update_preedit();
          return TRUE;
        } else {
          engine_commit();
          return FALSE;
        }
      } else {
        return FALSE;
      }
    /** Modifier keys **/
    case IBUS_KEY_Alt_R:
    case IBUS_KEY_ISO_Level3_Shift:
    altGr = true; // Keep track of the right Alt key (also known as the AltGr key)
    case IBUS_KEY_Shift_L:
    case IBUS_KEY_Shift_R:
    case IBUS_KEY_Control_L:
    case IBUS_KEY_Control_R:
    case IBUS_KEY_Alt_L:
    case IBUS_KEY_Meta_L:
    case IBUS_KEY_Meta_R:
      return (gboolean) riti_context_ongoing_input_session(ctx);
    default:
      break;
  }

  // Set modifiers
  if (state & IBUS_SHIFT_MASK) {
    modifier |= MODIFIER_SHIFT;
  }

  if (state & IBUS_CONTROL_MASK) {
    ctrl_key = true;
  }

  if (state & IBUS_MOD1_MASK) {
    alt_key = true;
  }

  // Convert the key value into riti's key value.
  uint16_t key = ibus_keycode(keyval);

  // Reject the key which has only Ctrl or Alt (not the right one) combination and riti doesn't handle.
  if((ctrl_key && !alt_key) || (!ctrl_key && alt_key && !altGr) || key == VC_UNKNOWN) {
    if(riti_context_ongoing_input_session(ctx)) {
      engine_commit();
    }
    return FALSE;
  }

  // If we have Ctrl and Alt combination or the right Alt, set it as the AltGr modifier.
  if((ctrl_key && alt_key) || altGr) {
    modifier |= MODIFIER_ALT_GR;
  }

  suggestion = riti_get_suggestion_for_key(ctx, key, modifier);

  if(!riti_suggestion_is_empty(suggestion)) {
    engine_update_lookup_table();
  } else {
    return FALSE;
  }

  return TRUE;
}

void engine_enable_cb(IBusEngine *engine) {
  LOG_INFO("[IM:iBus]: IM enabled\n");
  update_with_settings();
}

void engine_disable_cb(IBusEngine *engine) {
  LOG_INFO("[IM:iBus]: IM disabled\n");
}

void engine_focus_out_cb(IBusEngine *engine) {
  LOG_DEBUG("[IM:iBus]: IM Focus out\n");

  if(riti_context_ongoing_input_session(ctx)) {
    riti_context_finish_input_session(ctx);
    engine_reset();
  }
}

void engine_candidate_clicked_cb(IBusEngine *engine, guint index, guint button, guint state) {
  IBusText *text = ibus_lookup_table_get_candidate(table, index);
  engine_commit_text(text);
  riti_context_candidate_committed(ctx, index);
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
  table = ibus_lookup_table_new(10, 0, TRUE, TRUE);
  g_object_ref_sink(table);

  LOG_INFO("[IM:iBus]: Creating IM Engine\n");
  LOG_DEBUG("[IM:iBus]: Creating IM Engine with name:%s and id:%d\n", engine_name, id);

  g_signal_connect(engine, "process-key-event", G_CALLBACK(engine_process_key_event_cb), NULL);
  g_signal_connect(engine, "enable", G_CALLBACK(engine_enable_cb), NULL);
  g_signal_connect(engine, "disable", G_CALLBACK(engine_disable_cb), NULL);
  g_signal_connect(engine, "focus-out", G_CALLBACK(engine_focus_out_cb), NULL);
  g_signal_connect(engine, "candidate-clicked", G_CALLBACK(engine_candidate_clicked_cb), NULL);

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
                                   LIBEXECDIR "/ibus-engine-openbangla --ibus",
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
    UserFolders folders;
    update_with_settings();
    initKeycode();
    ctx = riti_context_new();

    start_setup(argc > 1 && strcmp(argv[1], "--ibus") == 0);

    riti_context_free(ctx);
    return 0;
}
