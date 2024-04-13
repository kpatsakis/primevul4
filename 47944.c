local void copymeta(char *from, char *to)
{
    struct stat st;
    struct timeval times[2];

    /* get all of from's Unix meta data, return if not a regular file */
    if (stat(from, &st) != 0 || (st.st_mode & S_IFMT) != S_IFREG)
        return;

    /* set to's mode bits, ignore errors */
    (void)chmod(to, st.st_mode & 07777);

    /* copy owner's user and group, ignore errors */
    (void)chown(to, st.st_uid, st.st_gid);

    /* copy access and modify times, ignore errors */
    times[0].tv_sec = st.st_atime;
    times[0].tv_usec = 0;
    times[1].tv_sec = st.st_mtime;
    times[1].tv_usec = 0;
    (void)utimes(to, times);
}
