#include "uchat-client.h"

GtkWidget *edit_pop_up;

typedef struct {
    int index;
    GtkWidget *mess_box;
} ButtonPressEventUserData;
// const char *input_image_file = "client/img/simple.png"; // Input image file path
t_selected_s selected_user = {
    .index = -1,
    .box = NULL
};

typedef struct {
    GtkWidget *edit_enter;
    int index_of_msg;
} InfoWidgets;

char* get_random_joke() {
    // Generate a random index between 0 and NUM_JOKES - 1
    srand(time(NULL));
    int random_index = rand() % 30;

    // Return the joke at the random index
    return strdup(jokes[random_index]); // strdup creates a duplicate of the string
}

// GtkWidget *selected_user = NULL;
void refresh_user_box() {
    // Clear the current contents of the chat_box
    gtk_container_foreach(GTK_CONTAINER(user_info_box), (GtkCallback)gtk_widget_destroy, NULL);

    // Refreshed content has been added to the chat_box, so show it
    draw_user_info_box(user_info_box);
    gtk_widget_show_all(user_info_box);
}

static void delete_message(GtkWidget *widget, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    g_print("total gays: %d\n", messages_count[selected_user.index]);
    g_print("gay3 is here-> %d\n", index);
    if (index < 0 || index >= messages_count[selected_user.index]) {
        // Некорректный индекс
        return;
    }

    // Освобождаем память для удаляемого элемента
    free(messages[selected_user.index][index].text);
    free(messages[selected_user.index][index].time);
    // free(messages[index]); // Не нужно освобождать сам указатель на сообщение

    // Сдвигаем элементы влево для замены удаленного элемента
    for (int i = index; i < messages_count[selected_user.index] - 1; ++i) {
        messages[selected_user.index][i] = messages[selected_user.index][i + 1];
    }

    // Уменьшаем количество сообщений
    messages_count[selected_user.index]--;

    // После удаления сообщения, последний элемент массива должен быть обнулен
    // Это важно, чтобы не возникло повторного освобождения памяти в будущем
    memset(&messages[selected_user.index][messages_count[selected_user.index]], 0, sizeof(t_message_s));

    refresh_scrollable_window2(scrollable_window2);
}

static void edit_message_button_clicked (GtkWidget *widget, gpointer user_data) {
    InfoWidgets *data = (InfoWidgets *)user_data;
    const gchar *data_edit = gtk_entry_get_text(GTK_ENTRY(data->edit_enter));

    char *parsed_edit = (char*)data_edit;
    wrap_text(parsed_edit);
    g_print("message wrapped-> ");
    // Edited message can not be bigger then const or less then 1 symbols
    if (strlen(parsed_edit) > DEFAULT_MESSAGE_SIZE || strlen(parsed_edit) == 0) NULL;
    else
        messages[selected_user.index][data->index_of_msg].text = mx_strdup(parsed_edit);
    gtk_widget_destroy(edit_pop_up);
    refresh_scrollable_window2(scrollable_window2);
}

static void edit_message(GtkWidget *widget, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    g_print("total gays: %d\n", messages_count[selected_user.index]);
    g_print("gay4 is here-> %d\n", index);

    if (index < 0 || index >= messages_count[selected_user.index]) {
        // Некорректный индекс
        return;
    }

    // add widget
    
    g_print("Edit message clicked\n");

    // Create a pop-up dialog
    edit_pop_up = gtk_dialog_new_with_buttons("Edit Message", GTK_WINDOW(user_window),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    NULL);

    gtk_window_set_default_size(GTK_WINDOW(edit_pop_up), 500, 150);

    // Add some content to the dialog
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(edit_pop_up));

    GtkWidget *edit_entry =  gtk_entry_new();
    

    // Add the text entry to the scrolled window
    //gtk_container_add(GTK_CONTAINER(scrolled_window), edit_entry);

    gtk_entry_set_placeholder_text(GTK_ENTRY(edit_entry), "Enter edited message...");
    //gtk_container_add(GTK_CONTAINER(content_area), edit_entry);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_entry_set_text(edit_entry, messages[selected_user.index][index].text);
    gtk_box_pack_start(GTK_BOX(box), edit_entry, TRUE, FALSE, 0);
    

    // Edit button
    GtkWidget *edit_button = gtk_button_new_with_label("Edit");
    //gtk_container_add(GTK_CONTAINER(content_area), edit_button);
    gtk_box_pack_start(GTK_BOX(box), edit_button, FALSE, FALSE, 0);

    InfoWidgets *entry = g_new(InfoWidgets, 1);
    entry->edit_enter = edit_entry;
    entry->index_of_msg = index;

    g_signal_connect(edit_button, "clicked", G_CALLBACK(edit_message_button_clicked), entry);
    // No need to connect the edit_message signal here

    gtk_widget_set_margin_top(edit_button, 10);

    // Center the widgets vertically
    /*gtk_widget_set_vexpand(edit_entry, TRUE);
    gtk_widget_set_vexpand(edit_button, TRUE);
    gtk_widget_set_valign(edit_entry, GTK_ALIGN_FILL);
    gtk_widget_set_valign(edit_button, GTK_ALIGN_CENTER);*/
    gtk_container_add(GTK_CONTAINER(content_area), box);
    gtk_widget_set_size_request(box, 400, 100);

    // Center the content area
    gtk_widget_set_halign(content_area, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(content_area, GTK_ALIGN_CENTER);

    gtk_widget_show_all(edit_pop_up);
    
    // Connect signal handler to close the dialog when the close button is clicked
    g_signal_connect_swapped(edit_pop_up, "response", G_CALLBACK(gtk_widget_destroy), edit_pop_up);
    

    //end of widget

    

}

static void delete_chatter(GtkWidget *widget, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    g_print("Nomer - (%d) v okno;\n", index);
    g_print("total pediks: %d\n", chatters_count);
    if (index < 0 || index >= chatters_count) {
        // Некорректный индекс
        return;
    }
    // // Освобождаем память для удаляемого элемента
    free(chatters[index].name);
    free(chatters[index].surname);
    free(chatters[index].username);
    free(chatters[index].lastmsg);
    // free(chatters[index].avatar);
    if (chatters[index].avatar != NULL) {
        g_object_unref(chatters[index].avatar); // Освобождаем память, если есть GdkPixbuf
    }
    for(int i = 0; i < messages_count[index]; i++) {
        free(messages[i][index].text);
        free(messages[i][index].time);
    }
    messages_count[index] = 0;
    
    for (int i = index; i < chatters_count - 1; ++i) {
        chatters[i] = chatters[i + 1];
        messages[i] = messages[i + 1];
        messages_count[i] = messages_count[i + 1];
    }
    chatters_count--;
    memset(&chatters[chatters_count], 0, sizeof(t_chatter_s));

    if (selected_user.index == index) {
        selected_user.box = NULL;
        selected_user.index = -1;
        gtk_widget_hide(chat_box);
        gtk_widget_show(empty_chat);
    }
    else if(selected_user.index > index){
        selected_user.index--;
    }
    
    refresh_scrollable_window(scrollable_window);
}


static GdkPixbuf *resize_img(GdkPixbuf *pixbuf, int w, int h) {
    int width = gdk_pixbuf_get_width(GDK_PIXBUF(pixbuf));
    int height = gdk_pixbuf_get_height(GDK_PIXBUF(pixbuf));

    if (width <= w && height <= h) return pixbuf;
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
static void create_tools_menu(GdkEvent *event, int index) {
    g_print("gay2 is here-> %d\n", index);
    GtkWidget *menu;
    GtkWidget *menu_item;
    
    // Create a new menu
    menu = gtk_menu_new();

    // Create menu items (buttons)
    menu_item = gtk_menu_item_new_with_label("Edit");
    g_signal_connect(menu_item, "activate", G_CALLBACK(edit_message), GINT_TO_POINTER(index));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    menu_item = gtk_menu_item_new_with_label("Delete");
    g_signal_connect(menu_item, "activate", G_CALLBACK(delete_message), GINT_TO_POINTER(index));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    // menu_item = gtk_menu_item_new_with_label("Button 3");
    // gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    menu_item = gtk_menu_item_new_with_label("React");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    // Show the menu
    gtk_widget_show_all(menu);

    // Popup the menu at the event coordinates
    gtk_menu_popup_at_pointer(GTK_MENU(menu), event);
}
static void create_chatter_menu(GdkEvent *event, int index) {
    GtkWidget *menu;
    GtkWidget *menu_item;
    
    // Create a new menu
    menu = gtk_menu_new();

    // Create menu items (buttons)
    menu_item = gtk_menu_item_new_with_label("Delete");
    g_signal_connect(menu_item, "activate", G_CALLBACK(delete_chatter), GINT_TO_POINTER(index));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    // Show the menu
    gtk_widget_show_all(menu);

    // Popup the menu at the event coordinates
    gtk_menu_popup_at_pointer(GTK_MENU(menu), event);
}
gboolean user_box_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    int index = GPOINTER_TO_INT(user_data);
    g_print("clicked -> ");

    if (event->type == GDK_BUTTON_PRESS) {
        if (event->button == GDK_BUTTON_SECONDARY) {
            // Right-click event handling
            create_chatter_menu(event, index);
            return TRUE; // Prevent further processing of the event
        } else if (event->button == GDK_BUTTON_PRIMARY) {
            // Left-click event handling
            if (selected_user.box != NULL && selected_user.index != index) {
                gtk_widget_override_background_color(selected_user.box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHT_GRAY, LIGHT_GRAY, LIGHT_GRAY, 1.0}); 
            }
            // Change background color of the clicked user box to indicate selection
            gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY, LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
            // Update the reference to the currently selected user box
            selected_user.box = widget;
            selected_user.index = index;

            gtk_widget_hide(empty_chat);
            refresh_user_box();
            refresh_scrollable_window2(scrollable_window2);
            gtk_widget_show(scrollable_window2);

            // refresh_
            gtk_widget_show_all(chat_box);
            
            g_print("%d\n", selected_user.index);

            // Returning FALSE allows the event to propagate further
            return FALSE;
        }
    }
    return FALSE;
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    // int index = GPOINTER_TO_INT(((gpointer*)data)[0]);
    int index = GPOINTER_TO_INT(data);
    g_print("gay is here-> %d\n", index);
    GtkWidget *event_box = GTK_WIDGET(widget);
    GtkWidget *mess_box = gtk_bin_get_child(GTK_BIN(event_box));
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_SECONDARY) {
        GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(widget)));
        if (parent_window != NULL) {
            gint x, y;
            gtk_window_get_position(parent_window, &x, &y);
            GtkAllocation allocation;
            gtk_widget_get_allocation(GTK_WIDGET(mess_box), &allocation);
            //ебаная проверка всё ломает, как же похуй на неё
    //             g_print("Event coordinates: (%f, %f)\n", event->x_root, event->y_root);
    //             g_print("Widget position: (%d, %d)\n", x, y);
    //             g_print("Widget size: (%d, %d)\n", allocation.width, allocation.height);
    //         if (event->x_root >= allocation.x && event->x_root <= allocation.x + allocation.width &&
    // event->y_root >= allocation.y && event->y_root <= allocation.y + allocation.height) {
    //             g_print("triple xui\n\n\n");
            create_tools_menu((GdkEvent *)event, index);
            return TRUE; // Prevent further processing of the event
            // }
        }
    }
    return FALSE;
}

GdkPixbuf *file_to_pixbuf(const gchar *filename) {
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
    // Get widget allocation
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    int img_w = alloc.width;
    int img_h = alloc.height;

    double corner_radius = 12.0;
    double degrees = G_PI / 180.0;
    double radius = corner_radius;

    // Draw rounded rectangle
    cairo_move_to(cr, radius, 0);
    cairo_arc(cr, img_w - radius, radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc(cr, img_w - radius, img_h - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc(cr, radius, img_h - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, radius, radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(cr);

    // Set source pixbuf and fill
    gdk_cairo_set_source_pixbuf(cr, data, 0, 0);
    cairo_fill(cr);
}

void set_widget_height(GtkWidget *widget, int height) {
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    alloc.height = height;
    gtk_widget_set_size_request(widget, alloc.width, height);
}

GtkWidget *create_user_box(char* tag, char* last_msg, char* input_image_file) {
    // Create an event box to make the user box clickable
    GtkWidget *event_box = gtk_event_box_new();
    gtk_widget_set_name(event_box, "clickable-user-box");

    // Create the user box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    // gtk_widget_set_name(box, "user-box");
    gtk_widget_set_margin_start(box, 15);

    //avatar
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(input_image_file, NULL);
    GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(pixbuf);
    // prev_pixbuf = resize_img(prev_pixbuf, 150, 150);

    GtkWidget *image = gtk_drawing_area_new();
    gtk_widget_set_halign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(image), gdk_pixbuf_get_width(GDK_PIXBUF(prev_pixbuf)) + 3, gdk_pixbuf_get_height(GDK_PIXBUF(prev_pixbuf)) + 3);
    g_signal_connect(G_OBJECT(image), "draw", G_CALLBACK(draw_image), prev_pixbuf);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 15);

    // Create user info box
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // User tag
    GtkWidget *user_tag = gtk_label_new(tag);
    gtk_widget_set_name(user_tag, "user-tag");

    // User name and surname
    GtkWidget *last_mess = gtk_label_new(last_msg);
    gtk_widget_set_name(last_mess, "last-message");

    gtk_box_pack_start(GTK_BOX(info_box), user_tag, FALSE, FALSE, 7);
    gtk_box_pack_start(GTK_BOX(info_box), last_mess, FALSE, FALSE, 0);

    // Pack avatar and info_box into the user box
    gtk_box_pack_start(GTK_BOX(box), info_box, FALSE, FALSE, 0);

    // Pack the user box into the event box to make it clickable
    gtk_container_add(GTK_CONTAINER(event_box), box);
// g_signal_connect(event_box, "button-press-event", G_CALLBACK(user_box_clicked), NULL);
    return event_box;
}

void user_populate_scrollable_window(GtkWidget *scrollable_window) {

    GtkWidget *user_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), user_list);
    
    if (chatters != NULL) {
        // g_print("%d\n", chatters_count);s
        for (int i = 0; i < chatters_count; i++) {
            GtkWidget *user_box = create_user_box(chatters[i].username, chatters[i].lastmsg, default_img);
            gtk_widget_set_name(user_box, "user-box");
            if(i == selected_user.index) {
                selected_user.box = user_box;
                gtk_widget_override_background_color(user_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY, LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
            }
            g_signal_connect(user_box, "button-press-event", G_CALLBACK(user_box_clicked), GINT_TO_POINTER(i));

            gtk_box_pack_start(GTK_BOX(user_list), user_box, FALSE, FALSE, 0);
        }
    }
}

void user_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter) {
    GtkWidget *user_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), user_list);
    
    if (chatters != NULL) {
        // g_print("%d\n", chatters_count);s
        for (int i = 0; i < chatters_count; i++) {
            if(is_in_format(chatters[i].username, filter) || is_in_format(chatters[i].name, filter)) {
                GtkWidget *user_box = create_user_box(chatters[i].username, chatters[i].lastmsg, default_img);
                gtk_widget_set_name(user_box, "user-box");
                if(i == selected_user.index) {
                    selected_user.box = user_box;
                    gtk_widget_override_background_color(user_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY, LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
                }
                g_signal_connect(user_box, "button-press-event", G_CALLBACK(user_box_clicked), GINT_TO_POINTER(i));

                gtk_box_pack_start(GTK_BOX(user_list), user_box, FALSE, FALSE, 0);
            }
        }
    }
}

GtkWidget *create_message_box(t_message_s *message) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(box, 10);

    // Create user info box
    GtkWidget *message_box = gtk_grid_new(); // Use GtkGrid for more control over positioning
    gtk_grid_set_row_homogeneous(GTK_GRID(message_box), FALSE); // Allow rows to have different heights
    gtk_grid_set_column_homogeneous(GTK_GRID(message_box), FALSE); // Allow columns to have different widths
    gtk_widget_set_name(message_box, "message-text");

    if(mx_strcmp(message->text, ":ucode") == 0) {
        mx_b64_decodef("code.txt", "server/source/tmp.png");
        // GdkPixbuf *ucode_pixbuf = gdk_pixbuf_new_from_file("server/source/tmp.png", NULL);
        GdkPixbuf *ucode_pixbuf = gdk_pixbuf_new_from_file("client/img/ucode.png", NULL);
        if (ucode_pixbuf != NULL) {
            // Create an image widget
            GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(ucode_pixbuf);
            prev_pixbuf = resize_img(prev_pixbuf, 128, 128);
            GtkWidget *ucode_image = gtk_image_new_from_pixbuf(prev_pixbuf);
            gtk_grid_attach(GTK_GRID(message_box), ucode_image, 0, 0, 1, 1); 
            if(G_IS_OBJECT(ucode_pixbuf)){
                g_print("valid\n"); // Attach image to grid
            }
            else g_print("nihuya)))\n");
            g_object_unref(ucode_pixbuf); // Unreference the pixbuf
            remove("server/source/tmp.png");
        } else {
            // Failed to load the image, display an error message
            GtkWidget *error_label = gtk_label_new("Error: Failed to load image");
            gtk_grid_attach(GTK_GRID(message_box), error_label, 0, 0, 1, 1); // Attach error label to grid
        }
    }
    else {
        // Add text label
        GtkWidget *text_label = gtk_label_new(message->text);
        gtk_grid_attach(GTK_GRID(message_box), text_label, 0, 0, 1, 1); // Attach text label to grid        
    }


    // Add time label
    GtkWidget *time_label = gtk_label_new(message->time);
    gtk_widget_set_name(time_label, "message-time");
    gtk_widget_set_halign(time_label, GTK_ALIGN_END); // Align time label to the end (right)
    gtk_widget_set_valign(time_label, GTK_ALIGN_END); // Align time label to the end (bottom)
    gtk_grid_attach(GTK_GRID(message_box), time_label, 1, 1, 1, 1); // Attach time label to grid

    // Add message box to main box
    if(message->is_user) {
        gtk_box_pack_end(GTK_BOX(box), message_box, FALSE, TRUE, 0);
    }
    else gtk_box_pack_start(GTK_BOX(box), message_box, FALSE, TRUE, 0);
    
    return box;
}

void message_populate_scrollable_window(GtkWidget *scrollable_window) {
    GtkWidget *mess_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), mess_list);

    // for(mes)
    if (messages != NULL) {
        // for (int i = 0; messages[s].text != NULL; i++) {
        for (int i = 0; i < messages_count[selected_user.index]; i++) {
            GtkWidget *event_box = gtk_event_box_new();
            gtk_container_add(GTK_CONTAINER(mess_list), event_box); // Здесь добавляется event_box в mess_list
            GtkWidget *mess_box = create_message_box(&messages[selected_user.index][i]);
            gtk_container_add(GTK_CONTAINER(event_box), mess_box);

            // gpointer user_data[] = {GINT_TO_POINTER(i)};
            // ButtonPressEventUserData *user_data = g_new(ButtonPressEventUserData, 1);
            // int i_copy = i;
            // user_data->mess_box = mess_box;
            g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_button_press), GINT_TO_POINTER(i));
        }
    }
}
void message_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter) {
    GtkWidget *mess_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), mess_list);

    // for(mes)
    if (messages != NULL) {
        // for (int i = 0; messages[s].text != NULL; i++) {
        for (int i = 0; i < messages_count[selected_user.index]; i++) {
            if(is_in_format(messages[selected_user.index][i].text, filter)){
                GtkWidget *event_box = gtk_event_box_new();
                gtk_container_add(GTK_CONTAINER(mess_list), event_box); // Здесь добавляется event_box в mess_list
                GtkWidget *mess_box = create_message_box(&messages[selected_user.index][i]);
                gtk_container_add(GTK_CONTAINER(event_box), mess_box);
                g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_button_press), GINT_TO_POINTER(i));
            }
        }
    }
}

char* format_last_msg(char* text) {
        int len = strlen(text);
    int max_length = 25; // Maximum length of the returned string (excluding "...")

    // Allocate memory for the formatted string
    char* formatted_text = (char*)malloc((max_length + 4) * sizeof(char)); // 4 extra chars for "..." and null terminator
    if (formatted_text == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // Copy the first 25 characters of the input text
    strncpy(formatted_text, text, max_length);

    // Null-terminate the string
    formatted_text[max_length] = '\0';

    // Add "..." to indicate truncated text if the original text is longer than 25 characters
    if (len > max_length) {
        strcat(formatted_text, "...");
    }

    return formatted_text;
}

gboolean is_in_format(char* text, char* format) {
    size_t text_len = strlen(text);
    size_t format_len = strlen(format);
    
    if (format_len > text_len) {
        return FALSE; // Format is longer than text, so text can't match format
    }
    
    // Iterate through the text to find the first character of the format
    for (size_t i = 0; i <= text_len - format_len; ++i) {
        if (strncmp(text + i, format, format_len) == 0) {
            return TRUE; // Found the format in the text
        }
    }
    
    return FALSE; // Format not found in text
}

void create_txt_with_data(const char *filename, const char *username, const char *password, bool button_recognize) {
    // Открываем файл для записи
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Unable to open file %s for writing.\n", filename);
        return;
    }

    // Записываем данные в файл
    fprintf(file, "%s\n%s\n%s\n", username, password, (button_recognize ? "button_recognize" : ""));

    // Закрываем файл
    fclose(file);
}

void read_txt_from_file(const char *filename, t_user_data_s *userdata) {
    // Открываем файл для чтения
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file %s for reading.\n", filename);
        return;
    }

    // Читаем данные из файла
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    userdata->username = strdup(buffer); // Выделение памяти и копирование строки
    if (!userdata->username) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return;
    }

    fgets(buffer, sizeof(buffer), file);
    userdata->password = strdup(buffer); // Выделение памяти и копирование строки
    if (!userdata->password) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        free(userdata->username); // Освобождаем ранее выделенную память
        return;
    }

    fgets(buffer, sizeof(buffer), file);
    userdata->button_recognize = (strcmp(buffer, "button_recognize\n") == 0);

    // Закрываем файл
    fclose(file);
}

void update_user_line(const char *filename, const char *new_line) {
    // Открываем файл для чтения и записи
    FILE *file = fopen(filename, "r+");
    if (!file) {
        fprintf(stderr, "Could not open %s for reading and writing\n", filename);
        return;
    }

    // Записываем новую строку в начало файла
    fseek(file, 0, SEEK_SET);
    fputs(new_line, file);

    // Если новая строка короче предыдущей, то усекаем файл до длины новой строки
    long new_line_length = strlen(new_line);
    long remaining_length = ftell(file) - new_line_length;
    if (remaining_length > 0) {
        ftruncate(fileno(file), new_line_length);
    }

    // Закрываем файл
    fclose(file);
}

void parse_txt_buffer(const char *buffer, t_user_data_s *userdata) {
    // Разбиваем буфер на строки
    char *token;
    int count = 0;
    char *lines[3];

    token = strtok(buffer, "\n");
    while (token != NULL && count < 3) {
        lines[count++] = token;
        token = strtok(NULL, "\n");
    }

    if (count != 3) {
        fprintf(stderr, "Error: Invalid format in txt buffer\n");
        return;
    }

    // Заполняем структуру данными
    strncpy(userdata->username, lines[0], sizeof(userdata->username));
    strncpy(userdata->password, lines[1], sizeof(userdata->password));
    if (strcmp(lines[2], "button_recognize") == 0) {
        userdata->button_recognize = true;
    } else {
        userdata->button_recognize = false;
    }
}


// void create_json_with_data(const char *filename, const char *username, const char *password, bool button_recognize) {
//     // Create a new cJSON object
//     cJSON *root = cJSON_CreateObject();
//     if (!root) {
//         printf("Error: Failed to create cJSON object.\n");
//         return;
//     }

//     // Add the username, password, and button_recognize fields to the object with provided values
//     cJSON_AddStringToObject(root, "username", username);
//     cJSON_AddStringToObject(root, "password", password);
//     cJSON_AddBoolToObject(root, "button_recognize", button_recognize);

//     // Open the JSON file for writing
//     FILE *file = fopen(filename, "w");
//     if (!file) {
//         printf("Error: Unable to open file %s for writing.\n", filename);
//         cJSON_Delete(root);
//         return;
//     }

//     // Write the JSON object to the file
//     char *json_string = cJSON_Print(root);
//     if (!json_string) {
//         printf("Error: Failed to create JSON string.\n");
//         fclose(file);
//         cJSON_Delete(root);
//         return;
//     }
//     fprintf(file, "%s", json_string);
//     free(json_string);

//     // Close the file
//     fclose(file);

//     // Clean up the cJSON object
//     cJSON_Delete(root);
// }

// void read_json_from_file(const char *filename, t_user_data_s *userdata) {
//     // Открываем файл для чтения
//     FILE *file = fopen(filename, "r");
//     if (!file) {
//         fprintf(stderr, "Could not open %s for reading\n", filename);
//         return;
//     }

//     // Получаем размер файла
//     fseek(file, 0, SEEK_END);
//     long file_size = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     // Выделяем память для буфера
//     char *json_buffer = malloc(file_size + 1);
//     if (!json_buffer) {
//         fprintf(stderr, "Memory allocation error\n");
//         fclose(file);
//         return;
//     }

//     // Считываем содержимое файла в буфер
//     fread(json_buffer, 1, file_size, file);
//     fclose(file);
//     json_buffer[file_size] = '\0';

//     // Парсим JSON буфер и заполняем структуру данными
//     parse_json_buffer(json_buffer, file_size, userdata);

//     // Освобождаем память, выделенную для буфера
//     free(json_buffer);
// }

// void parse_json_buffer(const char *buffer, long buffer_size, t_user_data_s *userdata) {
//     // Разбираем JSON строку
//     cJSON *json = cJSON_Parse(buffer);

//     // Проверяем, удалось ли разобрать JSON строку
//     if (!json) {
//         const char *error_ptr = cJSON_GetErrorPtr();
//         if (error_ptr != NULL) {
//             fprintf(stderr, "Error parsing JSON: %s\n", error_ptr);
//         } else {
//             fprintf(stderr, "Unknown error parsing JSON\n");
//         }
//         return;
//     }

//     // Получаем значения из JSON объекта
//     cJSON *username = cJSON_GetObjectItemCaseSensitive(json, "username");
//     cJSON *password = cJSON_GetObjectItemCaseSensitive(json, "password");
//     cJSON *button_recognize = cJSON_GetObjectItemCaseSensitive(json, "button_recognize");

//     // Проверяем, что все поля присутствуют и имеют правильные типы
//     if (!cJSON_IsString(username) || !cJSON_IsString(password) || !cJSON_IsBool(button_recognize)) {
//         fprintf(stderr, "Invalid JSON format\n");
//         cJSON_Delete(json);
//         return;
//     }

//     // Записываем значения в структуру
//     userdata->username = strdup(username->valuestring);
//     userdata->password = strdup(password->valuestring);
//     userdata->button_recognize = cJSON_IsTrue(button_recognize);

//     // Освобождаем память, занятую JSON объектом
//     cJSON_Delete(json);
// }
void load_chats(char *username) {
    char **response = get_chats_data(username);
    // g_print("--> %s\n\n", response);
    if (strcmp(response, "1") == 0) {
        g_print("tut\n");
        // display_error_message("Username or Password is incorrect");
        return;
    }
    if (strcmp(response, "1488") == 0) {
        g_print("tut2\n");
        // display_error_message("Server v govne");
        return;
    }
    char **tokens = mx_strsplit(response, '\n');
    for(int i = 0; i < mx_get_length(tokens); i++) {
               // printf("%s\n", token);
        char **response2 = get_chatter_data(tokens[i]);
        if (strcmp(response2, "1") == 0) {
            // display_error_message("User couldn't be found");
            g_print("%s couldn't be found\n", tokens[i]);
            return;
        }
        if (strcmp(response2, "1488") == 0) {
            // display_error_message("Server v govne");
            g_print("%s ne sud'ba\n", tokens[i]);
            return;
        }
        char *token2 = strtok(response2, "\n");
        // g_print("ya vishel1\n");
        char *username = strdup(token2);
        token2 = strtok(NULL, "\n");
        char *name = strdup(token2);
        token2 = strtok(NULL, "\n");
        char *surname = strdup(token2);
        // g_print("ya vishel2\n");
        t_chatter_s new_chatter = {
            .name = mx_strdup(name),
            .surname = mx_strdup(surname),
            .username = mx_strdup(username),
            .lastmsg = mx_strdup("No messages yet"),
            .avatar = NULL
        };
        // g_print("ya vishe3l\n");
        chatters[chatters_count] = new_chatter;
        chatters_count++;
        // g_print("%s\n", token);
        // tokens[i] = strtok(NULL, "\n");
        // g_print("ya vishe4l\n");
    }
    // while (tokens != NULL) {
 
    // }
    // g_print("%d\n", chatters_count);
}
