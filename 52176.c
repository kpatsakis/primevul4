make_cookie(char *cookie)
{
    static int randomized = 0;
    char hostname[500];
    struct timeval tv;
    char temp[1000];

    if ( ! randomized )
        srandom((int) time(0) ^ getpid());

    /* Generate a string based on hostname, time, randomness, and filler. */
    (void) gethostname(hostname, sizeof(hostname));
    (void) gettimeofday(&tv, 0);
    (void) snprintf(temp, sizeof(temp), "%s.%ld.%06ld.%08lx%08lx.%s", hostname, (unsigned long int) tv.tv_sec, (unsigned long int) tv.tv_usec, (unsigned long int) random(), (unsigned long int) random(), "1234567890123456789012345678901234567890");

    /* Now truncate it to 36 bytes and terminate. */
    memcpy(cookie, temp, 36);
    cookie[36] = '\0';
}
