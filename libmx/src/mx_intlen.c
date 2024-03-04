int mx_intlen(long n) {
    int length = 0;

    if (n == 0) {
        return 1;
    }

    while (n != 0) {
        n /= 10;
        length++;
    }

    return length;
}
