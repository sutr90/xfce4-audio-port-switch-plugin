#ifndef _sample_CAIRO_H_
#define _sample_CAIRO_H_

#include <glib.h>
#include <cairo.h>

void aps_draw_icon(cairo_t *cr, const gchar *flag_name, gint actual_width, gint actual_height, gint width, gint height);

#endif

