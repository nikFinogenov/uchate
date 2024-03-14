#include "../inc/libmx.h"

int mx_sqrt(int x) {

    if (x <= 0) {
        return 0; 
    }

    int left_side = 1;
    int right_side = x;
    int result = 0;

    while (left_side <= right_side) {
        int mid = left_side + (right_side - left_side) / 2;
        if (mid <= x / mid) {
            left_side = mid + 1;
            result = mid;
        } else {
            right_side = mid - 1;
        }
    }

    if (result * result == x) {
        return result;
    } else {
        return 0;
    }
}
