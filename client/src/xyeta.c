#include <uchat-client.h>
#include <math.h>

static gboolean draw_penis_cb(GtkWidget *widget, cairo_t *cr, gpointer data) {
    cairo_set_line_width(cr, 2);
    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0); // Red color

    cairo_arc(cr, 35, 93, 15, 0, 2 * M_PI);
    cairo_stroke(cr);

    cairo_arc(cr, 65, 93, 15, 0, 2 * M_PI);
    cairo_stroke(cr);

    cairo_move_to(cr, 40, 30);
    cairo_line_to(cr, 40, 80);
    cairo_move_to(cr, 60, 30); // Remove the bottom line
    cairo_line_to(cr, 60, 80);
    cairo_stroke(cr);

    cairo_arc(cr, 50, 30, 10, 0, 2 * M_PI);
    cairo_stroke(cr);

    cairo_move_to(cr, 40, 30);
    cairo_line_to(cr, 60, 30); // Horizontal line
    cairo_move_to(cr, 50, 30);
    cairo_line_to(cr, 50, 20); // Vertical line
    cairo_stroke(cr);

    return FALSE;
}

GtkWidget *create_penis(void) {
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 150, 150);

    g_signal_connect(G_OBJECT(drawing_area), "draw",
                     G_CALLBACK(draw_penis_cb), NULL);

    return drawing_area;
}



