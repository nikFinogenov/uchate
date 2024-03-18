#include "uchat-client.h"
#include <math.h>

// const char *input_image_file = "client/img/simple.png"; // Input image file path
t_selected_s selected_user = {
    .index = -1,
    .box = NULL
};
// GtkWidget *selected_user = NULL;

void on_scrollable_window_clicked(GtkWidget *widget, GdkEvent *event, gpointer data) {
    // g_print("xyia");
    if (selected_user.index == -1) {
        // Если пользователь не выбран, скрываем окно с сообщениями и показываем пустой чат
        gtk_widget_hide(widget);
        // GtkWidget *empty_chat = gtk_label_new("[ Select chat to start chatting ]");
        // gtk_widget_set_name(GTK_WIDGET(empty_chat), "empty-chat");
        // gtk_widget_set_halign(empty_chat, GTK_ALIGN_CENTER);
        // gtk_container_add(GTK_CONTAINER(scrollable_window), empty_chat);
        gtk_widget_show_all(empty_chat);
    } else {
        // Если пользователь выбран, скрываем пустой чат и показываем окно с сообщениями
        gtk_widget_hide(empty_chat); // Предполагается, что empty_chat доступен в этой области видимости
        gtk_widget_show_all(widget);
    }
}

int count_messages(void) {
    int count = 0;
    // Count messages until a sentinel value is encountered
    while (messages[count].text != NULL) {
        count++;
    }
    return count;
}

int count_chatters(void) {
    int count = 0;
    // Count messages until a sentinel value is encountered
    while (chatters[count].username != NULL) {
        count++;
    }
    return count;
}

gboolean user_box_clicked(GtkWidget *widget, GdkEvent *event, int index) {
    g_print("clicked -> ");
    if (selected_user.box != NULL) {
        gtk_widget_override_background_color(selected_user.box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHT_GRAY, LIGHT_GRAY, LIGHT_GRAY, 1.0}); 
    }

    // Change background color of the clicked user box to indicate selection
    gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY, LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
    // Update the reference to the currently selected user box
    selected_user.box = widget;
    selected_user.index = index;

    gtk_widget_hide(empty_chat);
    gtk_widget_show_all(scrollable_window2);
    
    g_print("%d\n", selected_user.index);

    // Returning FALSE allows the event to propagate further
    return FALSE;
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
    double degrees = M_PI / 180.0;
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
    gtk_widget_set_margin_start(box, 10);

    //avatar
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(input_image_file, NULL);
    GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(pixbuf);
    // prev_pixbuf = resize_img(prev_pixbuf, 150, 150);

    GtkWidget *image = gtk_drawing_area_new();
    gtk_widget_set_halign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(image), gdk_pixbuf_get_width(GDK_PIXBUF(prev_pixbuf)), gdk_pixbuf_get_height(GDK_PIXBUF(prev_pixbuf)));
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

    // GtkWidget *user_box1 = create_user_box("John", "Doe", "@john_doe");
    // GtkWidget *user_box2 = create_user_box("Jane", "Smith", "@jane_smith");
    // gtk_box_pack_start(GTK_BOX(user_list), user_box1, FALSE, FALSE, 0);
    // gtk_box_pack_start(GTK_BOX(user_list), user_box2, FALSE, FALSE, 0);

    for(int i = 0; i < count_chatters(); i++) {
        GtkWidget *user_box = create_user_box(chatters[i].username, chatters[i].lastmsg, default_img);
        gtk_widget_set_name(user_box, "user-box");
        g_signal_connect(user_box, "button-press-event", G_CALLBACK(user_box_clicked), GINT_TO_POINTER(i));
        gtk_box_pack_start(GTK_BOX(user_list), user_box, FALSE, FALSE, 0);
    }

    // for (int i = 0; i < 15; ++i) {
    //     char username[20];
    //     sprintf(username, "User%d", i+1);

    //     GtkWidget *user_box = create_user_box(username, "Xyi", default_img);
    //     gtk_widget_set_name(user_box, "user-box");
    //     g_signal_connect(user_box, "button-press-event", G_CALLBACK(user_box_clicked), NULL);

    //     gtk_box_pack_start(GTK_BOX(user_list), user_box, FALSE, FALSE, 0);
    // }
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

    // for(mes)
    for(int i = 0; i < count_messages(); i++) {
        GtkWidget *mess_box = create_message_box(messages[i].text, messages[i].is_user);
        gtk_box_pack_start(GTK_BOX(mess_list), mess_box, FALSE, FALSE, 0);
    }
}
