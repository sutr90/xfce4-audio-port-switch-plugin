#include "cairo.h"
#include "sample.h"

void
sample_cairo_draw_flag (cairo_t *cr,
                     const gchar *group_name,
                     gint actual_width,
                     gint actual_height,
                     gint width,
                     gint height)
{
    gchar *filename;
    RsvgHandle *handle;
    RsvgDimensionData dim;
    double scalex, scaley;
    double layoutx, layouty, img_width, img_height;
   
    g_assert (cr != NULL);

    if (!group_name)
        return;

    filename = g_strconcat (".", group_name, ".svg", NULL);
    handle = rsvg_handle_new_from_file (filename, NULL);
    g_free (filename);

    rsvg_handle_get_dimensions (handle, &dim);

    scalex = (double) (width - 4) / dim.width;
    scaley = (double) (height - 4) / dim.height;

    img_width  = dim.width * scalex;
    img_height = dim.height * scaley;

    DBG ("scale x/y: %.3f/%.3f, dim w/h: %d/%d, scaled w/h: %.1f/%.1f",
         scalex, scaley, dim.width, dim.height, scalex*dim.width, scaley*dim.height);

    layoutx = (actual_width - img_width) / 2;
    layouty = (actual_height - img_height) / 2;
    cairo_translate (cr, layoutx, layouty);

    cairo_save (cr);

    cairo_scale (cr, scalex, scaley);
    rsvg_handle_render_cairo (handle, cr);

    cairo_restore (cr);

    DBG ("actual width/height: %d/%d; w/h: %d/%d; img w/h: %.1f/%.1f; markers: %d, max markers: %d",
         actual_width, actual_height, width, height, img_width, img_height,
         variant_markers_count, max_variant_markers_count);
    DBG ("layout x/y: %.1f/%.1f", layoutx, layouty);

    rsvg_handle_close (handle, NULL);
    g_object_unref (handle);
}
