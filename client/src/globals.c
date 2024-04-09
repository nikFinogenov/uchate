#include "uchat-client.h"
#include "../../libmx/inc/libmx.h"
// GtkWidget *scrollable_window2 =NULL;
GtkWidget *empty_chat = NULL;
GtkWidget *chat_box = NULL;
GtkWidget *chats_box = NULL;
// GtkWidget *chats_box = NULL;
GtkWidget *settings_box = NULL;
GtkWidget *account_settings = NULL;
GtkWidget *user_info_box = NULL;
char* default_img = "client/img/simple.png";

t_message_s** messages = NULL; // Global variable declaration
int messages_count[MAX_CHATTERS];
// for(int i = 0; i < MAX_CHATTERS; i++) messages_count[i] = 0;
// Example initialization of chatters
t_chatter_s* chatters = NULL; // Global variable declaration
int chatters_count = 0;

// Example initialization of user
t_user_s user = {
    .username = NULL,
    .name = NULL,
    .surname = NULL,
    .desc = NULL,
    .avatar = NULL
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
    // Example initialization of messages
    for(int i = 0; i < MAX_CHATTERS; i++) {
        messages[i] = malloc(MAX_MESSAGES * sizeof(t_message_s));
        if (messages[i] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for messages.\n");
            exit(EXIT_FAILURE);
        }
        for(int j = 0; j < MAX_MESSAGES; j++) messages[i][j].text = NULL;
    }
    for(int i = 0; i < MAX_CHATTERS; i++) messages_count[i] = 0;
    // Initialize or populate messages with temporary data here
    // for(int i = 0; i < 2; i++) {
    //     for(int j = 0; j < 10; j++) {
    //         messages[i][j].text = mx_strjoin(mx_itoa(i), ") This is a message.");
    //         messages[i][j].is_user = (i % 2 == 0) ? true : false;
    //         messages_count[i]++;
    //     }
    // }

    // Example initialization of chatters
    chatters = malloc(MAX_CHATTERS * sizeof(t_chatter_s));
    if (chatters == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for chatters.\n");
        // free(messages); // Free previously allocated memory
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < MAX_CHATTERS; i++) {
        chatters[i].username = NULL;
    }
    // Initialize or populate chatters with temporary data here
    // for (int i = 0; i < 2; i++) {
    //     chatters[i].name = "Chatter";
    //     chatters[i].surname = "Surname";
    //     chatters[i].username = "chatter_username";
    //     chatters[i].lastmsg = "Last message from chatter.";
    //     chatters[i].avatar = NULL; // You can initialize the avatar if needed
    // }

    // Example initialization of user
    user.username = "Sidor";
    user.name = "Pidor";
    user.surname = "Pidorovich";
    user.desc = " ";
    user.avatar = NULL;
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
        // Reset values
        user->username = NULL;
        user->name = NULL;
        user->surname = NULL;
        user->desc = NULL;
        user->avatar = NULL;
    }
}

// Function to clear t_chatter_s structure
void clear_chatter(t_chatter_s *chatter) {
    if (chatter) {
        g_free(chatter->name);
        g_free(chatter->surname);
        g_free(chatter->username);
        g_free(chatter->lastmsg);
        if (chatter->avatar)
            g_object_unref(chatter->avatar);
        // Reset values
        chatter->name = NULL;
        chatter->surname = NULL;
        chatter->username = NULL;
        chatter->lastmsg = NULL;
        chatter->avatar = NULL;
    }
}

// Function to clear t_message_s structure
void clear_message(t_message_s *message) {
    if (message) {
        g_free(message->text);
        g_free(message->time);
        // Reset values
        message->text = NULL;
        message->time = NULL;
    }
}

// Function to clear t_selected_s structure
void clear_selected(t_selected_s *selected) {
    if (selected) {
        // Reset values
        selected->box = NULL;
        selected->index = 0;
    }
}

// Function to clear all structures
void clear_all(void) {
    clear_user(&user);
    for (int i = 0; messages[i] != NULL; ++i)
        clear_message(messages[i]);
    g_free(messages);
    messages = NULL;
    for (int i = 0; chatters[i].username != NULL; ++i)
        clear_chatter(&chatters[i]);
    g_free(chatters);
    chatters = NULL;
    clear_selected(&selected_user);
}