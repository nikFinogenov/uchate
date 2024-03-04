#include "../inc/libmx.h"

t_list *mx_sort_list(t_list *lst, bool (*cmp)(void *, void *)) {
   if (lst == NULL || cmp == NULL) {
        return NULL;
    }

    bool swapped;
    t_list *temp;

    do {
        swapped = false;
        t_list *current = lst;

        while (current != NULL && current->next != NULL) {
            if (cmp(current->data, current->next->data)) {
                temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                swapped = true;
            }

            current = current->next;
        }
    } while (swapped);

    return lst;
}
