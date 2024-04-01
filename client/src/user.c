#include "uchat-client.h"

static GtkWidget *user_window;
static GtkWidget* add_new_chat_when_no_chats;
GtkWidget *scrollable_window = NULL;
GtkWidget *scrollable_window2 = NULL;
// static GtkWidget *user_info_box;
typedef struct {
    GtkWidget *entry;
    // Add any other data needed by the callback function
} CallbackData;

static void wrap_text(char *text) {
    int len = strlen(text);
    int i, line_length = 0;

    for (i = 0; i < len; i++) {
        // Check if adding the current character will exceed the maximum line length
        if (line_length >= MAX_LINE_LENGTH) {
            // Insert a newline character
            memmove(&text[i + 1], &text[i], len - i);
            text[i] = '\n';
            // Reset the line length counter
            line_length = 0;
            // Increment the length of the text
            len++;
        }
        // Increment the line length
        line_length++;
    }
}

static void on_window_realize(GtkWidget *widget, gpointer data) {
    // Получаем пролистываемое окно
    GtkWidget *scrollable_window = GTK_WIDGET(data);

    // Получаем вертикальный регулятор прокрутки
    GtkAdjustment *v_adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollable_window));

    // Устанавливаем его на максимальное значение, чтобы прокрутить вниз
    gtk_adjustment_set_value(v_adjustment, gtk_adjustment_get_upper(v_adjustment) - gtk_adjustment_get_page_size(v_adjustment));
    gtk_widget_hide(chat_box);   
}

void on_window_realize_2(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(widget);
}

static void add_chatter_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Add chatter clicked\n");
    
    // Check if chatters array is initialized
    if (chatters == NULL) {
        g_print("Chatters array is not initialized\n");
        return;
    }
    
    // Создаем новый элемент структуры t_chatter_s
    t_chatter_s new_chatter = {
        .name = "New Name",
        .surname = "New Surname",
        .username = "new_username",
        .lastmsg = "No messages yet",
        .avatar = NULL
    };
    
    // Find the first available slot in the chatters array

    if(chatters_count + 1 < MAX_CHATTERS) {
        chatters[chatters_count] = new_chatter;
        chatters_count++;
        refresh_scrollable_window(scrollable_window);
            gtk_widget_show(scrollable_window);
            gtk_widget_hide(add_new_chat_when_no_chats);

        if (!is_chatters_empty()) {
            // Если массив больше не пуст, обновляем текст метки empty_chat
            gtk_label_set_text(GTK_LABEL(empty_chat), "[ Select a chat to start chatting ]");
        }
        return;
    }
    // int i;
    // for (i = 0; i < MAX_CHATTERS; i++) {
    //     if (chatters[i].name == NULL) {
    //         chatters[i] = new_chatter;
    //         chatters_count++;
    //         refresh_scrollable_window(scrollable_window);
    //                 // Иначе, показываем прокручиваемое окно для сообщений
    //         return;
    //     }
    // }
    
    // If no available slot is found
    g_print("Chatter limit reached\n");
}

static void add_message_button_clicked(GtkWidget *widget, gpointer user_data) {
    g_print("Add message clicked\n");
    CallbackData *data = (CallbackData *)user_data;
    char *text = gtk_entry_get_text(GTK_ENTRY(data->entry));

    int i = 0, j = 0;

    // Delete spaces from the beginning
    while (mx_isspace(text[i])) i++;
    while (text[i]) text[j++] = text[i++];
    text[j] = '\0';

    if(mx_strcmp(text, "") == 0) return;
    // const gchar *text = gtk_entry_get_text(GTK_ENTRY(wi));
    // Check if chatters array is initialized
    if (messages == NULL) {
        g_print("Messages array is not initialized\n");
        return;
    }

    wrap_text(text);
    
    // Создаем новый элемент структуры
    t_message_s new_mes = {
        .text = mx_strdup(text),
        .is_user = TRUE
    };

    g_print("%d\n", messages_count[selected_user.index]);
    // Find the first available slot in the chatters array
    if(messages_count[selected_user.index] + 1 < MAX_MESSAGES) {
        messages[selected_user.index][messages_count[selected_user.index]] = new_mes;
        messages_count[selected_user.index]++;
        refresh_scrollable_window2(scrollable_window2);
        gtk_widget_show(scrollable_window2);
        chatters[selected_user.index].lastmsg = format_last_msg((char *)text);//strdup(text);
        refresh_scrollable_window(scrollable_window);
        gtk_widget_show(scrollable_window); 
        // selected_user.box = 
        // gtk_widget_override_background_color(selected_user.box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY, LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
        // user_box_clicked(selected_user.box, NULL, selected_user.index);
        gtk_entry_set_text(GTK_ENTRY(data->entry), "");
        // user_box_clicked()

        // if (!is_message_empty()) {
        //     // Если массив больше не пуст, обновляем текст метки empty_chat
        //     // gtk_label_set_text(GTK_LABEL(empty_chat), "[ Select a chat to start chatting ]");
        // }
        return;
    }
    g_print("Messages limit reached\n");
}

static gboolean on_entry_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    // Проверяем, нажата ли клавиша Enter
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        // Получаем указатель на данные, переданные при подключении сигнала
        CallbackData *callback_data = (CallbackData *)user_data;
        
        // Вызываем функцию-обработчик, которая вызывается при нажатии кнопки отправки сообщения
        add_message_button_clicked(GTK_WIDGET(callback_data->entry), callback_data);
        
        // Возвращаем TRUE, чтобы предотвратить дальнейшую обработку события
        return TRUE;
    }

    // Возвращаем FALSE, чтобы разрешить дальнейшую обработку события
    return FALSE;
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

void refresh_scrollable_window(GtkWidget *scrollable_window) {
    // Очищаем содержимое скроллабельного окна
    gtk_container_foreach(GTK_CONTAINER(scrollable_window), (GtkCallback)gtk_widget_destroy, NULL);
    
    // Перерисовываем содержимое скроллабельного окна
    user_populate_scrollable_window(scrollable_window);
 
    // Перерисовываем окно
    gtk_widget_show_all(scrollable_window);
}

void refresh_scrollable_window2(GtkWidget *scrollable_window) {
    // Очищаем содержимое скроллабельного окна
    gtk_container_foreach(GTK_CONTAINER(scrollable_window), (GtkCallback)gtk_widget_destroy, NULL);
    
    // Перерисовываем содержимое скроллабельного окна
    message_populate_scrollable_window(scrollable_window);
 
    // Перерисовываем окно
    gtk_widget_show_all(scrollable_window);
}

void show_user_window() {
    gtk_widget_show_all(user_window);
}

void draw_user_info_box(GtkWidget *user_info_box) {
    // user_info_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    // gtk_widget_override_background_color(user_info_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY,LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
    // set_widget_height(user_info_box, 70);
    GdkPixbuf *pixbuf = file_to_pixbuf(default_img);
    GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(pixbuf);
    // prev_pixbuf = resize_img(prev_pixbuf, 150, 150);

    GtkWidget *image = gtk_drawing_area_new();
    gtk_widget_set_halign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(image), gdk_pixbuf_get_width(GDK_PIXBUF(prev_pixbuf)), gdk_pixbuf_get_height(GDK_PIXBUF(prev_pixbuf)));
    g_signal_connect(G_OBJECT(image), "draw", G_CALLBACK(draw_image), prev_pixbuf);
    gtk_box_pack_start(GTK_BOX(user_info_box), image, FALSE, FALSE, 15);

    GtkWidget *name_label = gtk_label_new((chatters == NULL || selected_user.index == -1) ? " " : chatters[selected_user.index].name);
    gtk_widget_set_name(name_label, "chatter-name");
    GtkWidget *surname_label = gtk_label_new((chatters == NULL || selected_user.index == -1) ? " " : chatters[selected_user.index].surname);
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
}

static void toggle(GtkWidget *widget, GtkWidget *element) {
    gboolean visible = gtk_widget_get_visible(element);
    gtk_widget_set_visible(element, !visible);
}

static void clicked_side(GtkWidget *widget, gpointer data){
    g_print("side clicked\n");
}

static void clicked_settings(GtkWidget *widget, gpointer data){
    g_print("settings clicked\n");
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
    settings_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    chats_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Set the background colors for each box container
    //gtk_widget_override_background_color(side_bar_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){DARK_GRAY, DARK_GRAY, DARK_GRAY, 1.0}); 
    gtk_widget_override_background_color(chats_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHT_GRAY, LIGHT_GRAY, LIGHT_GRAY, 1.0});
    gtk_widget_override_background_color(chat_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){DARK_GRAY, DARK_GRAY, DARK_GRAY, 1.0});
    // Set the widths of the box containers
    //gtk_widget_set_size_request(side_bar_box, 75, -1); // 75 pixels width
    gtk_widget_set_size_request(chats_box, 350, -1); // 350 pixels width
    // Third box will take the remaining width

    //sidebar
    // GtkWidget *sidebar = gtk_event_box_new();
    // gtk_widget_set_size_request(sidebar, 200, -1);
    // gtk_widget_set_visible(sidebar, FALSE);

    GtkWidget *side_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_override_background_color(side_box, GTK_STATE_FLAG_NORMAL, NORM_CVET);
    gtk_widget_set_size_request(side_box, 75, -1);
    //gtk_widget_set_visible(sidebar, FALSE);
    
    //endsidebar

    // GtkWidget* add = gtk_button_new();
    // gtk_widget_set_valign(GTK_WIDGET(add), GTK_ALIGN_CENTER);
    // gtk_button_set_relief(GTK_BUTTON(add), GTK_RELIEF_NONE);
    // gtk_container_set_border_width(GTK_CONTAINER(add), 0);
    // // gtk_widget_set_halign(GTK_WIDGET(side_img), GTK_ALIGN_CENTER);
    // gtk_widget_set_size_request(GTK_WIDGET(add), 64, 64);
    // gtk_widget_set_name(GTK_WIDGET(add), "add");
    // g_signal_connect(G_OBJECT(add), "realize", G_CALLBACK(on_window_realize_2), NULL);

    // GtkWidget* delete = gtk_button_new();
    // gtk_widget_set_valign(GTK_WIDGET(delete), GTK_ALIGN_CENTER);
    // gtk_button_set_relief(GTK_BUTTON(delete), GTK_RELIEF_NONE);
    // gtk_container_set_border_width(GTK_CONTAINER(delete), 0);
    // // gtk_widget_set_halign(GTK_WIDGET(side_img), GTK_ALIGN_CENTER);
    // gtk_widget_set_size_request(GTK_WIDGET(delete), 64, 64);
    // gtk_widget_set_name(GTK_WIDGET(delete), "delete");
    // g_signal_connect(G_OBJECT(delete), "realize", G_CALLBACK(on_window_realize_2), NULL);

    GtkWidget* anekdot = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(anekdot), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(anekdot), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(anekdot), 0);
    // gtk_widget_set_halign(GTK_WIDGET(side_img), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(anekdot), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(anekdot), "anekdot");
    g_signal_connect(G_OBJECT(anekdot), "realize", G_CALLBACK(on_window_realize_2), NULL);

    GtkWidget* account = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(account), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(account), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(account), 0);
    // gtk_widget_set_halign(GTK_WIDGET(side_img), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(account), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(account), "account");
    g_signal_connect(G_OBJECT(account), "realize", G_CALLBACK(on_window_realize_2), NULL);

    GtkWidget* settings_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(settings_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(settings_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(settings_img), 0);
    gtk_widget_set_size_request(GTK_WIDGET(settings_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(settings_img), "settings");
    g_signal_connect(G_OBJECT(settings_img), "clicked", G_CALLBACK(clicked_settings), NULL);
    g_signal_connect(G_OBJECT(settings_img), "realize", G_CALLBACK(on_window_realize_2), NULL);

    GtkWidget* side_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(side_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(side_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(side_img), 0);
    // gtk_widget_set_halign(GTK_WIDGET(side_img), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(side_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(side_img), "sidebar");
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), settings_img);
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), anekdot);
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), account);
    // g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), add);
    // g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), delete);
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(clicked_side), NULL);

    GtkWidget* logout_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(logout_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(logout_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(logout_img), 0);
    // gtk_widget_set_halign(GTK_WIDGET(side_img), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(logout_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(logout_img), "logout");
    g_signal_connect(G_OBJECT(logout_img), "clicked", G_CALLBACK(clicked_side), NULL);

    
    gtk_box_pack_start(GTK_BOX(side_box), side_img, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), settings_img, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), account, FALSE, FALSE, 0);
    // gtk_box_pack_start(GTK_BOX(side_box), add, FALSE, FALSE, 0);
    // gtk_box_pack_start(GTK_BOX(side_box), delete, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), anekdot, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(side_box), logout_img, FALSE, FALSE, 0);

    // Create a search bar
    GtkWidget *search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Search...");

    // Pack the search bar into a vertical box container
    GtkWidget *search_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(search_box), search_entry, FALSE, FALSE, 0);

    // Create the button
    GtkWidget *search_button = gtk_button_new_with_label("[ Find a user ]");
    gtk_widget_set_size_request(search_button, 25, 35);
    gtk_widget_set_name(GTK_WIDGET(search_button), "search_button");

    // Pack the search box and margin into a vertical box container
    gtk_box_pack_start(GTK_BOX(search_box), search_button, FALSE, FALSE, 0);

    // Pack the search box into the chats_box
    gtk_box_pack_start(GTK_BOX(chats_box), search_box, FALSE, FALSE, 0);
    scrollable_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollable_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    user_populate_scrollable_window(scrollable_window);
    gtk_box_pack_start(GTK_BOX(chats_box), scrollable_window, TRUE, TRUE, 0);

    add_new_chat_when_no_chats = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(add_new_chat_when_no_chats), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(add_new_chat_when_no_chats), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(add_new_chat_when_no_chats), 0);
    gtk_widget_set_size_request(GTK_WIDGET(add_new_chat_when_no_chats), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(add_new_chat_when_no_chats), "add_new_chat_when_no_chats");
    g_signal_connect(G_OBJECT(add_new_chat_when_no_chats), "clicked", G_CALLBACK(add_chatter_button_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(chats_box), add_new_chat_when_no_chats, FALSE, FALSE, 5);
    
    if (chatters == NULL) {
        gtk_widget_show(add_new_chat_when_no_chats);
        gtk_widget_hide(scrollable_window);
    } else {
        // Иначе, показываем прокручиваемое окно для сообщений
        gtk_widget_show(scrollable_window);
        gtk_widget_hide(add_new_chat_when_no_chats);
    }
    user_info_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_override_background_color(user_info_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY,LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
    set_widget_height(user_info_box, 70);
    draw_user_info_box(user_info_box);
    gtk_box_pack_start(GTK_BOX(chat_box), user_info_box, FALSE, FALSE, 0);

    scrollable_window2 = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollable_window2),
                                GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    message_populate_scrollable_window(scrollable_window2);
    g_signal_connect(G_OBJECT(user_window), "realize", G_CALLBACK(on_window_realize), scrollable_window2);
    gtk_box_pack_start(GTK_BOX(chat_box), scrollable_window2, TRUE, TRUE, 0);

    GtkWidget *text_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(text_entry), "Write a message...");

    GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(text_box), text_entry, TRUE, TRUE, 0);

    GtkWidget *send_button = gtk_button_new_with_label("[ Send ]");
    gtk_widget_set_size_request(send_button, 25, 25);
    CallbackData *callback_data = g_slice_new(CallbackData);
    callback_data->entry = text_entry;
    g_signal_connect(G_OBJECT(send_button), "clicked", G_CALLBACK(add_message_button_clicked), callback_data);
    g_signal_connect(G_OBJECT(text_entry), "key-press-event", G_CALLBACK(on_entry_key_press), callback_data);
    
    gtk_box_pack_start(GTK_BOX(text_box), send_button, FALSE, FALSE, 0);

    gtk_box_pack_end(GTK_BOX(chat_box),text_box, FALSE, FALSE, 0);

    empty_chat = (!is_chatters_empty()) ? gtk_label_new("[ Select a chat to start chatting ]") : gtk_label_new("[ Add your first chat! ]");
    gtk_widget_set_name(GTK_WIDGET(empty_chat), "empty-chat");
    gtk_widget_override_background_color(empty_chat, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){DARK_GRAY, DARK_GRAY, DARK_GRAY, 1.0}); 
    // if (selected_user.index == -1) {
        // gtk_widget_show(empty_chat);
    // gtk_widget_hide(chat_box);
    // } else {
    //     Иначе, показываем прокручиваемое окно для сообщений
    //     gtk_widget_show(chat_box);
    //     gtk_widget_hide(empty_chat);
    // }

    // Pack the box containers into the main horizontal box container
    //gtk_box_pack_start(GTK_BOX(hbox_main), side_bar_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), side_box, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), chats_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), chat_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), empty_chat, TRUE, TRUE, 0); // Allow it to expand to fill remaining space
    // gtk_widget_hide(chat_box);
    // g_signal_connect(G_OBJECT(chat_box), "realize", G_CALLBACK(on_chat_box_realize), NULL);
}

