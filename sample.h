#ifndef __SAMPLE_H__
#define __SAMPLE_H__


#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4panel/xfce-hvbox.h>

G_BEGIN_DECLS

/* plugin structure */
typedef struct
{
    XfcePanelPlugin *plugin;

    /* panel widgets */
    GtkWidget *btn;
    GtkWidget *layout_image;
    
    gint hsize;                   /* plugin horizontal size */
    gint vsize;                   /* plugin vertical size */
    gint button_hsize;            /* real allocated button size - see below */
    gint button_vsize;            /* read allocated button size - see below */
    gchar* filename;
    gint state;
}
SamplePlugin;


G_END_DECLS

#endif /* !__SAMPLE_H__ */
