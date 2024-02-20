#include <gtk/gtk.h>

static void login_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Login button clicked\n");
}
static void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}
GdkPixbuf *scale_image(const gchar *filename, int new_width, int new_height) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);

    if (pixbuf == NULL) {
        g_print("Error loading image.\n");
        return NULL;
    }

    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);

    g_object_unref(pixbuf);

    return scaled_pixbuf;
}
gboolean toggle_visibility(GtkWidget *button, GtkWidget *entry) {
    gboolean is_visible = gtk_entry_get_visibility(GTK_ENTRY(entry));
    gtk_entry_set_visibility(GTK_ENTRY(entry), !is_visible);

    const gchar *icon_name = is_visible ? "vse/eye-close.png" : "vse/eye-open.png";

    GdkPixbuf *image_buf = scale_image(icon_name, 25, 25);
    GtkWidget *image = gtk_image_new_from_pixbuf(image_buf);

    gtk_button_set_image(GTK_BUTTON(button), image);

    return TRUE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "McOk Chat");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GdkPixbuf *scaled_pixbuf = scale_image("vse/mcok.png", 111, 33);
    GtkWidget *img = gtk_image_new_from_pixbuf(scaled_pixbuf);

    GtkWidget *username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Username");

    GtkWidget *pass_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);

    GtkWidget* toggleButton = gtk_toggle_button_new();
    g_signal_connect(G_OBJECT(toggleButton), "toggled", G_CALLBACK(toggle_visibility), password_entry);
    GdkPixbuf *initialImage_buf = scale_image("vse/eye-close.png", 25, 25);
    GtkWidget *initialImage_icon = gtk_image_new_from_pixbuf(initialImage_buf);
    gtk_button_set_image(GTK_BUTTON(toggleButton), initialImage_icon);

    gtk_box_pack_start(GTK_BOX(pass_box), password_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(pass_box), toggleButton, TRUE, TRUE, 0);    
    gtk_widget_set_halign(toggleButton, GTK_ALIGN_END);

    GtkWidget *login_button = gtk_button_new();
    GtkWidget *image_login = gtk_image_new_from_file ("vse/arrow.png");
    gtk_button_set_image(GTK_BUTTON(login_button), image_login);
    g_signal_connect(login_button, "clicked", G_CALLBACK(login_button_clicked), NULL);
    
    GtkWidget *entry_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), username_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(entry_box), pass_box, TRUE, TRUE, 5);

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(login_button, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(button_box), login_button, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), img, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, TRUE, TRUE, 0);

    GdkRGBA color1;
    gdk_rgba_parse(&color1, "#171717");
    gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &color1);
    
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
