#include "uchat-client.h"
#include <math.h>

const char *input_image_file = "client/img/simple.png"; // Input image file path

GdkPixbuf *mx_size_image_down(GdkPixbuf *pixbuf, int w, int h) {
    int width = gdk_pixbuf_get_width(GDK_PIXBUF(pixbuf));
    int height = gdk_pixbuf_get_height(GDK_PIXBUF(pixbuf));

    if (width <= w && height <= h)
        return pixbuf;
    else {
        int source_width = width;
        int source_height = height;
        if (width > height) {
            width = w;
            height = (w * source_height) / source_width;
            if (height > h) {
                source_width = width;
                source_height = height;
                height = h;
                width = (h * source_width) / source_height;
            }
        }
        else {
            height = h;
            width = (h * source_width) / source_height;
            if (width > w) {
                source_width = width;
                source_height = height;
                width = w;
                height = (w * source_height) / source_width;
            }
        }
    }

    GdkPixbuf *result = gdk_pixbuf_scale_simple(GDK_PIXBUF(pixbuf), 
        width, height, GDK_INTERP_BILINEAR);
    g_object_unref(pixbuf);
    return result;
}
GdkPixbuf *mx_create_pixbuf(const gchar *filename) {
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    
    if (!pixbuf) {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }
    
    return pixbuf;
}


void draw_image(GtkWidget *widget, cairo_t *cr, GdkPixbuf *data) {
    int img_w, img_h;
    GtkAllocation alloc;
    gtk_widget_get_allocation(GTK_WIDGET(widget), &alloc);
    img_w = alloc.width;
    img_h = alloc.height;

    double x = 0,
        y = 0,
        width = img_w,
        height = img_h,
        aspect = 1.0,                       /* aspect ratio */
        corner_radius = 12;                 /* and corner curvature radius */
    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    cairo_new_sub_path (cr);
    cairo_arc (cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc (cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc (cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc (cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path (cr);

    gdk_cairo_set_source_pixbuf(cr, data, 0, 0);
    cairo_fill(cr);
}

gboolean mx_draw_event_image_avatar(GtkWidget *widget, cairo_t *cr, GdkPixbuf **img_data) {
    int img_w, img_h;
    gtk_widget_get_size_request(GTK_WIDGET(widget), &img_w, &img_h);
    GdkPixbuf *pixbuf = gdk_pixbuf_scale_simple(GDK_PIXBUF(*img_data), 
        img_w, img_h, GDK_INTERP_BILINEAR);
    gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
    g_object_unref(G_OBJECT(pixbuf));

    double x = 0,
        y = 0,
        width = img_w,
        height = img_h,
        aspect = 1.0,                       /* aspect ratio */
        corner_radius = height / 2.7;       /* and corner curvature radius */
    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    cairo_new_sub_path (cr);
    cairo_arc (cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc (cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc (cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc (cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path (cr);

    cairo_fill(cr);
    return FALSE;
}

GtkWidget *create_user_box(char* name, char* surname, char* tag) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(box, 10);

    //avatar
    GdkPixbuf *pixbuf = mx_create_pixbuf(input_image_file);
    GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(pixbuf);
    prev_pixbuf = mx_size_image_down(prev_pixbuf, 500, 350);

    GtkWidget *image = gtk_drawing_area_new();
    gtk_widget_set_halign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(image), gdk_pixbuf_get_width(GDK_PIXBUF(prev_pixbuf)), gdk_pixbuf_get_height(GDK_PIXBUF(prev_pixbuf)));
    g_signal_connect(G_OBJECT(image), "draw", G_CALLBACK(draw_image), prev_pixbuf);
    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);
    // mx_draw_event_image_avatar

    // Create user info box
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // User tag
    GtkWidget *tag_label = gtk_label_new(tag);
    gtk_widget_set_name(tag_label, "user-tag");

    // User name and surname
    GtkWidget *name_label = gtk_label_new(name);
    GtkWidget *surname_label = gtk_label_new(surname);
    gtk_widget_set_name(name_label, "user-name");
    gtk_widget_set_name(surname_label, "user-surname");

    gtk_box_pack_start(GTK_BOX(info_box), tag_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(info_box), name_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(info_box), surname_label, FALSE, FALSE, 0);

    // Pack avatar and info_box into the user box
    // gtk_box_pack_start(GTK_BOX(box), avatar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), info_box, TRUE, TRUE, 0);

    return box;
}

void user_populate_scrollable_window(GtkWidget *scrollable_window) {
    GtkWidget *user_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), user_list);

    // GtkWidget *user_box1 = create_user_box("John", "Doe", "@john_doe");
    // GtkWidget *user_box2 = create_user_box("Jane", "Smith", "@jane_smith");
    // gtk_box_pack_start(GTK_BOX(user_list), user_box1, FALSE, FALSE, 0);
    // gtk_box_pack_start(GTK_BOX(user_list), user_box2, FALSE, FALSE, 0);


    for (int i = 0; i < 15; ++i) {
        char username[20];
        sprintf(username, "User%d", i+1);

        GtkWidget *user_box = create_user_box(username, "Doe", "@username");
        gtk_box_pack_start(GTK_BOX(user_list), user_box, FALSE, FALSE, 0);
    }
}

GtkWidget *create_message_box(char* text, bool is_user) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(box, 10);

    // Create user info box
    GtkWidget *message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *text_label = gtk_label_new(text);
    gtk_widget_set_name(text_label, "message-text");
    gtk_box_pack_start(GTK_BOX(message_box), text_label, FALSE, FALSE, 0);

    if(is_user) {
        gtk_box_pack_end(GTK_BOX(box), message_box, FALSE, TRUE, 0);
    }
    else gtk_box_pack_start(GTK_BOX(box), message_box, FALSE, TRUE, 0);
    return box;
}

void message_populate_scrollable_window(GtkWidget *scrollable_window) {
    GtkWidget *mess_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), mess_list);

    GtkWidget *mess_box1 = create_message_box("John wht mn skdaksjdnjkasd", true);
    GtkWidget *mess_box2 = create_message_box("aksndjkansdkjnaskjdnkajsndkjansjkdnjaksdnjka", false);
    GtkWidget *mess_box3 = create_message_box("aksndjkansdkjnaskjdnkajsndkjansjkdnjaksdnjka", false);
    GtkWidget *mess_box4 = create_message_box("jkasndj", false);
    GtkWidget *mess_box5 = create_message_box("qopwkeopkcmsiqmwd", false);
    GtkWidget *mess_box6 = create_message_box("askjdjasndjknasjkd", true);
    GtkWidget *mess_box7 = create_message_box("asdkaslkcmklasnmjknjsdbjansjnxkasnxkamskcnkjfbnamkmcijdnfjknasne", false);
    gtk_box_pack_start(GTK_BOX(mess_list), mess_box1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mess_list), mess_box2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mess_list), mess_box3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mess_list), mess_box4, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mess_list), mess_box5, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mess_list), mess_box6, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mess_list), mess_box7, FALSE, FALSE, 0);
}
