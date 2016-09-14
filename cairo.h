#ifndef _sample_CAIRO_H_
#define _sample_CAIRO_H_

#include <glib.h>
#include <glib/gprintf.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>
#include <librsvg/rsvg.h>
#include <pango/pangocairo.h>

void sample_cairo_draw_flag(cairo_t *cr,
                            const gchar *flag_name,
                            gint actual_width,
                            gint actual_height,
                            gint width,
                            gint height);

#endif

