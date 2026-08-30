#include "nudi_composer.hpp"

#include <gtk/gtk.h>

#include <cctype>
#include <string>

namespace
{

    struct Editor
    {
        GtkWidget *window;
        GtkWidget *view;
        GtkWidget *status;
        nudi::Composer composer;
        std::string document;
    };

    void remove_last_utf8(std::string &text)
    {
        if (text.empty())
            return;
        std::size_t index = text.size() - 1;
        while (index > 0 && (static_cast<unsigned char>(text[index]) & 0xC0) == 0x80)
            --index;
        text.erase(index);
    }

    void refresh(Editor *editor)
    {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
        const std::string value = editor->document + editor->composer.preedit();
        gtk_text_buffer_set_text(buffer, value.c_str(), -1);
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_place_cursor(buffer, &end);
        gtk_label_set_text(GTK_LABEL(editor->status), editor->composer.empty()
                                                          ? "Nudi ready"
                                                          : "Nudi composing");
    }

    gboolean on_key_press(GtkWidget *, GdkEventKey *event, Editor *editor)
    {
        if ((event->state & (GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_SUPER_MASK)) != 0)
            return FALSE;
        if (event->keyval == GDK_KEY_BackSpace)
        {
            if (!editor->composer.empty())
            {
                editor->composer.backspace();
            }
            else
            {
                remove_last_utf8(editor->document);
            }
            refresh(editor);
            return TRUE;
        }
        if (event->keyval == GDK_KEY_space || event->keyval == GDK_KEY_Return ||
            event->keyval == GDK_KEY_KP_Enter)
        {
            editor->document += editor->composer.preedit();
            editor->composer.reset();
            editor->document += event->keyval == GDK_KEY_space ? " " : "\n";
            refresh(editor);
            return TRUE;
        }

        const gunichar unicode = gdk_keyval_to_unicode(event->keyval);
        if (unicode < 0x20 || unicode > 0x7e)
            return FALSE;
        const char key = static_cast<char>(g_ascii_tolower(static_cast<gchar>(unicode)));
        const bool shifted = (event->state & GDK_SHIFT_MASK) != 0;
        const bool caps_lock = (event->state & GDK_LOCK_MASK) != 0;
        const bool scroll_lock = (event->state & GDK_MOD3_MASK) != 0;
        const std::string committed = editor->composer.feed(key, shifted, caps_lock, scroll_lock);
        editor->document += committed;
        refresh(editor);
        return TRUE;
    }

    void on_start_engine(GtkButton *, Editor *editor)
    {
        GError *error = nullptr;
        const gboolean started = g_spawn_command_line_async(
            "systemctl --user restart org.freedesktop.IBus.session.GNOME.service", &error);
        if (started)
        {
            gtk_label_set_text(GTK_LABEL(editor->status), "Nudi engine restarted in background");
        }
        else
        {
            gtk_label_set_text(GTK_LABEL(editor->status), error->message);
            g_error_free(error);
        }
    }

    void on_activate(GtkApplication *application, gpointer)
    {
        Editor *editor = new Editor{};
        if (g_strcmp0(g_getenv("NUDI_DEFAULT_VOWEL"), "virama") == 0)
        {
            editor->composer.set_default_vowel(
                nudi::Composer::DefaultVowel::Virama);
        }
        editor->window = gtk_application_window_new(application);
        gtk_window_set_title(GTK_WINDOW(editor->window), "Nudi Kannada Editor");
        gtk_window_set_default_size(GTK_WINDOW(editor->window), 720, 460);

        GtkWidget *layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_container_set_border_width(GTK_CONTAINER(layout), 16);
        gtk_container_add(GTK_CONTAINER(editor->window), layout);

        GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
        gtk_box_pack_start(GTK_BOX(layout), header, FALSE, FALSE, 0);
        GtkWidget *title = gtk_label_new("Nudi Kannada");
        gtk_widget_set_halign(title, GTK_ALIGN_START);
        gtk_box_pack_start(GTK_BOX(header), title, TRUE, TRUE, 0);
        GtkWidget *start = gtk_button_new_with_label("Start Nudi Engine");
        gtk_box_pack_end(GTK_BOX(header), start, FALSE, FALSE, 0);

        editor->view = gtk_text_view_new();
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor->view), GTK_WRAP_WORD_CHAR);
        gtk_text_view_set_monospace(GTK_TEXT_VIEW(editor->view), FALSE);
        gtk_widget_set_vexpand(editor->view, TRUE);
        gtk_box_pack_start(GTK_BOX(layout), editor->view, TRUE, TRUE, 0);

        editor->status = gtk_label_new("Nudi ready");
        gtk_widget_set_halign(editor->status, GTK_ALIGN_START);
        gtk_box_pack_start(GTK_BOX(layout), editor->status, FALSE, FALSE, 0);

        g_signal_connect(editor->view, "key-press-event", G_CALLBACK(on_key_press), editor);
        g_signal_connect(start, "clicked", G_CALLBACK(on_start_engine), editor);
        g_object_set_data_full(G_OBJECT(editor->window), "nudi-editor", editor,
                               [](gpointer data)
                               { delete static_cast<Editor *>(data); });
        gtk_widget_show_all(editor->window);
        gtk_widget_grab_focus(editor->view);
    }

} // namespace

int main(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new(
        "com.example.NudiEditor", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(on_activate), nullptr);
    const int status = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return status;
}
