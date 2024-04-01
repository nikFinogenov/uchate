#include "uchat-client.h"
#include "../../libmx/inc/libmx.h"
// GtkWidget *scrollable_window2 =NULL;
GtkWidget *empty_chat = NULL;
GtkWidget *chat_box = NULL;
GtkWidget *chats_box = NULL;
// GtkWidget *chats_box = NULL;
GtkWidget *settings_box = NULL;
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
    user.name = "Sidor";
    user.surname = "Pidorovich";
    user.desc = " ";
    user.avatar = NULL;
}
void free_chatters() {
    free(chatters);
    chatters = NULL;
}

// struct User: name, surname, desc, img
// struct Chatter: name, surname, img -> get from chatters by username
// struct Chatters: [Username, lastmsg, img]
// strunt Message_history: [message : [text, is_user]] -> get from chatters by username