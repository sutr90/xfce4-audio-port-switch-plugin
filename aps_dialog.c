#include <gtk/gtk.h>

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "aps_plugin.h"
#include "aps_dialog.h"

void sample_configure(XfcePanelPlugin *plugin, SamplePlugin *sample) {
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

    g_object_set_data (G_OBJECT (plugin), "dialog", dialog);
    gtk_widget_show_all(dialog);    
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    if(result == GTK_RESPONSE_ACCEPT){
		g_free(sample->port_speaker);
		g_free(sample->port_headphones);
		sample->port_speaker = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_speaker)));
		sample->port_headphones = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_headphones)));
	}
	
	g_object_set_data (G_OBJECT (sample->plugin), "dialog", NULL);
    gtk_widget_destroy(dialog);
}
