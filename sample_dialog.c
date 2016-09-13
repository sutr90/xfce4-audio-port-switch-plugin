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
    GtkWidget *entry_speaker, *entry_headphones;
    GtkWidget *content_area;

dialog = gtk_dialog_new_with_buttons ("Port settings", GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plugin))),
                                      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, _("_OK"),
                                      GTK_RESPONSE_ACCEPT, _("_Cancel"), GTK_RESPONSE_REJECT, NULL);
                                      
                                      gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    entry_speaker = gtk_entry_new();
    entry_headphones = gtk_entry_new();
    
    gtk_entry_set_text(GTK_ENTRY(entry_speaker), sample->port_speaker);
	gtk_entry_set_text(GTK_ENTRY(entry_headphones), sample->port_headphones);

    gtk_box_pack_start (GTK_BOX (content_area), gtk_label_new ("Headphones Port"), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX (content_area), entry_headphones, TRUE, TRUE, 0);    

    gtk_box_pack_start(GTK_BOX (content_area), gtk_label_new ("Speakers Port"), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX (content_area), entry_speaker, TRUE, TRUE, 0);


    gtk_widget_show_all(dialog);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    if(result == GTK_RESPONSE_ACCEPT){
		sample->port_speaker = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_speaker)));
		sample->port_headphones = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_headphones)));
	}

    gtk_widget_destroy(dialog);

}
