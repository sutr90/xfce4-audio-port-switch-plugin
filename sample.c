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

static SamplePlugin *
sample_new(XfcePanelPlugin *plugin) {
    SamplePlugin *sample;

    /* allocate memory for the plugin structure */
    sample = panel_slice_new0 (SamplePlugin);

    /* pointer to plugin */
    sample->plugin = plugin;
    sample->state = 0;
    sample->filename = _("vn");

    sample->btn = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON (sample->btn), GTK_RELIEF_NONE);
    gtk_container_add(GTK_CONTAINER (sample->plugin), sample->btn);
    xfce_panel_plugin_add_action_widget(sample->plugin, sample->btn);
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

void sample_plugin_button_clicked(GtkButton *btn, gpointer data) {
    SamplePlugin *plg = (SamplePlugin *) data;
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

static void
sample_construct(XfcePanelPlugin *plugin) {
    SamplePlugin *sample;

    /* create the plugin */
    sample = sample_new(plugin);

    /* connect plugin signals */
    g_signal_connect (G_OBJECT(plugin), "free-data", G_CALLBACK(sample_free), sample);
    g_signal_connect (plugin, "size-changed", G_CALLBACK(xfce_sample_set_size), sample);
    g_signal_connect (plugin, "orientation-changed", G_CALLBACK(xfce_sample_orientation_changed), sample);

    /* show the configure menu item and connect signal */
    xfce_panel_plugin_menu_show_configure (plugin);
    g_signal_connect (G_OBJECT (plugin), "configure-plugin",
                      G_CALLBACK (sample_configure), sample);

}
