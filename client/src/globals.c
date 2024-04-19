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
GtkWidget *error_poppi_upe = NULL;
bool is_error_shown = false;
bool ded_is_shown = false;

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
    "Чому програмісти не люблять грати в шахи? \nТому що в них завжди бувають проблеми з коронами!",
    "Why did the scarecrow win an award? Because he was outstanding in his field!",
    "Які у програмістів найпопулярніші танці? Вони називаються 'алгоритмічні кроки'!",
    "Якщо б програмісти були фруктами, то якими б вони були? \nНу, звісно, були б персиками - з завжди відкритою оболонкою!",
    "What did one wall say to the other wall? I'll meet you at the corner!",
    "Why did the tomato turn red? Because it saw the salad dressing!",
    "Чому програмістам подобається літо? Бо там можна повертатися на берег, де всі їх помилки затоплені!",
    "Why did the golfer bring two pairs of pants? In case he got a hole in one!",
    "How does a penguin build its house? Igloos it together!",
    "Яка відмінність між програмістами та дітьми? Діти ростуть, а програмісти збільшуються у своєму коді!",
    "Чому програміст завжди щасливий на роботі? Бо він весь час має доступ до клавіші Esc!",
    "What did the fish say when it hit the wall? Dam!",
     "Якщо ваш програміст говорить, що він завершив усі роботи, \nви можете бути впевнені - це не правда. Він просто зараз видає вам останній баг!",
    "Why was the belt arrested? For holding up a pair of pants!",
    "Чому програміст завжди такий втомлений? Бо він усю ніч марить \nпро ефективний сон, але йому постійно сниться дивні помилки!",
    "Why don't skeletons fight each other? They don't have the guts!",
    "Що зробить програміст, який вийде з-під столу? Скоріш за все, він перезавантажиться!",
    "What did one ocean say to the other ocean? Nothing, they just waved!",
    "Якщо ваш програміст говорить, що він дуже багато працює,\nпитайте його, чи він працює під Windows чи під графіком!",
    "What's orange and sounds like a parrot? A carrot!",
    "Why did the bicycle fall over? Because it was two-tired!",
    "Як програміст вибирає свій одяг? Він перевіряє, чи його рубашка має підтримку CSS!",
    "Why did the tomato turn red? Because it saw the salad dressing!",
    "Чому програмістам так подобається зима? \nТому що там їм не треба перейматися виходом на вулицю - вони просто засинають індексацію!",
    "How do you make holy water? You boil the hell out of it!",
    "Що спільного між програмістом і кавою? Вони обидва бажають\nвам чудового дня, але перш ніж це зробити, вони мусять спочатку завантажитися!",
    "What do you call a fake noodle? An impasta!",
    "Як програміст може посварити? Він просто залишить коментар у вашому коді!",
    "Чому програмісти називають своїх колег по ім'ях? Бо вони завжди прагнуть до ясності у своєму коді!"
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

    gdk_rgba_parse(&black, "#0B0205");
    gdk_rgba_parse(&dark_purple, "#2C1B47");
    gdk_rgba_parse(&not_too_dark_purple, "#724C9D");
    gdk_rgba_parse(&light_purple, "#DCCAE9");
    gdk_rgba_parse(&vrode_norm_purple, "#9356A0");
    gdk_rgba_parse(&ny_takoy_purple, "#604d9e");
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
    for(int i = 0; i < MAX_CHATTERS; i++) {
        if (chatters[i].username != NULL) {
            free(chatters[i].username);
        }
    }
    free(chatters);
    chatters = NULL;
}

void clear_messages(void) {
    for (int i = 0; i < MAX_CHATTERS; i++) {
        if (messages[i] != NULL) {
            for (int j = 0; j < MAX_MESSAGES; j++) {
                if (messages[i][j].text != NULL) {
                    free(messages[i][j].text);
                }
            }
            free(messages[i]);
            messages[i] = NULL;
        }
    }
    free(messages);
    messages = NULL;
}

void clear_data(void) {
    clear_selected(&selected_user);
    for(int i = 0; i < chatters_count; i++) {
        for(int j = 0; j < messages_count[i]; j++) {
            free(messages[i][j].text);
            free(messages[i][j].time);
        }
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
