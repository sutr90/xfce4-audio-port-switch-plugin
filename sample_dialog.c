#include <string.h>
#include <gtk/gtk.h>

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "sample.h"
#include "sample_dialog.h"

/* the website url */
#define PLUGIN_WEBSITE "http://goodies.xfce.org/projects/panel-plugins/xfce4-sample-plugin"


static void
sample_configure_response(GtkWidget *dialog,
                          gint response,
                          SamplePlugin *sample) {
    gboolean result;

    if (response == GTK_RESPONSE_HELP) {
        /* show help */
        result = g_spawn_command_line_async("exo-open --launch WebBrowser " PLUGIN_WEBSITE, NULL);

        if (G_UNLIKELY (result == FALSE))
            g_warning (_("Unable to open the following url: %s"), PLUGIN_WEBSITE);
    }
    else {
        /* remove the dialog data from the plugin */
        g_object_set_data(G_OBJECT (sample->plugin), "dialog", NULL);

        /* unlock the panel menu */
        xfce_panel_plugin_unblock_menu(sample->plugin);

        /* destroy the properties dialog */
        gtk_widget_destroy(dialog);
    }
}


void
sample_configure(XfcePanelPlugin *plugin,
                 SamplePlugin *sample) {
    GtkWidget *dialog;
    GtkWidget *entry1, *entry2;
    GtkWidget *content_area;

    dialog = gtk_dialog_new();
    gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", 0);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "CANCEL", 1);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    entry1 = gtk_entry_new();
    entry2 = gtk_entry_new();

    gtk_box_pack_start (GTK_BOX (content_area), gtk_label_new ("One"), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX (content_area), entry1, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX (content_area), gtk_label_new ("Two"), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX (content_area), entry2, TRUE, TRUE, 0);

    gtk_widget_show_all(dialog);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    const gchar* entry_line = ""; // variable for entered text

    switch(result)
    {
        case 0:
            entry_line = gtk_entry_get_text(GTK_ENTRY(entry1));
            break;

        case 1:
            printf("CANCEL was clicked\n");
            break;
        default:
            printf("Undefined. Perhaps dialog was closed\n");
    }

    gtk_widget_destroy(dialog);

}
