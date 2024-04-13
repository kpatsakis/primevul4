is_closed(int fd)
{
    struct timeval tv;
    fd_set readset;

    FD_ZERO(&readset);
    FD_SET(fd, &readset);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    if (select(fd+1, &readset, NULL, NULL, &tv) < 0) {
        if (errno == EBADF)
            return 1;
    }
    return 0;
}
