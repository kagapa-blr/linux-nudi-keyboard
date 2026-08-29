#include "nudi_composer.hpp"

#include <ibus.h>

#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unistd.h>

#include <glib/gstdio.h>

namespace {

std::string log_path() {
    const std::time_t now = std::time(nullptr);
    std::tm local_time{};
    localtime_r(&now, &local_time);

    std::ostringstream date;
    date << std::put_time(&local_time, "%Y-%m-%d");
    const std::string directory = "/var/log/kannada-nudi/" + date.str();
    if (g_mkdir_with_parents(directory.c_str(), 0777) != 0) return {};
    g_chmod(directory.c_str(), 01777);
    return directory + "/engine-" + std::to_string(getuid()) + ".log";
}

void log_event(const std::string& event) {
    const std::string path = log_path();
    if (path.empty()) return;

    std::ofstream log(path, std::ios::app);
    if (!log) return;

    const std::time_t now = std::time(nullptr);
    std::tm local_time{};
    localtime_r(&now, &local_time);
    log << std::put_time(&local_time, "%Y-%m-%dT%H:%M:%S%z") << " " << event << '\n';
}

}  // namespace

typedef struct _NudiEngine {
    IBusEngine parent;
    nudi::Composer* composer;
} NudiEngine;

typedef struct _NudiEngineClass {
    IBusEngineClass parent_class;
} NudiEngineClass;

G_DEFINE_TYPE(NudiEngine, nudi_engine, IBUS_TYPE_ENGINE)

static void register_component(IBusBus* bus) {
    static const char* candidates[] = {
        "/usr/share/ibus/component/com.example.Nudi.xml",
        "/usr/local/share/ibus/component/com.example.Nudi.xml",
        "./com.example.Nudi.xml",
        nullptr,
    };

    for (size_t i = 0; candidates[i] != nullptr; ++i) {
        if (!g_file_test(candidates[i], G_FILE_TEST_EXISTS)) {
            continue;
        }

        IBusComponent* component = ibus_component_new_from_file(candidates[i]);
        if (component == nullptr) {
            continue;
        }

        ibus_bus_register_component(bus, component);
        log_event("component_registered");
        g_object_unref(component);
        return;
    }

    g_printerr("Unable to locate IBus component metadata for Nudi.\n");
    log_event("component_metadata_missing");
}

static void update_preedit(NudiEngine* engine) {
    const std::string& value = engine->composer->preedit();
    IBusText* text = ibus_text_new_from_string(value.c_str());
    ibus_engine_update_preedit_text(
        IBUS_ENGINE(engine), text, g_utf8_strlen(value.c_str(), -1), !value.empty());
    g_object_unref(text);
}

static void nudi_engine_reset(IBusEngine* ibus_engine) {
    NudiEngine* engine = reinterpret_cast<NudiEngine*>(ibus_engine);
    log_event("reset");
    engine->composer->reset();
    update_preedit(engine);
    IBUS_ENGINE_CLASS(nudi_engine_parent_class)->reset(ibus_engine);
}

static gboolean nudi_engine_process_key_event(
    IBusEngine* ibus_engine, guint keyval, guint, guint state) {
    NudiEngine* engine = reinterpret_cast<NudiEngine*>(ibus_engine);
    log_event("key keyval=" + std::to_string(keyval) + " state=" + std::to_string(state));
    if ((state & IBUS_RELEASE_MASK) != 0) return FALSE;
    const guint blocked = IBUS_CONTROL_MASK | IBUS_SUPER_MASK;
    const bool shifted = (state & IBUS_SHIFT_MASK) != 0;
    const bool caps_lock = (state & IBUS_LOCK_MASK) != 0;
    const bool alt = (state & IBUS_MOD1_MASK) != 0;
    if ((state & blocked) != 0 || (alt && !(caps_lock && shifted)) ||
        keyval == IBUS_KEY_Caps_Lock || keyval == IBUS_KEY_Num_Lock)
        return FALSE;

    if (keyval == IBUS_KEY_BackSpace) {
        if (!engine->composer->backspace()) return FALSE;
        update_preedit(engine);
        return TRUE;
    }

    if (keyval == IBUS_KEY_Delete) {
        if (engine->composer->empty()) return FALSE;
        engine->composer->reset();
        update_preedit(engine);
        log_event("delete_preedit");
        return TRUE;
    }

    if (keyval == IBUS_KEY_space || keyval == IBUS_KEY_Return || keyval == IBUS_KEY_KP_Enter ||
        keyval == IBUS_KEY_Tab) {
        if (!engine->composer->empty()) {
            const std::string value = engine->composer->separator();
            IBusText* text = ibus_text_new_from_string(value.c_str());
            ibus_engine_commit_text(ibus_engine, text);
            g_object_unref(text);
            engine->composer->reset();
            update_preedit(engine);
            log_event("commit separator");
        }
        if (keyval == IBUS_KEY_Tab || keyval == IBUS_KEY_space ||
            keyval == IBUS_KEY_Return || keyval == IBUS_KEY_KP_Enter)
            return FALSE;
        return FALSE;
    }

    if (keyval < 0x20 || keyval > 0x7e) return FALSE;
    const bool scroll_lock = (state & IBUS_MOD3_MASK) != 0;
    const bool num_lock = (state & IBUS_MOD2_MASK) != 0;
    const char key = static_cast<char>(g_ascii_tolower(static_cast<gchar>(keyval)));
    const std::string committed = engine->composer->feed(
        key, shifted, caps_lock, scroll_lock, num_lock, alt);
    if (!committed.empty()) {
        IBusText* text = ibus_text_new_from_string(committed.c_str());
        ibus_engine_commit_text(ibus_engine, text);
        g_object_unref(text);
        log_event("commit length=" + std::to_string(committed.size()));
    }
    update_preedit(engine);
    return TRUE;
}

static void nudi_engine_finalize(GObject* object) {
    NudiEngine* engine = reinterpret_cast<NudiEngine*>(object);
    log_event("engine_finalized");
    delete engine->composer;
    G_OBJECT_CLASS(nudi_engine_parent_class)->finalize(object);
}

static void nudi_engine_init(NudiEngine* engine) {
    engine->composer = new nudi::Composer();
    log_event("engine_initialized");
}

static void nudi_engine_class_init(NudiEngineClass* klass) {
    IBusEngineClass* engine_class = IBUS_ENGINE_CLASS(klass);
    engine_class->process_key_event = nudi_engine_process_key_event;
    engine_class->reset = nudi_engine_reset;
    GObjectClass* object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = nudi_engine_finalize;
}

int main() {
    log_event("engine_start");
    ibus_init();
    IBusBus* bus = ibus_bus_new();
    if (!ibus_bus_is_connected(bus)) {
        log_event("ibus_connection_failed");
        g_object_unref(bus);
        return 1;
    }

    register_component(bus);

    IBusFactory* factory = ibus_factory_new(ibus_bus_get_connection(bus));
    ibus_factory_add_engine(factory, "nudi", nudi_engine_get_type());
    ibus_main();
    g_object_unref(factory);
    g_object_unref(bus);
    return 0;
}