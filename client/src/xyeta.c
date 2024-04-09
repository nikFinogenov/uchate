#include <uchat-client.h>
#include <math.h>

static gboolean draw_penis_cb(GtkWidget *widget, cairo_t *cr, gpointer data) {
    cairo_set_line_width(cr, 2);
    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0); // Red color

    cairo_arc(cr, 35, 93, 15, 0, 2 * M_PI);
    cairo_stroke(cr);

    cairo_arc(cr, 65, 93, 15, 0, 2 * M_PI);
    cairo_stroke(cr);

    cairo_move_to(cr, 40, 30);
    cairo_line_to(cr, 40, 80);
    cairo_move_to(cr, 60, 30); // Remove the bottom line
    cairo_line_to(cr, 60, 80);
    cairo_stroke(cr);

    cairo_arc(cr, 50, 30, 10, 0, 2 * M_PI);
    cairo_stroke(cr);

    cairo_move_to(cr, 40, 30);
    cairo_line_to(cr, 60, 30); // Horizontal line
    cairo_move_to(cr, 50, 30);
    cairo_line_to(cr, 50, 20); // Vertical line
    cairo_stroke(cr);

    return FALSE;
}

GtkWidget *create_penis(void) {
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 150, 150);

    g_signal_connect(G_OBJECT(drawing_area), "draw",
                     G_CALLBACK(draw_penis_cb), NULL);

    return drawing_area;
}


// void *check_messages(void *data) {
//     while (true) {
//         if (sockfd == -1) {
//             mx_connect_to_server(&sockfd);
//             sleep(1);
//             continue;
//         }
//         char sendBuff[256];
//         bzero(sendBuff, 256);
//         sprintf(sendBuff, "CheckMessages\n%d\n%d\n%d", (int)t_user.id, (int)curr_destination, (int)max_msg_id);
        
//         if(send(sockfd, sendBuff, 256, 0) == -1) {
//             pthread_t thread_id;
//             char *err_msg = "Connection lost\nTry again later";
//             pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
//             sockfd = -1;
//             continue;
//         }

//         int latest = max_msg_id;
//         if(recv(sockfd, &latest, sizeof(int), 0) == 0){
//             pthread_t thread_id;
//             char *err_msg = "Connection lost\nTry again later";
//             pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
//             sockfd = -1;
//             continue;
//         }

//         if (latest == max_msg_id) {
//             usleep(1000000);
//             continue;
//         }
//         else if (latest < max_msg_id) {
//             max_msg_id--;
//             gtk_widget_destroy(GTK_WIDGET(t_chat_room_vars.right_container));
//             t_chat_room_vars.right_container = NULL;
//             if (curr_room_msg_head != NULL)
//                 mx_clear_message_list(&curr_room_msg_head);
//             mx_create_messages_area();
//         }
//         else {
//             bzero(sendBuff, 256);
//             sprintf(sendBuff, "LoadMessages\n%d\n%d\n%d", (int)t_user.id, (int)curr_destination, (int)max_msg_id);
//             if(send(sockfd, sendBuff, 256, 0) == -1){
//                 pthread_t thread_id;
//                 char *err_msg = "Connection lost\nTry again later";
//                 pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
//                 sockfd = -1;
//                 continue;
//             }

//             latest = 0;
//             if(recv(sockfd, &latest, sizeof(int), 0) ==0){
//                 pthread_t thread_id;
//                 char *err_msg = "Connection lost\nTry again later";
//                 pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
//                 sockfd = -1;
//                 continue;
//             }

//             char recvBuff[1024];
//             bzero(recvBuff, 1024);
//             for (int i = max_msg_id; i < latest; i++) {
//                 if(recv(sockfd, recvBuff, 1024, 0) ==0){
//                     pthread_t thread_id;
//                     char *err_msg = "Connection lost\nTry again later";
//                     pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
//                     sockfd = -1;
//                     continue;
//                 }

//                 int m_id = 0;
//                 if(recv(sockfd, &m_id, sizeof(int), 0) ==0){
//                     pthread_t thread_id;
//                     char *err_msg = "Connection lost\nTry again later";
//                     pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
//                     sockfd = -1;
//                     continue;
//                 }

//                 char **recvData = mx_strsplit(recvBuff, '\n');
//                 int msg_id = mx_atoi(recvData[0]);
//                 int msg_addresser = mx_atoi(recvData[1]);
//                 int seconds = mx_atoi(recvData[3]);
//                 char *text = NULL;
//                 if (mx_strcmp("(null)", recvData[2]))
//                     text = mx_strdup(recvData[2]);

//                 mx_push_back_message(&curr_room_msg_head, 
//                     text, 
//                     msg_addresser,
//                     mx_read_image_message(m_id),
//                     seconds,
//                     m_id);

//                 mx_del_strarr(&recvData);
//                 bzero(recvBuff, 1024);
//             }
            
//             t_message *msg = mx_message_search(&curr_room_msg_head, max_msg_id + 1);
//             while (msg != NULL) {
//                 mx_add_message(t_chat_room_vars.messages_box, msg);
//                 max_msg_id = msg->id;
//                 msg = msg->next;
//             }
//         }
//     }
// }

