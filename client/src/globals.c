#include "uchat-client.h"

// GtkWidget *scrollable_window2 =NULL;
GtkWidget *empty_chat = NULL;
GtkWidget *chat_box = NULL;

char* default_img = "client/img/simple.png";

t_message_s* messages = NULL; // Global variable declaration

// Example initialization of chatters
t_chatter_s* chatters = NULL; // Global variable declaration

// Example initialization of user
t_user_s user = {
    .username = NULL,
    .name = NULL,
    .surname = NULL,
    .desc = NULL,
    .avatar = NULL
};

void fill_data(void) {
    // Example initialization of messages
    messages = malloc(MAX_MESSAGES * sizeof(t_message_s));
    if (messages == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for messages.\n");
        exit(EXIT_FAILURE);
    }
    // Initialize or populate messages with temporary data here
    for (int i = 0; i < MAX_MESSAGES; i++) {
        // messages[i].text = ") This is a message.";
        messages[i].text = mx_strjoin(mx_itoa(i), ") This is a message.");
        messages[i].is_user = (i % 2 == 0) ? true : false;
    }

    // Example initialization of chatters
    chatters = malloc(MAX_CHATTERS * sizeof(t_chatter_s));
    if (chatters == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for chatters.\n");
        free(messages); // Free previously allocated memory
        exit(EXIT_FAILURE);
    }
    // Initialize or populate chatters with temporary data here
    for (int i = 0; i < MAX_CHATTERS; i++) {
        chatters[i].name = "Chatter";
        chatters[i].surname = "Surname";
        chatters[i].username = "chatter_username";
        chatters[i].lastmsg = "Last message from chatter.";
        chatters[i].avatar = NULL; // You can initialize the avatar if needed
    }

    // Example initialization of user
    user.username = "Sidor";
    user.name = "Sidor";
    user.surname = "Pidorovich";
    user.desc = " ";
    user.avatar = NULL;
}
// struct User: name, surname, desc, img
// struct Chatter: name, surname, img -> get from chatters by username
// struct Chatters: [Username, lastmsg, img]
// strunt Message_history: [message : [text, is_user]] -> get from chatters by username
void change_messages(void) {
    for (int i = 0; i < MAX_MESSAGES; i++) {
        messages[i].text = "message is this.";
        // messages[i].is_user = (i % 2 == 0) ? true : false;
    }
}