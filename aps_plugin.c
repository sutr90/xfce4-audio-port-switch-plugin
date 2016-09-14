#include "aps_plugin.h"
#include "aps_dialog.h"
#include "aps_drawing.h"

/* prototypes */
static void aps_construct(XfcePanelPlugin *plugin);

gboolean aps_layout_image_exposed(GtkWidget *widget, GdkEventExpose *event, SamplePlugin *sample);

static gboolean aps_calculate_sizes(SamplePlugin *sample, GtkOrientation orientation, gint panel_size);

static void aps_orientation_changed(XfcePanelPlugin *plugin, GtkOrientation orientation, SamplePlugin *sample);

static gboolean aps_set_size(XfcePanelPlugin *plugin, gint size, SamplePlugin *sample);

void aps_button_clicked(GtkButton *btn, gpointer data);

XFCE_PANEL_PLUGIN_REGISTER (aps_construct);


void aps_button_clicked(GtkButton *btn, gpointer data) {
    SamplePlugin *plg = (SamplePlugin *) data;
    GError *error = NULL;

    gchar *command;
    if (plg->state == 0) {
        command = g_strconcat("pactl set-sink-port ", plg->port_speaker, NULL);
    } else {
        command = g_strconcat("pactl set-sink-port ", plg->port_headphones, NULL);
    }
    gint exit_status = 0;
    gboolean result;
    gchar *err_out;
    result = g_spawn_command_line_sync(command, NULL, &err_out, &exit_status, &error);

    g_free(command);

    if (G_UNLIKELY (result == FALSE)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW (gtk_widget_get_toplevel(GTK_WIDGET(plg))),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                   "Error executing switch command: “%s”", error->message);
        gtk_dialog_run(GTK_DIALOG (dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    if (!g_spawn_check_exit_status(exit_status, &error)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW (gtk_widget_get_toplevel(GTK_WIDGET(plg))),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                   "Error switching ports: “%s”", err_out);
        gtk_dialog_run(GTK_DIALOG (dialog));
        gtk_widget_destroy(dialog);
        g_free(err_out);
        return;
    }

    if (plg->state == 0) {
        plg->filename = _("speaker");
    } else {
        plg->filename = _("headphones");
    }
    plg->state = ~plg->state;
}

gboolean aps_layout_image_exposed(GtkWidget *widget, GdkEventExpose *event, SamplePlugin *sample) {
    const gchar *group_name;
    cairo_t *cr;
    gint actual_hsize, actual_vsize;

    actual_hsize = (sample->button_hsize > sample->hsize) ? sample->button_hsize : sample->hsize;
    actual_vsize = (sample->button_vsize > sample->vsize) ? sample->button_vsize : sample->vsize;

    group_name = sample->filename;

    cr = gdk_cairo_create(gtk_widget_get_window((GTK_WIDGET (sample->layout_image))));

    aps_draw_icon(cr, group_name, actual_hsize, actual_vsize, sample->hsize, sample->vsize);

    cairo_destroy(cr);

    return FALSE;
}

static void sample_free(XfcePanelPlugin *plugin, SamplePlugin *sample) {
    GtkWidget *dialog;

    /* check if the dialog is still open. if so, destroy it */
    dialog = g_object_get_data(G_OBJECT (plugin), "dialog");
    if (G_UNLIKELY (dialog != NULL)) {
        gtk_widget_destroy(dialog);
    }

    /* destroy the panel widgets */
    gtk_widget_destroy(sample->layout_image);
    gtk_widget_destroy(sample->btn);

    /* destroy data */
    g_free(sample->port_speaker);
    g_free(sample->port_headphones);

    /* free the plugin structure */
    panel_slice_free (SamplePlugin, sample);
}

static gboolean aps_calculate_sizes(SamplePlugin *sample, GtkOrientation orientation, gint panel_size) {
    sample->vsize = panel_size;
    sample->hsize = panel_size;
    gtk_widget_set_size_request(sample->btn, sample->hsize, sample->vsize);

    gtk_widget_queue_draw_area(sample->btn, 0, 0, sample->button_hsize, sample->button_vsize);
    return TRUE;
}

static void aps_orientation_changed(XfcePanelPlugin *plugin, GtkOrientation orientation, SamplePlugin *sample) {
    aps_calculate_sizes(sample, orientation, xfce_panel_plugin_get_size(plugin));
}

static gboolean aps_set_size(XfcePanelPlugin *plugin, gint size, SamplePlugin *sample) {
    return aps_calculate_sizes(sample, xfce_panel_plugin_get_orientation(plugin), size);
}

void sample_save(XfcePanelPlugin *plugin, SamplePlugin *sample) {
    XfceRc *rc;
    gchar *file;

    file = xfce_panel_plugin_save_location(plugin, TRUE);

    if (G_UNLIKELY (file == NULL)) {
        DBG ("Failed to open config file");
        return;
    }

    rc = xfce_rc_simple_open(file, FALSE);
    g_free(file);

    if (G_LIKELY (rc != NULL)) {
        if (sample->port_speaker)
            xfce_rc_write_entry(rc, "port_speaker", sample->port_speaker);

        if (sample->port_headphones)
            xfce_rc_write_entry(rc, "port_headphones", sample->port_headphones);

        if (sample->filename)
            xfce_rc_write_entry(rc, "filename", sample->filename);

        xfce_rc_write_int_entry(rc, "state", sample->state);

        xfce_rc_close(rc);
    }
}

static void sample_read(SamplePlugin *sample) {
    XfceRc *rc;
    gchar *file;
    const gchar *value;

    file = xfce_panel_plugin_save_location(sample->plugin, TRUE);

    if (G_LIKELY (file != NULL)) {
        rc = xfce_rc_simple_open(file, TRUE);

        g_free(file);

        if (G_LIKELY (rc != NULL)) {
            value = xfce_rc_read_entry(rc, "port_speaker", "");
            sample->port_speaker = g_strdup(value);

            value = xfce_rc_read_entry(rc, "port_headphones", "");
            sample->port_headphones = g_strdup(value);

            value = xfce_rc_read_entry(rc, "filename", "speaker");
            sample->filename = g_strdup(value);

            sample->state = xfce_rc_read_int_entry(rc, "state", 0);

            xfce_rc_close(rc);
            return;
        }
    }

    sample->port_speaker = g_strdup("");
    sample->port_headphones = g_strdup("");
}

static SamplePlugin *sample_new(XfcePanelPlugin *plugin) {
    SamplePlugin *sample;

    sample = panel_slice_new0 (SamplePlugin);
    sample->plugin = plugin;
    sample_read(sample);

    sample->btn = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON (sample->btn), GTK_RELIEF_NONE);
    gtk_widget_show(sample->btn);

    sample->layout_image = gtk_image_new();
    gtk_container_add(GTK_CONTAINER (sample->btn), sample->layout_image);
    g_signal_connect (G_OBJECT(sample->layout_image), "expose-event", G_CALLBACK(aps_layout_image_exposed), sample);
    g_signal_connect (sample->btn, "clicked", G_CALLBACK(aps_button_clicked), sample);

    gtk_widget_show(GTK_WIDGET (sample->layout_image));
    gtk_widget_queue_draw_area(sample->btn, 0, 0, sample->button_hsize, sample->button_vsize);

    return sample;
}

static void aps_construct(XfcePanelPlugin *plugin) {
    SamplePlugin *sample;

    /* create the plugin */
    sample = sample_new(plugin);

    gtk_container_add(GTK_CONTAINER (sample->plugin), sample->btn);
    xfce_panel_plugin_add_action_widget(sample->plugin, sample->btn);
    /* connect plugin signals */
    g_signal_connect (G_OBJECT(plugin), "free-data", G_CALLBACK(sample_free), sample);
    g_signal_connect (G_OBJECT(plugin), "size-changed", G_CALLBACK(aps_set_size), sample);
    g_signal_connect (G_OBJECT(plugin), "orientation-changed", G_CALLBACK(aps_orientation_changed), sample);
    g_signal_connect (G_OBJECT(plugin), "save", G_CALLBACK(sample_save), sample);

    /* show the configure menu item and connect signal */
    xfce_panel_plugin_menu_show_configure(plugin);
    g_signal_connect (G_OBJECT(plugin), "configure-plugin", G_CALLBACK(aps_show_config), sample);
}
