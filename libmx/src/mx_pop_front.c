#include "../inc/libmx.h"

void mx_pop_front(t_list **head) {
	if (*head == NULL) {
        return;
    }

    t_list *temp = *head;
    *head = (*head)->next;
    free(temp);
}
