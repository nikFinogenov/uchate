#include "../inc/libmx.h"

int mx_list_size(t_list *list) {
	int count = 0;

    while (list != NULL) {
        count++;
        list = list->next;
    }

    return count;
}
