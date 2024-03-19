#include "uchat-client.h"

static GtkWidget *user_window;

void on_window_realize(GtkWidget *widget, gpointer data) {
    // Получаем пролистываемое окно
    GtkWidget *scrollable_window = GTK_WIDGET(data);

    // Получаем вертикальный регулятор прокрутки
    GtkAdjustment *v_adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollable_window));

    // Устанавливаем его на максимальное значение, чтобы прокрутить вниз
    gtk_adjustment_set_value(v_adjustment, gtk_adjustment_get_upper(v_adjustment) - gtk_adjustment_get_page_size(v_adjustment));
    gtk_widget_hide(chat_box);
    // gtk_widget_hide(scrollable_window);
    // g_print("%d\n", selected_user.index);
    // Проверяем, выбран ли пользователь
}
static void logout_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Logout clicked\n");
}

static void settings_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Settings clicked\n");
}

static void user_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("User clicked\n");
}
static void message_search_clicked(GtkWidget *widget, gpointer data) {
    g_print("Message search clicked\n");
}

void show_user_window() {
    gtk_widget_show_all(user_window);   
}

void draw_user_window() {
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "client/style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
    GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    user_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(user_window), MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    g_signal_connect(user_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create the main horizontal box container
    GtkWidget *hbox_main = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(user_window), hbox_main);

    // Create three box containers
    GtkWidget *settings_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *chats_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Set the background colors for each box container
    gtk_widget_override_background_color(settings_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){DARK_GRAY, DARK_GRAY, DARK_GRAY, 1.0}); 
    gtk_widget_override_background_color(chats_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHT_GRAY, LIGHT_GRAY, LIGHT_GRAY, 1.0});
    gtk_widget_override_background_color(chat_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){DARK_GRAY, DARK_GRAY, DARK_GRAY, 1.0});

    // Set the widths of the box containers
    gtk_widget_set_size_request(settings_box, 75, -1); // 75 pixels width
    gtk_widget_set_size_request(chats_box, 350, -1); // 350 pixels width
    // Third box will take the remaining width


    GtkWidget* settings_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(settings_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(settings_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(settings_img), 0);
    // gtk_widget_set_halign(GTK_WIDGET(settings_img), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(settings_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(settings_img), "settings");
    g_signal_connect(G_OBJECT(settings_img), "clicked", G_CALLBACK(settings_button_clicked), NULL);

    GtkWidget* user_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(user_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(user_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(user_img), 0);
    // gtk_widget_set_halign(GTK_WIDGET(settings_img), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(user_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(user_img), "user");
    g_signal_connect(G_OBJECT(user_img), "clicked", G_CALLBACK(user_button_clicked), NULL);

    GtkWidget* logout_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(logout_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(logout_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(logout_img), 0);
    // gtk_widget_set_halign(GTK_WIDGET(settings_img), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(logout_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(logout_img), "logout");
    g_signal_connect(G_OBJECT(logout_img), "clicked", G_CALLBACK(logout_button_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(settings_box), settings_img, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(settings_box), user_img, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(settings_box), logout_img, FALSE, FALSE, 0);

    // Create a search bar
    GtkWidget *search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Search...");

    // Pack the search bar into a vertical box container
    GtkWidget *search_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(search_box), search_entry, FALSE, FALSE, 0);

    // Create the button
    GtkWidget *search_button = gtk_button_new_with_label("[ Find a user ]");
    gtk_widget_set_size_request(search_button, 25, 35);

    // Pack the search box and margin into a vertical box container
    gtk_box_pack_start(GTK_BOX(search_box), search_button, FALSE, FALSE, 0);

    // Pack the search box into the chats_box
    gtk_box_pack_start(GTK_BOX(chats_box), search_box, FALSE, FALSE, 0);
    GtkWidget *scrollable_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollable_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    user_populate_scrollable_window(scrollable_window);
    gtk_box_pack_start(GTK_BOX(chats_box), scrollable_window, TRUE, TRUE, 0);
    // gtk_container_add(GTK_CONTAINER(window), scrollable_window);
    // int users_count = 0;
    // for(int i = 0; i < users_count; i++) {
        // show_user();
    // }

    // GtkWidget *pn = create_penis();
    // gtk_box_pack_start(GTK_BOX(chat_box), pn, FALSE, FALSE, 0);


    // GtkWidget *user_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    GtkWidget *user_info_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_override_background_color(user_info_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY,LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
    set_widget_height(user_info_box, 70);
    //avatar
    // const char *input_image_file = "client/img/simple.png";
    // const char* name = "Nik";
    // const char* surname = "Fin";
    GdkPixbuf *pixbuf = file_to_pixbuf(default_img);
    GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(pixbuf);
    // prev_pixbuf = resize_img(prev_pixbuf, 150, 150);

    GtkWidget *image = gtk_drawing_area_new();
    gtk_widget_set_halign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(image), gdk_pixbuf_get_width(GDK_PIXBUF(prev_pixbuf)), gdk_pixbuf_get_height(GDK_PIXBUF(prev_pixbuf)));
    g_signal_connect(G_OBJECT(image), "draw", G_CALLBACK(draw_image), prev_pixbuf);
    gtk_box_pack_start(GTK_BOX(user_info_box), image, FALSE, FALSE, 15);

    GtkWidget *name_label = gtk_label_new(user.name);
    gtk_widget_set_name(name_label, "chatter-name");
    GtkWidget *surname_label = gtk_label_new(user.surname);
    gtk_widget_set_name(surname_label, "chatter-surname");
    gtk_box_pack_start(GTK_BOX(user_info_box), name_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(user_info_box), surname_label, FALSE, FALSE, 5);

    GtkWidget *message_search_entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    set_widget_height(message_search_entry_box, 20);
    GtkWidget *message_search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(message_search_entry), "Search message...");
    gtk_widget_set_valign(GTK_WIDGET(message_search_entry_box), GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(message_search_entry_box), message_search_entry, FALSE, FALSE, 5);

    GtkWidget* message_search_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(message_search_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(message_search_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(message_search_img), 0);
    gtk_widget_set_size_request(GTK_WIDGET(message_search_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(message_search_img), "message-search-img");
    g_signal_connect(G_OBJECT(message_search_img), "clicked", G_CALLBACK(message_search_clicked), NULL);

    gtk_box_pack_end(GTK_BOX(user_info_box), message_search_img, FALSE, FALSE,0);
    gtk_box_pack_end(GTK_BOX(user_info_box), message_search_entry_box, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(chat_box), user_info_box, FALSE, FALSE, 0);


    GtkWidget *scrollable_window2 = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollable_window2),
                                GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    message_populate_scrollable_window(scrollable_window2);
    g_signal_connect(G_OBJECT(user_window), "realize", G_CALLBACK(on_window_realize), scrollable_window2);
    // gtk_widget_hide(scrollable_window2);
    gtk_box_pack_start(GTK_BOX(chat_box), scrollable_window2, TRUE, TRUE, 0);

    GtkWidget *text_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(text_entry), "Write a message...");

    GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(text_box), text_entry, TRUE, TRUE, 0);

    GtkWidget *send_button = gtk_button_new_with_label("[ Send ]");
    gtk_widget_set_size_request(send_button, 25, 25);

    gtk_box_pack_start(GTK_BOX(text_box), send_button, FALSE, FALSE, 0);

    gtk_box_pack_end(GTK_BOX(chat_box),text_box, FALSE, FALSE, 0);

    empty_chat = gtk_label_new("[ Select a chat to start chatting ]");
    gtk_widget_set_name(GTK_WIDGET(empty_chat), "empty-chat");
    // gtk_widget_set_halign(empty_chat, GTK_ALIGN_CENTER);
    // gtk_widget_set_valign(empty_chat, GTK_ALIGN_CENTER);
    gtk_widget_override_background_color(empty_chat, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){DARK_GRAY, DARK_GRAY, DARK_GRAY, 1.0}); 
    gtk_box_pack_end(GTK_BOX(hbox_main), empty_chat, TRUE, TRUE, 0);

    // Pack the box containers into the main horizontal box container
    gtk_box_pack_start(GTK_BOX(hbox_main), settings_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), chats_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), chat_box, TRUE, TRUE, 0); // Allow it to expand to fill remaining space
}

