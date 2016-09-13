#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "sample.h"
#include "sample_dialog.h"
#include "cairo.h"

/* prototypes */
static void
sample_construct(XfcePanelPlugin *plugin);

gboolean sample_plugin_layout_image_exposed(GtkWidget *widget, GdkEventExpose *event, SamplePlugin *sample);

void sample_plugin_button_size_allocated(GtkWidget *button, GtkAllocation *allocation, SamplePlugin *sample);

static gboolean sample_calculate_sizes(SamplePlugin *sample, GtkOrientation orientation, gint panel_size);

static void xfce_sample_orientation_changed(XfcePanelPlugin *plugin, GtkOrientation orientation, SamplePlugin *sample);

static gboolean xfce_sample_set_size(XfcePanelPlugin *plugin, gint size, SamplePlugin *sample);

void sample_plugin_button_clicked(GtkButton *btn, gpointer data);

void sample_refresh_gui(SamplePlugin *sample);

/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER (sample_construct);


void sample_plugin_button_clicked(GtkButton *btn, gpointer data) {
    SamplePlugin *plg = (SamplePlugin *) data;
    GError *error = NULL;
    
    gchar *command;
   if(plg->state == 0){
	   command = g_strconcat ("pactl set-sink-port ", plg->port_speaker, NULL);
   } else {
	   command = g_strconcat ("pactl set-sink-port ", plg->port_headphones, NULL);
   }
   
   gboolean result = g_spawn_command_line_async (command, &error);
	g_free(command);
    
    if (G_UNLIKELY (result == FALSE)){
        GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plg))),
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error switching ports: “%s”", error->message);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		return;
    }
    //0 "analog-output;output-amplifier-on"
    //0 "analog-output;output-amplifier-off"
    
    if (plg->state == 0) {
        plg->filename = _("so");
    } else {
        plg->filename = _("vn");
    }
    plg->state = ~plg->state;
}

gboolean
sample_plugin_layout_image_exposed(GtkWidget *widget,
                                   GdkEventExpose *event,
                                   SamplePlugin *sample) {
    const gchar *group_name;
    cairo_t *cr;
    gint actual_hsize, actual_vsize;
    gint vsize;

    actual_hsize = (sample->button_hsize > sample->hsize) ? sample->button_hsize : sample->hsize;
    actual_vsize = (sample->button_vsize > sample->vsize) ? sample->button_vsize : sample->vsize;

    vsize = MIN (sample->vsize, (int) (sample->hsize * 0.75));

    group_name = sample->filename;

    cr = gdk_cairo_create(gtk_widget_get_window((GTK_WIDGET (sample->layout_image))));


    sample_cairo_draw_flag(cr, group_name,
                           actual_hsize, actual_vsize,
                           sample->hsize, vsize
    );

    cairo_destroy(cr);

    return FALSE;
}

static void
sample_free(XfcePanelPlugin *plugin,
            SamplePlugin *sample) {
    GtkWidget *dialog;

    /* check if the dialog is still open. if so, destroy it */
    dialog = g_object_get_data(G_OBJECT (plugin), "dialog");
    if (G_UNLIKELY (dialog != NULL))
        gtk_widget_destroy(dialog);

    /* destroy the panel widgets */
    gtk_widget_destroy(sample->layout_image);
    gtk_widget_destroy(sample->btn);
    
    /* destroy data */
    g_free(sample->port_speaker);
    g_free(sample->port_headphones);

    /* free the plugin structure */
    panel_slice_free (SamplePlugin, sample);
}

void
sample_plugin_button_size_allocated(GtkWidget *button,
                                    GtkAllocation *allocation,
                                    SamplePlugin *sample) {
    sample->button_hsize = allocation->width;
    sample->button_vsize = allocation->height;
}

static gboolean
sample_calculate_sizes(SamplePlugin *sample, GtkOrientation orientation, gint panel_size) {
    guint nrows;

    nrows = xfce_panel_plugin_get_nrows(sample->plugin);
    panel_size /= nrows;
    TRACE ("calculate_sizes(%p: %d,%d)", sample, panel_size, nrows);

    switch (orientation) {
        case GTK_ORIENTATION_HORIZONTAL:
            sample->vsize = panel_size;
            if (nrows > 1) {
                sample->hsize = sample->vsize;
            }
            else {
                sample->hsize = (int) (1.33 * panel_size);
            }

            gtk_widget_set_size_request(sample->btn, sample->hsize, sample->vsize);
            break;
        case GTK_ORIENTATION_VERTICAL:
            sample->hsize = panel_size;
            if (nrows > 1) {
                sample->vsize = sample->hsize;
            }
            else {
                sample->vsize = (int) (0.75 * panel_size);
            }
            if (sample->vsize < 10) sample->vsize = 10;
            gtk_widget_set_size_request(sample->btn, sample->hsize, sample->vsize);
            break;
        default:
            break;
    }

    DBG ("size requested: h/v (%p: %d/%d)", sample, sample->hsize, sample->vsize);
    sample_refresh_gui(sample);
    return TRUE;
}

static void
xfce_sample_orientation_changed(XfcePanelPlugin *plugin,
                                GtkOrientation orientation,
                                SamplePlugin *sample) {
    sample_calculate_sizes(sample, orientation, xfce_panel_plugin_get_size(plugin));
}

static gboolean
xfce_sample_set_size(XfcePanelPlugin *plugin, gint size,
                     SamplePlugin *sample) {
    return sample_calculate_sizes(sample, xfce_panel_plugin_get_orientation(plugin), size);
}

void
sample_refresh_gui(SamplePlugin *sample) {
    /* Part of the image may remain visible after display type change */
    gtk_widget_queue_draw_area(sample->btn, 0, 0, sample->button_hsize, sample->button_vsize);
}

void
sample_save (XfcePanelPlugin *plugin,
             SamplePlugin    *sample)
{
  XfceRc *rc;
  gchar  *file;

  /* get the config file location */
  file = xfce_panel_plugin_save_location (plugin, TRUE);

  if (G_UNLIKELY (file == NULL))
    {
       DBG ("Failed to open config file");
       return;
    }

  /* open the config file, read/write */
  rc = xfce_rc_simple_open (file, FALSE);
  g_free (file);

  if (G_LIKELY (rc != NULL))
    {
      /* save the settings */
      if (sample->port_speaker)
        xfce_rc_write_entry    (rc, "port_speaker", sample->port_speaker);
        
      if (sample->port_headphones)
        xfce_rc_write_entry    (rc, "port_headphones", sample->port_headphones);
        
      if (sample->filename)
        xfce_rc_write_entry    (rc, "filename", sample->filename);
        
      xfce_rc_write_int_entry(rc, "state", sample->state);

      /* close the rc file */
      xfce_rc_close (rc);
    }
}

static void
sample_read (SamplePlugin *sample)
{
  XfceRc      *rc;
  gchar       *file;
  const gchar *value;

  /* get the plugin config file location */
  file = xfce_panel_plugin_save_location (sample->plugin, TRUE);

  if (G_LIKELY (file != NULL))
    {
      /* open the config file, readonly */
      rc = xfce_rc_simple_open (file, TRUE);

      /* cleanup */
      g_free (file);

      if (G_LIKELY (rc != NULL))
        {
          /* read the settings */
          value = xfce_rc_read_entry (rc, "port_speaker", "");
          sample->port_speaker = g_strdup (value);
          
          value = xfce_rc_read_entry (rc, "port_headphones", "");
          sample->port_headphones = g_strdup (value);
          
          value = xfce_rc_read_entry (rc, "filename", "vn");
          sample->filename = g_strdup (value);
          
          sample->state = xfce_rc_read_int_entry (rc, "state", 0);

          /* cleanup */
          xfce_rc_close (rc);

          /* leave the function, everything went well */
          return;
        }
    }

  /* something went wrong, apply default values */
  DBG ("Applying default settings");

  sample->port_speaker = g_strdup ("");
   sample->port_headphones = g_strdup ("");
 }
 
 static SamplePlugin *
sample_new(XfcePanelPlugin *plugin) {
    SamplePlugin *sample;

    /* allocate memory for the plugin structure */
    sample = panel_slice_new0 (SamplePlugin);

    /* pointer to plugin */
    sample->plugin = plugin;
    sample_read (sample);

/* todo load config from file */

    sample->btn = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON (sample->btn), GTK_RELIEF_NONE);
    gtk_widget_show(sample->btn);

    sample->layout_image = gtk_image_new();
    gtk_container_add(GTK_CONTAINER (sample->btn), sample->layout_image);
    g_signal_connect (G_OBJECT(sample->layout_image), "expose-event", G_CALLBACK(sample_plugin_layout_image_exposed),
                      sample);
    g_signal_connect (sample->btn, "clicked", G_CALLBACK(sample_plugin_button_clicked), sample);
    gtk_widget_show(GTK_WIDGET (sample->layout_image));

    sample_refresh_gui(sample);

    return sample;
}

static void
sample_construct(XfcePanelPlugin *plugin) {
    SamplePlugin *sample;

    /* create the plugin */
    sample = sample_new(plugin);

    gtk_container_add(GTK_CONTAINER (sample->plugin), sample->btn);
    xfce_panel_plugin_add_action_widget(sample->plugin, sample->btn);
    /* connect plugin signals */
    g_signal_connect (G_OBJECT(plugin), "free-data", G_CALLBACK(sample_free), sample);
    g_signal_connect (G_OBJECT(plugin), "size-changed", G_CALLBACK(xfce_sample_set_size), sample);
    g_signal_connect (G_OBJECT(plugin), "orientation-changed", G_CALLBACK(xfce_sample_orientation_changed), sample);
    g_signal_connect (G_OBJECT (plugin), "save", G_CALLBACK (sample_save), sample);

    /* show the configure menu item and connect signal */
    xfce_panel_plugin_menu_show_configure (plugin);
    g_signal_connect (G_OBJECT (plugin), "configure-plugin", G_CALLBACK (sample_configure), sample);
}
