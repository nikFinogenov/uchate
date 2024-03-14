#include "uchat-client.h"

int main(int argc, char *argv[]) {
    // GtkCssProvider *cssProvider = gtk_css_provider_new();
    // gtk_css_provider_load_from_path(cssProvider, "client/style.css", NULL);
    // gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
    // GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_init(&argc, &argv);

    // Get screen width and height
    GdkScreen *screen = gdk_screen_get_default();
    gint screen_width = gdk_screen_get_width(screen);
    gint screen_height = gdk_screen_get_height(screen);


    // Draw windows
    draw_login();
    draw_singup();
    

    // // Set background color for both windows
    // GdkRGBA color;
    // gdk_rgba_parse(&color, "#171717");
    // gtk_widget_override_background_color(login_window, GTK_STATE_FLAG_NORMAL, &color);
    // gtk_widget_override_background_color(signup_window, GTK_STATE_FLAG_NORMAL, &color);

    show_login();
    // 

    gtk_main();

    return 0;
}
