#include "aps_drawing.h"
#include "aps_plugin.h"

void aps_draw_icon(cairo_t *cr, const gchar *group_name, gint actual_width, gint actual_height, gint width, gint height) {
    gchar *filename;
    RsvgHandle *handle;
    RsvgDimensionData dim;
    double scalex, scaley;
    double layoutx, layouty, img_width, img_height;

    g_assert (cr != NULL);

    if (!group_name)
        return;

    filename = g_strconcat("/usr/share/xfce4/xkb/flags/", group_name, ".svg", NULL);
    handle = rsvg_handle_new_from_file(filename, NULL);
    g_free(filename);

    rsvg_handle_get_dimensions(handle, &dim);

    scalex = (double) (width - 4) / dim.width;
    scaley = (double) (height - 4) / dim.height;

    img_width = dim.width * scalex;
    img_height = dim.height * scaley;

    layoutx = (actual_width - img_width) / 2;
    layouty = (actual_height - img_height) / 2;
    cairo_translate(cr, layoutx, layouty);

    cairo_save(cr);

    cairo_scale(cr, scalex, scaley);
    rsvg_handle_render_cairo(handle, cr);

    cairo_restore(cr);

    rsvg_handle_close(handle, NULL);
    g_object_unref(handle);
}
