#include "uchat-client.h"


const gchar *image_base64 = "YOUR_BASE64_ENCODED_IMAGE_DATA_HERE";

gboolean draw_ava(GtkWidget *widget, cairo_t *cr, gpointer data) {
    guint width, height;
    GdkPixbuf *pixbuf;
    GdkPixbuf *scaled_pixbuf;

    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);

    // Load the base64 encoded image
    GError *error = NULL;
    gsize length;
    guchar *data = g_base64_decode(image_base64, &length);
    pixbuf = gdk_pixbuf_new_from_data(data, GDK_COLORSPACE_RGB, TRUE, 8, 100, 100, 400, NULL, NULL);

    // Scale the image to fit into the circle
    scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 100, 100, GDK_INTERP_BILINEAR);

    // Draw a circle
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_arc(cr, width / 2, height / 2, MIN(width, height) / 2 - 2, 0, 2 * G_PI);
    cairo_fill_preserve(cr);

    // Paint the image inside the circle
    gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf, (width - 100) / 2, (height - 100) / 2);
    cairo_paint(cr);

    g_object_unref(pixbuf);
    g_object_unref(scaled_pixbuf);
    g_free(data);

    return FALSE;
}

GtkWidget *create_user_box(char* name, char* surname, char* tag) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(box, 10);

    // Create circle avatar
    GtkWidget *avatar = gtk_button_new();
    GtkWidget *avatar_image = gtk_image_new_from_icon_name("avatar-default-symbolic", GTK_ICON_SIZE_DIALOG);
    gtk_container_add(GTK_CONTAINER(avatar), avatar_image);
    gtk_widget_set_size_request(avatar, 50, 50);

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
    gtk_box_pack_start(GTK_BOX(box), avatar, FALSE, FALSE, 0);
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
