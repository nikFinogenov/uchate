#include "uchat-client.h"

GtkWidget *empty_chat = NULL;
GtkWidget *chat_box = NULL;
GtkWidget *chats_box = NULL;
GtkWidget *settings_box = NULL;
GtkWidget *account_settings = NULL;
GtkWidget *user_info_box = NULL;
GtkWidget *user_window = NULL;
GtkWidget *login_window = NULL;
GtkWidget *signup_window = NULL;
GdkPixbuf *temp_avatar = NULL;
GtkWidget *scrollable_window = NULL;
GtkWidget *scrollable_window2 = NULL;
GtkWidget *add_new_chat_when_no_chats = NULL;

char *login_info = "client/client-data/login_info.txt";
bool remember;
char* default_img = "client/img/moai.png";

t_message_s** messages;
int messages_count[MAX_CHATTERS];
t_chatter_s* chatters;
int chatters_count;

gint screen_width;
gint screen_height;

t_user_s user = {
    .username = NULL,
    .name = NULL,
    .surname = NULL,
    .desc = NULL,
    .status = NULL,
    .avatar = NULL
};

t_user_data_s userdata = {
    .username = NULL,
    .password = NULL,
    .button_recognize = FALSE
};

const char *jokes[] = {
    "Why don't skeletons fight each other? They don't have the guts!",
    "Why couldn't the bicycle stand up by itself? Because it was two-tired!",
    "Why did the scarecrow win an award? Because he was outstanding in his field!",
    "What do you call fake spaghetti? An impasta!",
    "Why did the math book look sad? Because it had too many problems.",
    "What did one wall say to the other wall? I'll meet you at the corner!",
    "Why did the tomato turn red? Because it saw the salad dressing!",
    "What do you get when you cross a snowman with a vampire? Frostbite!",
    "Why did the golfer bring two pairs of pants? In case he got a hole in one!",
    "How does a penguin build its house? Igloos it together!",
    "What did the janitor say when he jumped out of the closet? Supplies!",
    "Why don't scientists trust atoms? Because they make up everything!",
    "What did the fish say when it hit the wall? Dam!",
    "Why did the coffee file a police report? It got mugged!",
    "Why was the belt arrested? For holding up a pair of pants!",
    "What did one hat say to the other? You stay here, I'll go on ahead!",
    "Why don't skeletons fight each other? They don't have the guts!",
    "What do you call a bear with no teeth? A gummy bear!",
    "What did one ocean say to the other ocean? Nothing, they just waved!",
    "Why did the scarecrow win an award? Because he was outstanding in his field!",
    "What's orange and sounds like a parrot? A carrot!",
    "Why did the bicycle fall over? Because it was two-tired!",
    "What did one snowman say to the other snowman? Do you smell carrots?",
    "Why did the tomato turn red? Because it saw the salad dressing!",
    "Why did the golfer bring two pairs of pants? In case he got a hole in one!",
    "How do you make holy water? You boil the hell out of it!",
    "Why don't eggs tell jokes? Because they'd crack each other up!",
    "What do you call a fake noodle? An impasta!",
    "Why did the math book look sad? Because it had too many problems.",
    "What do you call a can opener that doesn't work? A can't opener!"
};

bool is_chatters_empty(void) {
    for (int i = 0; i < MAX_CHATTERS; i++) {
        if (chatters[i].username != NULL) {
            return FALSE;
        }
    }
    return TRUE;
}

bool is_messages_empty(void) {
    for (int i = 0; i < MAX_CHATTERS; i++) {
        for(int j = 0; j < MAX_MESSAGES; j++){
            if (messages[i][j].text != NULL) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

void fill_data(void) {
    messages = malloc(MAX_CHATTERS * sizeof(t_message_s*));
    if (messages == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for messages array.\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < MAX_CHATTERS; i++) {
        messages[i] = malloc(MAX_MESSAGES * sizeof(t_message_s));
        if (messages[i] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for messages.\n");
            exit(EXIT_FAILURE);
        }
        for(int j = 0; j < MAX_MESSAGES; j++) messages[i][j].text = NULL;
    }
    for(int i = 0; i < MAX_CHATTERS; i++) messages_count[i] = 0;

    chatters = malloc(MAX_CHATTERS * sizeof(t_chatter_s));
    if (chatters == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for chatters.\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < MAX_CHATTERS; i++) {
        chatters[i].username = NULL;
    }

    user.username = "";
    user.name = "";
    user.surname = "";
    user.desc = "";
    user.avatar = NULL;

    chatters_count = 0;
}

void free_chatters() {
    free(chatters);
    chatters = NULL;
}

void clear_user(t_user_s *user) {
    if (user) {
        g_free(user->username);
        g_free(user->name);
        g_free(user->surname);
        g_free(user->desc);
        if (user->avatar)
            g_object_unref(user->avatar);

        user->username = NULL;
        user->name = NULL;
        user->surname = NULL;
        user->desc = NULL;
        user->avatar = NULL;
    }
}

void clear_chatter(t_chatter_s *chatter) {
    if (chatter) {
        g_free(chatter->name);
        g_free(chatter->surname);
        g_free(chatter->username);
        g_free(chatter->lastmsg);
        if (chatter->avatar)
            g_object_unref(chatter->avatar);

        chatter->name = NULL;
        chatter->surname = NULL;
        chatter->username = NULL;
        chatter->lastmsg = NULL;
        chatter->avatar = NULL;
    }
}

void clear_message(t_message_s *message) {
    free(message->text);
    free(message->time);
    message->id = 0;
    message->text = NULL;
    message->time = NULL;
    message->is_user = false;
}

void clear_selected(t_selected_s *selected) {
    if (selected) {
        selected->box = NULL;
        selected->index = -1;
    }
}

void clear_all(void) {
    clear_user(&user);
    for (int i = 0; i < MAX_CHATTERS; ++i){
        for(int j = 0; messages[i][j].text != NULL; j++) clear_message(&messages[i][j]);
    }
    for(int i = 0; i < MAX_CHATTERS; i++) messages_count[i] = 0;
    g_free(messages);
    messages = NULL;
    for (int i = 0; chatters[i].username != NULL; ++i)
        clear_chatter(&chatters[i]);
    g_free(chatters);
    chatters = NULL;
    chatters_count = 0;
    clear_selected(&selected_user);
}

void clear_chats(void) {
    for(int i = 0; i < chatters_count; i++) {
        free(chatters[i].name);
        free(chatters[i].surname);
        free(chatters[i].username);
        free(chatters[i].lastmsg);
        if (chatters[i].avatar != NULL) gdk_pixbuf_unref(chatters[i].avatar);
    }
    free(chatters);
    chatters_count = 0;
}

void clear_messages(void){
    for(int i = 0; i < chatters_count; i++) {
        free(messages[i]);
    }
    free(messages);
    for(int i = 0; i < MAX_CHATTERS; i++) {
        messages_count[i] = 0;
    }
}

void clear_data(void) {
    clear_selected(&selected_user);
    for(int i = 0; i < chatters_count; i++) {
        free(messages[i]);
    }
    free(messages);

    for(int i = 0; i < chatters_count; i++) {
        free(chatters[i].name);
        free(chatters[i].surname);
        free(chatters[i].username);
        free(chatters[i].lastmsg);
        if (chatters[i].avatar != NULL) gdk_pixbuf_unref(chatters[i].avatar);
    }
    free(chatters);

    free(user.name);
    free(user.surname);
    free(user.username);
    free(user.desc);
    gdk_pixbuf_unref(user.avatar);

    user.username = "";
    user.name = "";
    user.surname = "";
    user.desc = "";
    user.avatar = NULL;

    chatters_count = 0;
    for(int i = 0; i < MAX_CHATTERS; i++) {
        messages_count[i] = 0;
    }
}
