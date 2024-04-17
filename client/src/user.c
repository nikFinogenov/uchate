#include "uchat-client.h"

static GtkWidget *search_pop_up = NULL;
static GtkWidget *error_label = NULL;
static bool settings_visible = TRUE;
static bool account_visible = TRUE;
static bool chats_visible = FALSE;
static bool chats_was_opened = FALSE;
static char *file_path_for_db;
static bool change_button_yes = FALSE;

static bool toggled = true;
typedef struct {
    GtkWidget *entry;
} CallbackData;

typedef struct {
    GtkWidget *username_entry;
} EntryWidgets;

static void refresh_account_settings(){
    gtk_container_foreach(GTK_CONTAINER(account_settings), (GtkCallback)gtk_widget_destroy, NULL);
    draw_account_settings_box();
    gtk_widget_show_all(account_settings);
}

static void deactivate_children(GtkWidget *widget) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget *child = GTK_WIDGET(iter->data);
        gtk_widget_set_sensitive(child, FALSE);
    }
    gtk_widget_set_sensitive(widget, FALSE);
    g_list_free(children);
}

static void activate_children(GtkWidget *widget) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget *child = GTK_WIDGET(iter->data);
        gtk_widget_set_sensitive(child, TRUE);
    }
    gtk_widget_set_sensitive(widget, TRUE);
    g_list_free(children);
}

static gboolean on_window_clicked(GtkWidget *widget, GdkEventButton *event, GtkWidget *element) {
    gint x = event->x;
    gint y = event->y;

    GtkAllocation allocation;
    gtk_widget_get_allocation(element, &allocation);

    if (x < allocation.x || x > allocation.x + allocation.width ||
        y < allocation.y || y > allocation.y + allocation.height) {
        gtk_widget_set_visible(element, FALSE);
        if(element == settings_box){
            settings_visible = TRUE;
            deactivate_children(settings_box);
        }
        if (element == account_settings) {
            account_visible = TRUE;
            deactivate_children(account_settings);
        }
        if(chats_was_opened && chats_visible == FALSE && account_visible == TRUE && settings_visible == TRUE) {
            chats_visible = TRUE;
            gtk_widget_set_visible(chats_box, TRUE);
            activate_children(chats_box);
        }
    }

    return FALSE;
}

static void clicked_settings(GtkWidget *widget, gpointer data){
    g_print("settings clicked\n");
}

static void display_error_message(char *message, int which) {
    error_label = gtk_label_new(message);
    gtk_widget_override_color(error_label, GTK_STATE_FLAG_NORMAL, RED_CVET);
    gtk_widget_set_margin_top(error_label, 10);
    if (which == 0){
        gtk_box_pack_start(GTK_BOX(gtk_bin_get_child(GTK_BIN(search_pop_up))), error_label, FALSE, FALSE, 0);
        gtk_widget_show_all(search_pop_up);
    } else if (which == 1) {
        gtk_box_pack_end(GTK_BOX(account_settings), error_label, FALSE, FALSE, 0);
        gtk_widget_show_all(account_settings);
    }
}

static void on_change_button_clicked(GtkWidget *area, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Select a file",
                                         GTK_WINDOW(user_window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

    GtkFileFilter *filter;
    filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        temp_avatar = gdk_pixbuf_new_from_file(filename, NULL);
        file_path_for_db = g_strdup(filename);
        change_button_yes = TRUE;
        refresh_account_settings();
        change_button_yes = FALSE;
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    GdkPixbuf *pixbuf = (GdkPixbuf *)user_data;
    gint width = gdk_pixbuf_get_width(pixbuf);
    gint height = gdk_pixbuf_get_height(pixbuf);
    gint radius = MIN(width, height) / 2;

    gint x = (gtk_widget_get_allocated_width(widget) - width) / 2;
    gint y = (gtk_widget_get_allocated_height(widget) - height) / 2;

    cairo_arc(cr, x + width / 2, y + height / 2, radius, 0, 2 * G_PI);
    cairo_clip(cr);

    gdk_cairo_set_source_pixbuf(cr, pixbuf, x, y);
    cairo_paint(cr);

    return FALSE;
}

static void on_confirm_button_clicked(GtkButton *button, gpointer data) {
    if (error_label != NULL) {
        gtk_widget_destroy(error_label);
        error_label = NULL;
    }
    GtkWidget *name_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "name_entry"));
    GtkWidget *surname_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "surname_entry"));
    GtkWidget *username_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "username_entry"));
    GtkWidget *description_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "description_entry"));

    const gchar *name_text = gtk_entry_get_text(GTK_ENTRY(name_entry));
    const gchar *surname_text = gtk_entry_get_text(GTK_ENTRY(surname_entry));
    const gchar *username_text = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const gchar *description_text = gtk_entry_get_text(GTK_ENTRY(description_entry));

    gchar *combined_text = g_strdup_printf("Name: %s\nSurname: %s\nUsername: %s\nDescription: %s\n",
                                           name_text, surname_text, username_text, description_text);

    // Выводим данные в консоль
    //g_print("Text from entries:\n%s\n", combined_text);
    if (temp_avatar != NULL){
        update_avatar(file_path_for_db, user.username);
    }
    
    char **response = update_user_info((char *)username_text, (char *)name_text, (char *)surname_text, (char *)description_text, user.username);
    if (strcmp((char *)username_text, user.username) == 0) {
        g_free(combined_text);
    } else {
        if(strcmp((char *)response, "Username already exists") == 0) {
        display_error_message("Username exists", 1);
        } else if(strcmp((char *)response, "Error checking username existence") == 0) {
            display_error_message("Username exists", 1);
        } else {
            user.username = g_strdup(username_text);
            update_user_line(login_info, user.username);
            delete_more_than_three_lines(login_info);
        }
        g_free(combined_text);
    }
}

void draw_account_settings_box(){
    GtkWidget *avatar_button;
    GtkWidget *confirm_button;
    GdkPixbuf *avatar;
    if (change_button_yes){
        avatar = temp_avatar;
    } else {
        avatar = user.avatar;
    }
    GtkWidget *drawing_area = gtk_drawing_area_new();
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(avatar, 128, 128, GDK_INTERP_BILINEAR);

    gtk_widget_set_size_request(drawing_area, 128, 128);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_event), scaled_pixbuf);
    gtk_widget_set_margin_top(drawing_area, 5);
    gtk_widget_set_margin_bottom(drawing_area, 5);
    gtk_widget_set_valign(drawing_area, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(drawing_area, GTK_ALIGN_CENTER);

    GtkWidget *name_entry;
    GtkWidget *surname_entry;
    GtkWidget *username_entry;
    GtkWidget *description_entry;

    GtkWidget *user_frame = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(user_frame, "myButton");
    GtkWidget *user_label = gtk_label_new("Username:");
    gtk_box_pack_start(GTK_BOX(user_frame), user_label, FALSE, FALSE, 0);

    GtkWidget *name_frame = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(name_frame, "myButton");
    GtkWidget *name_label = gtk_label_new("Name:");
    gtk_box_pack_start(GTK_BOX(name_frame), name_label, FALSE, FALSE, 0);

    GtkWidget *sur_frame = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(sur_frame, "myButton");
    GtkWidget *sur_label = gtk_label_new("Surname:");
    gtk_box_pack_start(GTK_BOX(sur_frame), sur_label, FALSE, FALSE, 0);

    GtkWidget *desc_frame = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(desc_frame, "myButton");
    GtkWidget *desc_label = gtk_label_new("Description:");
    gtk_box_pack_start(GTK_BOX(desc_frame), desc_label, FALSE, FALSE, 0);


    gtk_widget_set_margin_top(user_frame, 25);
    gtk_widget_set_margin_start(user_frame, 10);
    gtk_widget_set_valign(user_frame, GTK_ALIGN_START);
    gtk_widget_set_halign(user_frame, GTK_ALIGN_START);
    gtk_widget_set_margin_start(name_frame, 10);
    gtk_widget_set_valign(name_frame, GTK_ALIGN_START);
    gtk_widget_set_halign(name_frame, GTK_ALIGN_START);
    gtk_widget_set_margin_start(sur_frame, 10);
    gtk_widget_set_valign(sur_frame, GTK_ALIGN_START);
    gtk_widget_set_halign(sur_frame, GTK_ALIGN_START);
    gtk_widget_set_margin_start(desc_frame, 10);
    gtk_widget_set_valign(desc_frame, GTK_ALIGN_START);
    gtk_widget_set_halign(desc_frame, GTK_ALIGN_START);

    name_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(name_entry), user.name);

    surname_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(surname_entry), user.surname);

    username_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(username_entry), user.username);

    description_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(description_entry), user.desc);
    gtk_entry_set_placeholder_text(GTK_ENTRY(description_entry), "Write your description here...");
    
    gtk_widget_set_margin_bottom(username_entry, 10);
    gtk_widget_set_margin_start(username_entry, 10);
    gtk_widget_set_margin_end(username_entry, 10);

    gtk_widget_set_margin_bottom(name_entry, 10);
    gtk_widget_set_margin_start(name_entry, 10);
    gtk_widget_set_margin_end(name_entry, 10);

    gtk_widget_set_margin_bottom(surname_entry, 10);
    gtk_widget_set_margin_start(surname_entry, 10);
    gtk_widget_set_margin_end(surname_entry, 10);

    gtk_widget_set_margin_bottom(description_entry, 10);
    gtk_widget_set_margin_start(description_entry, 10);
    gtk_widget_set_margin_end(description_entry, 10);
    
    avatar_button = gtk_button_new_with_label("Change image");
    gtk_widget_set_hexpand(avatar_button, FALSE);
    gtk_widget_set_halign(avatar_button, GTK_ALIGN_CENTER);

    confirm_button = gtk_button_new_with_label("Confirm");
    gtk_widget_set_hexpand(confirm_button, FALSE);
    gtk_widget_set_halign(confirm_button, GTK_ALIGN_CENTER);

    GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);

    g_signal_connect(G_OBJECT(avatar_button), "clicked", G_CALLBACK(on_change_button_clicked), drawing_area);
    g_signal_connect(G_OBJECT(confirm_button), "clicked", G_CALLBACK(on_confirm_button_clicked), buffer);

    gtk_box_pack_start(GTK_BOX(account_settings), drawing_area, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(account_settings), avatar_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(account_settings), user_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(account_settings), username_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(account_settings), sur_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(account_settings), surname_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(account_settings), name_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(account_settings), name_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(account_settings), desc_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(account_settings), description_entry, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(account_settings), confirm_button, FALSE, FALSE, 50);
    g_signal_connect(user_window, "button-press-event", G_CALLBACK(on_window_clicked), account_settings);
    g_object_set_data(G_OBJECT(confirm_button), "name_entry", name_entry);
    g_object_set_data(G_OBJECT(confirm_button), "surname_entry", surname_entry);
    g_object_set_data(G_OBJECT(confirm_button), "username_entry", username_entry);
    g_object_set_data(G_OBJECT(confirm_button), "description_entry", description_entry);
}

void set_textview_text(GtkTextView *textview, const char *text) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
    gtk_text_buffer_set_text(buffer, text ? text : "", -1);
}

static void update_box_widgets(GtkWidget *box) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));

    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget *widget = GTK_WIDGET(iter->data);

        gtk_widget_queue_draw(widget);
    }

    g_list_free(children);
}

static void on_click(GtkWidget *widget, gpointer data){
    if(chats_visible){
        g_print("chats TRUE\n");
    } else {
        g_print("chat FALSE\n");
    }
    if(account_visible){
        g_print("account TRUE\n");
    } else {
        g_print("account FALSE\n");
    }
    if(settings_visible){
        g_print("settings TRUE\n");
    } else {
        g_print("settings FALSE\n");
    }
}

static gboolean on_window_clicked_for_sub(GtkWidget *widget, GdkEventButton *event, GtkWidget *element) {
    gint x = event->x;
    gint y = event->y;

    GtkAllocation allocation;
    gtk_widget_get_allocation(element, &allocation);

    if (x < allocation.x || x > allocation.x + allocation.width ||
        y < allocation.y || y > allocation.y + allocation.height) {
        gtk_widget_set_visible(element, FALSE);
    }

    return FALSE;
}

static void toggle(GtkWidget *widget, GtkWidget *element) {
    gboolean visible = gtk_widget_get_visible(element);
    activate_children(element);
    gtk_widget_set_visible(element, !visible);
}

static void special_toggle_chats(GtkWidget *widget, GtkWidget *element){
    gboolean visible = gtk_widget_get_visible(element);

    if(element == chats_box && account_visible == FALSE) {
        gtk_widget_hide(account_settings);
        account_visible = TRUE;
        deactivate_children(account_settings);

        gtk_widget_set_visible(element, !visible);
        chats_visible = !visible;
        activate_children(element);
        return;
    } else if (element == chats_box && settings_visible == FALSE) {
        gtk_widget_hide(settings_box);
        settings_visible = TRUE;
        deactivate_children(settings_box);

        gtk_widget_set_visible(element, !visible);
        chats_visible = !visible;
        activate_children(element);
        return;
    }

    if(element == chats_box && account_visible == TRUE && settings_visible == TRUE && chats_visible == TRUE) {
        gtk_widget_set_visible(element, !visible);
        chats_visible = !visible;
        activate_children(element);
        chats_was_opened = FALSE;
        return;
    }
    if(element == chats_box && account_visible == TRUE && settings_visible == TRUE) {
        gtk_widget_set_visible(element, !visible);
        chats_visible = !visible;
        activate_children(element);
        chats_was_opened = TRUE;
        return;
    }
}

static void special_toggle_account(GtkWidget *widget, GtkWidget *element){
    gboolean visible = gtk_widget_get_visible(element);
    gboolean tvisible = account_visible;
    if (element == account_settings && settings_visible == FALSE && account_visible == FALSE || chats_visible == TRUE) {
        gtk_widget_hide(chats_box);
        chats_visible = FALSE;
        deactivate_children(chats_box);
        refresh_account_settings();
        gtk_widget_set_visible(element, !visible);
        account_visible = !tvisible;
        activate_children(element);
        return;
    }


    if (element == account_settings && settings_visible == FALSE && account_visible == TRUE) {
        gtk_widget_hide(settings_box);
        settings_visible = TRUE;
        deactivate_children(settings_box);
        refresh_account_settings();
        gtk_widget_set_visible(element, !visible);
        account_visible = !tvisible;
        activate_children(element);
        return;
    }


    if (element == account_settings && settings_visible == FALSE || chats_visible == FALSE) {
        refresh_account_settings();
        gtk_widget_set_visible(element, !visible);
        account_visible = !tvisible;
        activate_children(element);
        return;
    }
}

static void special_toggle_settings(GtkWidget *widget, GtkWidget *element){
    gboolean visible = gtk_widget_get_visible(element);
    gboolean tvisible = settings_visible;
    if (element == settings_box && account_visible == FALSE && settings_visible == FALSE || chats_visible == TRUE) {
        gtk_widget_hide(chats_box);
        chats_visible = FALSE;
        deactivate_children(chats_box);

        gtk_widget_set_visible(element, !visible);
        settings_visible = !tvisible;
        activate_children(element);
        return;
    }

    if (element == settings_box && settings_visible == TRUE && account_visible == FALSE) {
        gtk_widget_hide(account_settings);
        account_visible = TRUE;
        deactivate_children(account_settings);

        gtk_widget_set_visible(element, !visible);
        settings_visible = !tvisible;
        activate_children(element);
        return;
    }

    if (element == settings_box && account_visible == FALSE || chats_visible == FALSE) {
        gtk_widget_set_visible(element, !visible);
        settings_visible = !tvisible;
        activate_children(element);
        return;
    }
}

static void realize_side_bar(GtkWidget *widget, gpointer data) {
    deactivate_children(widget);
    gtk_widget_hide(widget);
}

static void minus_dengi(GtkWidget *widget, gpointer data){
    GtkWidget *settings_t = gtk_dialog_new_with_buttons("Subscriptions", GTK_WINDOW(data),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    NULL, GTK_RESPONSE_NONE, NULL);
    gtk_window_set_default_size(GTK_WINDOW(settings_t), 600, 400);
    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 20 * PANGO_SCALE);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *bbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *sub_bbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *sbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *sub_sbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *gbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *sub_gbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_signal_connect(G_OBJECT(sub_bbox), "realize", G_CALLBACK(realize_side_bar), NULL);
    g_signal_connect(G_OBJECT(sub_sbox), "realize", G_CALLBACK(realize_side_bar), NULL);
    g_signal_connect(G_OBJECT(sub_gbox), "realize", G_CALLBACK(realize_side_bar), NULL);
    gtk_widget_set_size_request(bbox, -1, 30);
    gtk_widget_set_size_request(sbox, -1, 30);
    gtk_widget_set_size_request(gbox, -1, 30);
    gtk_widget_set_size_request(sub_bbox, -1, 30);
    gtk_widget_set_size_request(sub_sbox, -1, 30);
    gtk_widget_set_size_request(sub_gbox, -1, 30);

    GtkWidget *bronze = gtk_button_new_with_label("McOk Bronze");
    GtkWidget *silver = gtk_button_new_with_label("McOk Silver");
    GtkWidget *gold = gtk_button_new_with_label("McOk Gold");

    GtkWidget *bronze_title = gtk_label_new("2.50€/month");
    gtk_label_set_justify(GTK_LABEL(bronze_title), GTK_JUSTIFY_CENTER);
    GtkWidget *bronze_text = gtk_label_new("Pros:\n \tplus money for us :)\n Cons:\n \tminus money for you :(");
    gtk_label_set_justify(GTK_LABEL(bronze_text), GTK_JUSTIFY_RIGHT);
    gtk_container_add(GTK_CONTAINER(sub_bbox), bronze_title);
    gtk_container_add(GTK_CONTAINER(sub_bbox), bronze_text);

    GtkWidget *silver_title = gtk_label_new("5.00€/month");
    gtk_label_set_justify(GTK_LABEL(silver_title), GTK_JUSTIFY_CENTER);
    GtkWidget *silver_text = gtk_label_new("Pros:\n \tplus money for us :)\n Cons:\n \tminus money for you :(");
    gtk_label_set_justify(GTK_LABEL(silver_text), GTK_JUSTIFY_RIGHT);
    gtk_container_add(GTK_CONTAINER(sub_sbox), silver_title);
    gtk_container_add(GTK_CONTAINER(sub_sbox), silver_text);

    GtkWidget *gold_title = gtk_label_new("10.00€/month");
    gtk_label_set_justify(GTK_LABEL(gold_title), GTK_JUSTIFY_CENTER);
    GtkWidget *gold_text = gtk_label_new("Pros:\n \tplus money for us :)\n Cons:\n \tminus money for you :(");
    gtk_label_set_justify(GTK_LABEL(gold_text), GTK_JUSTIFY_RIGHT);
    gtk_container_add(GTK_CONTAINER(sub_gbox), gold_title);
    gtk_container_add(GTK_CONTAINER(sub_gbox), gold_text);

    gtk_container_add(GTK_CONTAINER(bbox), bronze);
    gtk_container_add(GTK_CONTAINER(sbox), silver);
    gtk_container_add(GTK_CONTAINER(gbox), gold);

    g_signal_connect(G_OBJECT(bronze), "clicked", G_CALLBACK(toggle), sub_bbox);
    g_signal_connect(G_OBJECT(silver), "clicked", G_CALLBACK(toggle), sub_sbox);
    g_signal_connect(G_OBJECT(gold), "clicked", G_CALLBACK(toggle), sub_gbox);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(settings_t));
    gtk_container_add(GTK_CONTAINER(content_area), bbox);
    gtk_container_add(GTK_CONTAINER(content_area), sub_bbox);
    gtk_container_add(GTK_CONTAINER(content_area), sbox);
    gtk_container_add(GTK_CONTAINER(content_area), sub_sbox);
    gtk_container_add(GTK_CONTAINER(content_area), gbox);
    gtk_container_add(GTK_CONTAINER(content_area), sub_gbox);

    gtk_widget_show_all(settings_t);

    g_signal_connect_swapped(settings_t, "response", G_CALLBACK(gtk_widget_destroy), settings_t);
    g_signal_connect(settings_t, "button-press-event", G_CALLBACK(on_window_clicked_for_sub), sub_bbox);
    g_signal_connect(settings_t, "button-press-event", G_CALLBACK(on_window_clicked_for_sub), sub_sbox);
    g_signal_connect(settings_t, "button-press-event", G_CALLBACK(on_window_clicked_for_sub), sub_gbox);
}

static void three_hundred_bucks_window(GtkWidget *widget, gpointer data){
    GtkWidget *settings_s = gtk_dialog_new_with_buttons("Donate", GTK_WINDOW(data),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    NULL, GTK_RESPONSE_NONE, NULL);
    gtk_window_set_default_size(GTK_WINDOW(settings_s), 600, 400);
    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 20 * PANGO_SCALE);
    GtkWidget *dengi_goni = gtk_label_new(NULL);
    gtk_widget_override_font(dengi_goni, font_desc);
    gtk_label_set_markup(GTK_LABEL(dengi_goni), "<a href=''>Dat' na lapu</a>");
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(settings_s));
    gtk_container_add(GTK_CONTAINER(content_area), dengi_goni);

    gtk_widget_show_all(settings_s);

    g_signal_connect_swapped(settings_s, "response", G_CALLBACK(gtk_widget_destroy), settings_s);
}

static void devs_window(GtkWidget *widget, gpointer data){
    GtkWidget *settings_f = gtk_dialog_new_with_buttons("Developers", GTK_WINDOW(data),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    NULL, GTK_RESPONSE_NONE, NULL);
    gtk_window_set_default_size(GTK_WINDOW(settings_f), 600, 400);
    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 20 * PANGO_SCALE);
    GtkWidget *razrab1 = gtk_label_new(NULL);
    GtkWidget *razrab2 = gtk_label_new(NULL);
    GtkWidget *razrab3 = gtk_label_new(NULL);
    GtkWidget *razrab4 = gtk_label_new(NULL);
    gtk_widget_override_font(razrab1, font_desc);
    gtk_widget_override_font(razrab2, font_desc);
    gtk_widget_override_font(razrab3, font_desc);
    gtk_widget_override_font(razrab4, font_desc);
    gtk_label_set_markup(GTK_LABEL(razrab1), "<a href='https://github.com/nikFinogenov'>razrab1</a>");
    gtk_label_set_markup(GTK_LABEL(razrab2), "<a href='https://github.com/DMYTRO-DOLHII'>razrab2</a>");
    gtk_label_set_markup(GTK_LABEL(razrab3), "<a href='https://github.com/WoCCeR'>razrab3</a>");
    gtk_label_set_markup(GTK_LABEL(razrab4), "<a href='https://github.com/kitska'>razrab4</a>");
    gtk_label_set_justify(GTK_LABEL(razrab1), GTK_JUSTIFY_CENTER);
    gtk_label_set_justify(GTK_LABEL(razrab2), GTK_JUSTIFY_CENTER);
    gtk_label_set_justify(GTK_LABEL(razrab3), GTK_JUSTIFY_CENTER);
    gtk_label_set_justify(GTK_LABEL(razrab4), GTK_JUSTIFY_CENTER);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(settings_f));
    gtk_container_add(GTK_CONTAINER(content_area), razrab1);
    gtk_container_add(GTK_CONTAINER(content_area), razrab2);
    gtk_container_add(GTK_CONTAINER(content_area), razrab3);
    gtk_container_add(GTK_CONTAINER(content_area), razrab4);
    gtk_widget_set_halign(GTK_WIDGET(content_area), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(content_area), GTK_ALIGN_CENTER);

    gtk_widget_show_all(settings_f);

    g_signal_connect_swapped(settings_f, "response", G_CALLBACK(gtk_widget_destroy), settings_f);
}

static void on_clear_search_clicked(GtkButton *button, GtkEntry *entry) {
    gtk_entry_set_text(entry, "");
    refresh_scrollable_window(scrollable_window);
}

static void on_clear_mess_search_clicked(GtkButton *button, GtkEntry *entry) {
    gtk_entry_set_text(entry, "");
    refresh_scrollable_window2(scrollable_window2);
}

static void display_joke(GtkWidget *widget, gpointer data) {
    GtkWidget *joke = gtk_dialog_new_with_buttons("Random Joke", GTK_WINDOW(data),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    NULL, GTK_RESPONSE_NONE, NULL);

    gtk_window_set_default_size(GTK_WINDOW(joke), 50, 50);

    gint random_number = g_random_int_range(1, 3);

    if (random_number == 1) {
        GtkWidget *joke_text = gtk_label_new(get_random_joke());
        gtk_label_set_xalign(GTK_LABEL(joke_text), 0.5); 
        gtk_label_set_yalign(GTK_LABEL(joke_text), 0.5);
        gtk_box_pack_start(GTK_BOX(gtk_bin_get_child(GTK_BIN(joke))), joke_text, FALSE, FALSE, 100); 
    } else {
        int index = g_random_int_range(1, 31);
        char path[50];
        sprintf(path, "client/jokes/%d.png", index);
        GdkPixbuf *joke_pixbuf = gdk_pixbuf_new_from_file(path, NULL);
        if (joke_pixbuf != NULL) {
            GtkWidget *joke_image = gtk_image_new_from_pixbuf(joke_pixbuf);
            g_object_unref(joke_pixbuf);
            gtk_box_pack_start(GTK_BOX(gtk_bin_get_child(GTK_BIN(joke))), joke_image, FALSE, FALSE, 100); 
            gtk_widget_set_size_request(joke, -1, -1); // set the size of the dialog to the size of the image
            gtk_widget_show_all(joke_image);
        }
    }
    gtk_widget_show_all(joke);

    g_signal_connect_swapped(joke, "response", G_CALLBACK(gtk_widget_destroy), joke);
}
static void on_window_realize(GtkWidget *widget, gpointer data) {
    GtkWidget *scrollable_window = GTK_WIDGET(data);

    GtkAdjustment *v_adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollable_window));

    gtk_adjustment_set_value(v_adjustment, gtk_adjustment_get_upper(v_adjustment) - gtk_adjustment_get_page_size(v_adjustment));
    gtk_widget_hide(chat_box);   
}
void add_chatter(const char* name, const char* surname, const char* username, const char* lastmsg, GdkPixbuf* avatar) {
    if (chatters_count >= MAX_CHATTERS) {
        fprintf(stderr, "Error: Maximum number of chatters reached.\n");
        return;
    }

    size_t name_len = strlen(name) + 1;
    size_t surname_len = strlen(surname) + 1;
    size_t username_len = strlen(username) + 1;
    size_t lastmsg_len = strlen(lastmsg) + 1;

    chatters[chatters_count].name = malloc(name_len);
    if (chatters[chatters_count].name == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for name.\n");
        return;
    }
    strcpy(chatters[chatters_count].name, name);

    chatters[chatters_count].surname = malloc(surname_len);
    if (chatters[chatters_count].surname == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for surname.\n");
        return;
    }
    strcpy(chatters[chatters_count].surname, surname);

    chatters[chatters_count].username = malloc(username_len);
    if (chatters[chatters_count].username == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for username.\n");
        return;
    }
    strcpy(chatters[chatters_count].username, username);

    chatters[chatters_count].lastmsg = malloc(lastmsg_len);
    if (chatters[chatters_count].lastmsg == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for lastmsg.\n");
        return;
    }
    strcpy(chatters[chatters_count].lastmsg, lastmsg);

    chatters[chatters_count].avatar = avatar ? gdk_pixbuf_ref(avatar) : NULL;

    chatters_count++;
}
static void search_user(GtkWidget *widget, gpointer user_data) {
    EntryWidgets *data = (EntryWidgets *)user_data;
    const gchar *data_username = gtk_entry_get_text(GTK_ENTRY(data->username_entry));

    char *parsed_username = (char*)data_username; 

    if (error_label != NULL) {
        gtk_widget_destroy(error_label);
        error_label = NULL;
    }

    if (strcmp(parsed_username, "") == 0) {
        display_error_message("Username cannot be empty", 0);
        return;
    }

    if (strcmp(user.username, parsed_username) == 0) {
        display_error_message("You cannot open chat with yourself", 0);
        return;
    }

    for (int i = 0; i < chatters_count; i++) {
        if (strcmp(chatters[i].username, parsed_username) == 0){
            display_error_message("Chat already exists", 0);
            return;
        }
    }
    
    char **response = get_chatter_data(parsed_username);

    if (strcmp((char *)response, "1") == 0) {
        display_error_message("User couldn't be found", 0);
        return;
    }
    if (strcmp((char *)response, "1488") == 0) {
        display_error_message("Server offline", 0);
        return;
    }

    char *token = strtok((char *)response, "\n");
    char *username = strdup(token);
    token = strtok(NULL, "\n");
    char *name = strdup(token);
    token = strtok(NULL, "\n");
    char *surname = strdup(token);
    
    if(chatters_count + 1 < MAX_CHATTERS) {
        char** response2 = send_new_chat_data(user.username, username);
    
        if (strcmp((char *)response2, "1") == 0) {
            display_error_message("Chat already exists", 0);
            return;
        }
        if (strcmp((char *)response2, "1488") == 0) {
            display_error_message("Server offline", 0);
            return;
        }
        add_chatter(name, surname, username, "No messages yet", NULL);
        refresh_scrollable_window(scrollable_window);
        gtk_widget_show(scrollable_window);
        gtk_widget_hide(add_new_chat_when_no_chats);

        gtk_widget_destroy(search_pop_up);
        
        if (!is_chatters_empty()) {
            gtk_label_set_text(GTK_LABEL(empty_chat), "[ Select a chat to start chatting ]");
        }
        return;
    }
    
    g_print("Chatter limit reached\n");
}

static void destroy_close_search(void){
    if(error_label != NULL) gtk_widget_destroy(error_label);
    gtk_widget_destroy(search_pop_up);
    error_label = NULL;
}

static void add_chatter_button_clicked(GtkWidget *widget, gpointer data) {
    if (chatters == NULL) {
        g_print("Chatters array is not initialized\n");
        return;
    }

    search_pop_up = gtk_dialog_new_with_buttons("Search User", GTK_WINDOW(data),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    NULL, GTK_RESPONSE_NONE, NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(search_pop_up), 400, 150);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(search_pop_up));

    GtkWidget *search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter username...");
    gtk_container_add(GTK_CONTAINER(content_area), search_entry);

    GtkWidget *search_button = gtk_button_new_with_label("Search");
    gtk_container_add(GTK_CONTAINER(content_area), search_button);

    EntryWidgets *entry = g_new(EntryWidgets, 1);
    entry->username_entry = search_entry;
    g_signal_connect(search_button, "clicked", G_CALLBACK(search_user), entry);

    gtk_widget_set_margin_top(search_button, 10);

    gtk_widget_set_vexpand(search_entry, TRUE);
    gtk_widget_set_vexpand(search_button, TRUE);
    gtk_widget_set_valign(search_entry, GTK_ALIGN_FILL);
    gtk_widget_set_valign(search_button, GTK_ALIGN_CENTER);

    gtk_widget_set_halign(content_area, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(content_area, GTK_ALIGN_CENTER);

    gtk_widget_show_all(search_pop_up);
    
    g_signal_connect_swapped(search_pop_up, "response", G_CALLBACK(destroy_close_search), NULL);    
}

void add_message(int mess_id, int chatter_id, const char* text, const char* time, bool is_user) {
    if (chatter_id < 0 || chatter_id >= MAX_CHATTERS) {
        fprintf(stderr, "Error: Invalid chatter ID.\n");
        return;
    }

    if (messages_count[chatter_id] >= MAX_MESSAGES) {
        fprintf(stderr, "Error: Maximum number of messages reached for chatter ID %d.\n", chatter_id);
        return;
    }

    size_t text_len = strlen(text) + 1;
    size_t time_len = strlen(time) + 1;

    messages[chatter_id][messages_count[chatter_id]].id = mess_id;
    messages[chatter_id][messages_count[chatter_id]].text = malloc(text_len);
    if (messages[chatter_id][messages_count[chatter_id]].text == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for message text.\n");
        return;
    }
    strcpy(messages[chatter_id][messages_count[chatter_id]].text, text);

    messages[chatter_id][messages_count[chatter_id]].time = malloc(time_len);
    if (messages[chatter_id][messages_count[chatter_id]].time == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for message time.\n");
        return;
    }
    strcpy(messages[chatter_id][messages_count[chatter_id]].time, time);

    messages[chatter_id][messages_count[chatter_id]].is_user = is_user;

    messages_count[chatter_id]++;
}

static void add_message_button_clicked(GtkWidget *widget, gpointer user_data) {
    CallbackData *data = (CallbackData *)user_data;
    char *text = (char *)gtk_entry_get_text(GTK_ENTRY(data->entry));
    int i = 0, j = 0;

    while (mx_isspace(text[i])) i++;
    while (text[i]) text[j++] = text[i++];
    text[j] = '\0';

    if(mx_strcmp(text, "") == 0) return;
    if (messages == NULL) {
        g_print("Messages array is not initialized\n");
        return;
    }

    if(strlen(text) > 512) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error: Message is too long\n512 symbols max, buy premium for more");
        gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    wrap_text(text);

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char time_str[6];
    strftime(time_str, sizeof(time_str), "%H:%M", timeinfo);

    int m_id = mx_atoi((char *)add_new_message(user.username, chatters[selected_user.index].username, text, time_str, user.username));
    
    if(m_id == -1) {
        g_print("Ne poluchilos\n");
        return;
    }
    if(m_id == -1488) {
        g_print("Server offline\n");
        return;
    }
    // t_message_s new_mes = {
    //     .id = m_id,
    //     .text = mx_strdup(text),
    //     .time = mx_strdup(time_str),
    //     .is_user = TRUE
    // };

    if(messages_count[selected_user.index] + 1 < MAX_MESSAGES) {
        add_message(m_id, selected_user.index, text, time_str, true);
        // messages[selected_user.index][messages_count[selected_user.index]] = new_mes;
        // messages_count[selected_user.index]++;
        refresh_scrollable_window2(scrollable_window2);
        gtk_widget_show(scrollable_window2);
        chatters[selected_user.index].lastmsg = format_last_msg((char *)text);
        refresh_scrollable_window(scrollable_window);
        gtk_widget_show(scrollable_window); 
        gtk_entry_set_text(GTK_ENTRY(data->entry), "");
        return;
    }
    g_print("Messages limit reached\n");
}

static gboolean on_entry_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        CallbackData *callback_data = (CallbackData *)user_data;
        
        add_message_button_clicked(GTK_WIDGET(callback_data->entry), callback_data);
        
        return TRUE;
    }

    return FALSE;
}

static void settings_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Settings clicked\n");
}

static void user_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("User clicked\n");
}

static void message_search_clicked(GtkWidget *widget, gpointer user_data) {
    CallbackData *data = (CallbackData *)user_data;
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(data->entry));
    gtk_container_foreach(GTK_CONTAINER(scrollable_window2), (GtkCallback)gtk_widget_destroy, NULL);
    message_populate_scrollable_filtred_window(scrollable_window2, (char *)text);
    gtk_widget_show_all(scrollable_window2);
}

static void chatter_search_clicled(GtkWidget *widget, gpointer user_data) {
    CallbackData *data = (CallbackData *)user_data;
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(data->entry));

    gtk_container_foreach(GTK_CONTAINER(scrollable_window), (GtkCallback)gtk_widget_destroy, NULL);
    user_populate_scrollable_filtred_window(scrollable_window, (char *)text);
    gtk_widget_show_all(scrollable_window);
}

void on_adjustment_size_allocate(GtkAdjustment *adjustment, gpointer data) {
    gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment));
}

void wrap_text(char *text) {
    int len = strlen(text);
    int i, line_length = 0;
    int j = MAX_LINE_LENGTH;
    for (i = 0; i < len; i++) {
        if (mx_isspace(text [i])){
                j = i;
            }
        if (line_length >= MAX_LINE_LENGTH) {
            if (j == MAX_LINE_LENGTH) {
                j = i;
                memmove(&text[j + 1], &text[j], len - j);
                len++;
            }
            
            text[j] = '\n';
            line_length = 0;
            j = MAX_LINE_LENGTH;
            
        }
        line_length++;
    }
}

void refresh_scrollable_window(GtkWidget *scrollable) {
    gtk_container_foreach(GTK_CONTAINER(scrollable), (GtkCallback)gtk_widget_destroy, NULL);
    
    user_populate_scrollable_window(scrollable);
    GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollable));
    g_signal_connect(vadjustment, "changed", G_CALLBACK(on_adjustment_size_allocate), NULL);
    gtk_widget_show_all(scrollable);
}

void refresh_scrollable_window2(GtkWidget *scrollable) {
    gtk_container_foreach(GTK_CONTAINER(scrollable), (GtkCallback)gtk_widget_destroy, NULL);
    
    message_populate_scrollable_window(scrollable);
    GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrollable));
    g_signal_connect(vadjustment, "changed", G_CALLBACK(on_adjustment_size_allocate), NULL);

    
    gtk_widget_show_all(scrollable);
}

void show_user_window() {
    gtk_widget_show_all(user_window);
}

void draw_user_info_box(GtkWidget *user_info_box) {
    GdkPixbuf *avatar_for_chat;
    GdkPixbuf *pixbuf;
    GdkPixbuf *prev_pixbuf;
    if (chatters == NULL || selected_user.index == -1) {
        pixbuf = NULL;
        prev_pixbuf = NULL;
    } else {
        char *avatar_path = (char *)malloc(strlen(AVATAR_FOLDER) + strlen(chatters[selected_user.index].username) + strlen("_avatar.png") + 1);
        get_and_save_avatar_to_file(chatters[selected_user.index].username);
        sprintf(avatar_path, "%s%s_avatar.png", AVATAR_FOLDER, chatters[selected_user.index].username);
        avatar_for_chat = gdk_pixbuf_new_from_file(avatar_path, NULL);
        pixbuf = gdk_pixbuf_scale_simple(avatar_for_chat, 64, 64, GDK_INTERP_BILINEAR);
        prev_pixbuf = gdk_pixbuf_copy(pixbuf);
        remove(avatar_path);
        free(avatar_path);
    }
    
    GtkWidget *image = gtk_drawing_area_new();
    gtk_widget_set_halign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    int w = (pixbuf == NULL) ? 10 : gdk_pixbuf_get_width(GDK_PIXBUF(prev_pixbuf));
    int h = (pixbuf == NULL) ? 10 : gdk_pixbuf_get_height(GDK_PIXBUF(prev_pixbuf));
    gtk_widget_set_size_request(GTK_WIDGET(image), w, h);
    g_signal_connect(G_OBJECT(image), "draw", G_CALLBACK(draw_image_for_chat_box), prev_pixbuf);
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

    GtkWidget *message_clear_search_button = gtk_button_new();
    GtkWidget *mess_clear_label = gtk_label_new("x");
    gtk_container_add(GTK_CONTAINER(message_clear_search_button), mess_clear_label);

    g_signal_connect(message_clear_search_button, "clicked", G_CALLBACK(on_clear_mess_search_clicked), message_search_entry);

    gtk_box_pack_start(GTK_BOX(message_search_entry_box), message_search_entry, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(message_search_entry_box), message_clear_search_button, FALSE, FALSE, 0);


    GtkWidget* message_search_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(message_search_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(message_search_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(message_search_img), 0);
    gtk_widget_set_size_request(GTK_WIDGET(message_search_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(message_search_img), "message-search-img");
    CallbackData *find_mess_data = g_slice_new(CallbackData);
    find_mess_data->entry = message_search_entry;
    g_signal_connect(G_OBJECT(message_search_img), "clicked", G_CALLBACK(message_search_clicked), find_mess_data);

    gtk_box_pack_end(GTK_BOX(user_info_box), message_search_img, FALSE, FALSE,0);
    gtk_box_pack_end(GTK_BOX(user_info_box), message_search_entry_box, FALSE, FALSE, 5);
}

static void clicked_side(GtkWidget *widget, gpointer data){
    if(toggled)
        gtk_widget_set_name(GTK_WIDGET(widget), "sidebar-rotated");
    else 
        gtk_widget_set_name(GTK_WIDGET(widget), "sidebar");
    toggled = !toggled;
}

static void logout_clicked(GtkWidget *widget, gpointer data){
    gtk_widget_hide(user_window);
    stop_chat_checker();
    userdata.button_recognize = false;
    update_user_status("offline", user.username);
    // clear_all();
    clear_data();
    // fill_data();
    show_login();
}

void on_window_destroy(GtkWidget *widget, gpointer data) {
    update_user_status("offline", user.username);
    gtk_main_quit();
}
static void realize_chatters(GtkWidget *widget, gpointer data) {
    if(chatters_count != 0) gtk_widget_hide(add_new_chat_when_no_chats);
    else gtk_widget_show(add_new_chat_when_no_chats);
}
void draw_user_window() {
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "client/style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
    GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    user_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(user_window), MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    g_signal_connect(user_window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_window_maximize(GTK_WINDOW(user_window));

    GtkWidget *hbox_main = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(user_window), hbox_main);

    settings_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    account_settings = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    chats_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_widget_override_background_color(settings_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHT_GRAY, LIGHT_GRAY, LIGHT_GRAY, 1.0}); 
    gtk_widget_override_background_color(account_settings, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHT_GRAY, LIGHT_GRAY, LIGHT_GRAY, 1.0});
    gtk_widget_override_background_color(chats_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHT_GRAY, LIGHT_GRAY, LIGHT_GRAY, 1.0});
    gtk_widget_override_background_color(chat_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){DARK_GRAY, DARK_GRAY, DARK_GRAY, 1.0});

    gtk_widget_set_size_request(settings_box, 500, -1);
    gtk_widget_set_size_request(account_settings, 500, -1);
    gtk_widget_set_size_request(chats_box, 350, -1);
    g_signal_connect(G_OBJECT(settings_box), "realize", G_CALLBACK(realize_side_bar), NULL);
    g_signal_connect(G_OBJECT(account_settings), "realize", G_CALLBACK(realize_side_bar), NULL);
    g_signal_connect(G_OBJECT(chats_box), "realize", G_CALLBACK(realize_side_bar), NULL);

    GtkStyleContext *context;
    context = gtk_widget_get_style_context(GTK_WIDGET(settings_box));
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    GtkWidget *ebox1, *ebox2, *ebox3;
    GtkWidget *nbox1, *nbox2, *nbox3;

    nbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    nbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    nbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    
    ebox1 = gtk_event_box_new();
    ebox2 = gtk_event_box_new();
    ebox3 = gtk_event_box_new();

    gtk_widget_override_background_color(ebox1, GTK_STATE_FLAG_NORMAL, NORM_CVET);
    gtk_widget_override_background_color(ebox2, GTK_STATE_FLAG_NORMAL, NORM_CVET);
    gtk_widget_override_background_color(ebox3, GTK_STATE_FLAG_NORMAL, NORM_CVET);

    GtkWidget *fimage = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(fimage, 40, 40);
    GtkWidget *simage = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(simage, 40, 40);
    GtkWidget *timage = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(timage, 40, 40);
    GtkWidget *fbutton = gtk_button_new_with_label("Devs(bedolagy)");
    GtkWidget *sbutton = gtk_button_new_with_label("Пожертвовать в семью бедолаг");
    GtkWidget *tbutton = gtk_button_new_with_label("McOk Gold | Silver | Bronze");

    gtk_widget_set_name(fimage, "fimg_in_settings");
    gtk_widget_set_name(simage, "simg_in_settings");
    gtk_widget_set_name(timage, "timg_in_settings");
    gtk_widget_set_name(ebox1, "settings_button");
    gtk_widget_set_name(ebox2, "settings_button");
    gtk_widget_set_name(ebox3, "settings_button");

    set_widget_height(ebox1, 50);
    set_widget_height(ebox2, 50);
    set_widget_height(ebox3, 50);

    GtkWidget *falign = gtk_alignment_new(0, 0, 1, 0);
    GtkWidget *salign = gtk_alignment_new(0, 0, 1, 0);
    GtkWidget *talign = gtk_alignment_new(0, 0, 1, 0);
    GtkWidget *ffalign = gtk_alignment_new(0, 0, 0, 0);
    GtkWidget *sfalign = gtk_alignment_new(0, 0, 0, 0);
    GtkWidget *fsalign = gtk_alignment_new(0, 0, 0, 0);
    GtkWidget *ssalign = gtk_alignment_new(0, 0, 0, 0);
    GtkWidget *ftalign = gtk_alignment_new(0, 0, 0, 0);
    GtkWidget *stalign = gtk_alignment_new(0, 0, 0, 0);

    gtk_container_add(GTK_CONTAINER(ffalign), fimage);
    gtk_container_add(GTK_CONTAINER(sfalign), fbutton);
    gtk_container_add(GTK_CONTAINER(nbox1), ffalign);
    gtk_container_add(GTK_CONTAINER(nbox1), sfalign);

    gtk_container_add(GTK_CONTAINER(fsalign), simage);
    gtk_container_add(GTK_CONTAINER(ssalign), sbutton);
    gtk_container_add(GTK_CONTAINER(nbox2), fsalign);
    gtk_container_add(GTK_CONTAINER(nbox2), ssalign);

    gtk_container_add(GTK_CONTAINER(ftalign), timage);
    gtk_container_add(GTK_CONTAINER(stalign), tbutton);
    gtk_container_add(GTK_CONTAINER(nbox3), ftalign);
    gtk_container_add(GTK_CONTAINER(nbox3), stalign);

    gtk_container_add(GTK_CONTAINER(ebox1), nbox1);
    gtk_container_add(GTK_CONTAINER(ebox2), nbox2);
    gtk_container_add(GTK_CONTAINER(ebox3), nbox3);

    gtk_container_add(GTK_CONTAINER(falign), ebox1);
    gtk_container_add(GTK_CONTAINER(salign), ebox2);
    gtk_container_add(GTK_CONTAINER(talign), ebox3);

    gtk_alignment_set_padding(GTK_ALIGNMENT(ffalign), 10, 0, 60, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(sfalign), 15, 10, 5, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(fsalign), 10, 0, 60, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(ssalign), 15, 10, 5, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(ftalign), 10, 0, 60, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(stalign), 15, 10, 5, 0);

    gtk_alignment_set_padding(GTK_ALIGNMENT(falign), 20, 10, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(salign), 0, 10, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(talign), 0, 10, 0, 0);

    g_signal_connect(G_OBJECT(fbutton), "clicked", G_CALLBACK(devs_window), user_window);
    g_signal_connect(G_OBJECT(sbutton), "clicked", G_CALLBACK(three_hundred_bucks_window), user_window);
    g_signal_connect(G_OBJECT(tbutton), "clicked", G_CALLBACK(minus_dengi), user_window);

    gtk_box_pack_start(GTK_BOX(settings_box), falign, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(settings_box), salign, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(settings_box), talign, FALSE, FALSE, 0);
    g_signal_connect(user_window, "button-press-event", G_CALLBACK(on_window_clicked), settings_box);

    draw_account_settings_box();

    GtkWidget *side_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_override_background_color(side_box, GTK_STATE_FLAG_NORMAL, NORM_CVET);
    gtk_widget_set_size_request(side_box, 75, -1);
    

    GtkWidget* add = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(add), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(add), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(add), 0);
    gtk_widget_set_size_request(GTK_WIDGET(add), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(add), "add");
    g_signal_connect(G_OBJECT(add), "realize", G_CALLBACK(realize_side_bar), NULL);
    g_signal_connect(G_OBJECT(add), "clicked", G_CALLBACK(add_chatter_button_clicked), user_window);

    GtkWidget* chats = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(chats), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(chats), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(chats), 0);
    gtk_widget_set_size_request(GTK_WIDGET(chats), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(chats), "chats");
    g_signal_connect(G_OBJECT(chats), "realize", G_CALLBACK(realize_side_bar), NULL);
    g_signal_connect(G_OBJECT(chats), "clicked", G_CALLBACK(special_toggle_chats), chats_box);


    GtkWidget* anekdot = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(anekdot), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(anekdot), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(anekdot), 0);
    gtk_widget_set_size_request(GTK_WIDGET(anekdot), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(anekdot), "anekdot");
    g_signal_connect(G_OBJECT(anekdot), "realize", G_CALLBACK(realize_side_bar), NULL);
    g_signal_connect(G_OBJECT(anekdot), "clicked", G_CALLBACK(display_joke), user_window);

    GtkWidget* account = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(account), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(account), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(account), 0);
    gtk_widget_set_size_request(GTK_WIDGET(account), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(account), "account");
    g_signal_connect(G_OBJECT(account), "realize", G_CALLBACK(realize_side_bar), NULL);
    g_signal_connect(G_OBJECT(account), "clicked", G_CALLBACK(special_toggle_account), account_settings);


    GtkWidget* settings_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(settings_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(settings_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(settings_img), 0);
    gtk_widget_set_size_request(GTK_WIDGET(settings_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(settings_img), "settings");
    g_signal_connect(G_OBJECT(settings_img), "realize", G_CALLBACK(realize_side_bar), NULL);
    g_signal_connect(G_OBJECT(settings_img), "clicked", G_CALLBACK(special_toggle_settings), settings_box);


    GtkWidget* side_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(side_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(side_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(side_img), 0);
    gtk_widget_set_size_request(GTK_WIDGET(side_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(side_img), "sidebar");
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), chats);
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), settings_img);
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), anekdot);
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), account);
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(toggle), add);
    g_signal_connect(G_OBJECT(side_img), "clicked", G_CALLBACK(clicked_side), NULL);

    GtkWidget* logout_img = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(logout_img), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(logout_img), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(logout_img), 0);
    gtk_widget_set_size_request(GTK_WIDGET(logout_img), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(logout_img), "logout");
    g_signal_connect(G_OBJECT(logout_img), "clicked", G_CALLBACK(logout_clicked), NULL);

    
    gtk_box_pack_start(GTK_BOX(side_box), side_img, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), chats, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), settings_img, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), account, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), add, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(side_box), anekdot, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(side_box), logout_img, FALSE, FALSE, 0);

    GtkWidget *search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Search...");

    GtkWidget *search_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *search_button = gtk_button_new_with_label("[ Find a user ]");
    gtk_widget_set_size_request(search_button, 25, 35);
    gtk_widget_set_name(GTK_WIDGET(search_button), "search_button");
    CallbackData *find__data = g_slice_new(CallbackData);
    find__data->entry = search_entry;
    g_signal_connect(G_OBJECT(search_button), "clicked", G_CALLBACK(chatter_search_clicled), find__data);

    GtkWidget *clear_search_button = gtk_button_new();
    GtkWidget *label = gtk_label_new("x");
    gtk_container_add(GTK_CONTAINER(clear_search_button), label);

    g_signal_connect(clear_search_button, "clicked", G_CALLBACK(on_clear_search_clicked), search_entry);

    
    GtkWidget *search_entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(search_entry_box), search_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(search_entry_box), clear_search_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(search_box), search_entry_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(search_box), search_button, TRUE, TRUE, 0);
    
    gtk_box_pack_start(GTK_BOX(chats_box), search_box, FALSE, FALSE, 0);
    scrollable_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollable_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    user_populate_scrollable_window(scrollable_window);
    g_signal_connect(G_OBJECT(scrollable_window), "realize", G_CALLBACK(realize_chatters), NULL);
    gtk_box_pack_start(GTK_BOX(chats_box), scrollable_window, TRUE, TRUE, 0);

    add_new_chat_when_no_chats = gtk_button_new();
    gtk_widget_set_valign(GTK_WIDGET(add_new_chat_when_no_chats), GTK_ALIGN_CENTER);
    gtk_button_set_relief(GTK_BUTTON(add_new_chat_when_no_chats), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(add_new_chat_when_no_chats), 0);
    gtk_widget_set_size_request(GTK_WIDGET(add_new_chat_when_no_chats), 64, 64);
    gtk_widget_set_name(GTK_WIDGET(add_new_chat_when_no_chats), "add_new_chat_when_no_chats");
    g_signal_connect(G_OBJECT(add_new_chat_when_no_chats), "clicked", G_CALLBACK(add_chatter_button_clicked), user_window);
    gtk_box_pack_end(GTK_BOX(chats_box), add_new_chat_when_no_chats, FALSE, FALSE, 5);

    if (chatters_count != 0) {
    // //     gtk_widget_show(add_new_chat_when_no_chats);
    // //     gtk_widget_hide(scrollable_window);
    // // } else {
    //     // gtk_widget_show(scrollable_window);
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

    gtk_box_pack_start(GTK_BOX(hbox_main), side_box, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), chats_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), settings_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), account_settings, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), chat_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_main), empty_chat, TRUE, TRUE, 0);
}
